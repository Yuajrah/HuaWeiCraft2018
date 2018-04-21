//
// Created by txink on 18-3-23.
//

#include "packing.h"
#include <cstring>
#include "BasicInfo.h"

packing::packing(Server server, std::map<int, int> vm_num, std::map<int,Vm> vm_info, int vmtypenum){
    this->server = server;
    this->vm_num = vm_num;
    this->vm_info = vm_info;
    vm_typenum = vmtypenum;
    paramA[0] = 0;
    paramA[1] = 0;
    score[0] = 0;
    score[1] = 0;
}


packing::packing(Server server, std::map<int, int> vm_num, std::map<int, Vm> vm_info) {
    packing(server,vm_num,vm_info,18);
}


packing::packing(Server server, std::map<int, int> vm_num){
    packing(server,vm_num,BasicInfo::vm_info,18);
}

void packing::set_paramA() {
    int count = 0;
    paramA[0] = 0;
    paramA[1] = 0;
    for(auto &t: vm_num)
    {
        paramA[0] += (vm_info[t.first].core * t.second);
        paramA[1] += (vm_info[t.first].mem * t.second);
        count += t.second;
    }
    paramA[0] = paramA[0] * 1.0 / count;
    paramA[1] = paramA[1] * 1.0 / count;
}



std::vector<std::map<int,int>> packing::pack_item() {
     return pack_item(server, vm_num, 2);
}


std::vector<std::map<int,int>> packing::pack_item(Server ser, std::map<int, int> vnum, int type) {
    std::vector<std::map<int,int>>result_record_1;
    result_record_1 = pack_base(ser, vnum, type);
    return  result_record_1;
}


std::vector<std::map<int,int>> packing::pack_item_compareA(Server ser, std::map<int, int> vnum) {
    std::vector<std::map<int,int>>result_record_1;
    std::vector<std::map<int,int>>result_record_2;
    result_record_1 = pack_base(ser, vnum, 1);
    result_record_2 = pack_base(ser, vnum, 2);

    if(score[0] >= score[1]){
        return result_record_2;
    }else{
        return  result_record_1;
    }
}


std::vector<std::map<int,int>> packing::pack_base(Server server, std::map<int, int> vmnum, int type) {
    std::map<int, int> vm_num_copy = vmnum;

    //保存最终结果的map，vector中的ID对应编号为多少的分配结果
    std::vector<std::map<int,int>>result_record;
    // 初始化服务器节点
    int server_number = 0;
    std::priority_queue<Allocat_server> allocate_result;
    //归一化系数
    set_paramA();

    //用来保存虚拟机的剩余数量，反向对应，比如tmp_vm_num[vm_typenum]对应vm1，tmp_vm_num[1]对应vm_typenum
    std::vector<int> tmp_vm_num(vm_typenum+1,0);
    bool is_vm_empty;
    for(int i=1; i<=vm_typenum; i++){
        std::map<int, int>::iterator iter;
        iter = vmnum.find(vm_typenum+1-i);
        if(iter == vmnum.end()){
            tmp_vm_num[i] = 0;
        }else {
            tmp_vm_num[i] = iter->second;
        }
    }
    is_vm_empty = check_vmnum_empty(tmp_vm_num);
    /*对于预测文档的数据进行分配,每次装满一个二维多重背包，背包容量分别为服务器CPU核数U和MEM大小V(转化为GB)，物品数量最大值N为vm_typenum，
     * 物品费用分别为CPU核数和MEM大小(转化为GB)，状态转移数组dp[N+1][max{U+1,V+1}]，同时利用一个二维数组记录选择了哪些物品used[U+1][V+1],
     * 最后通过读取use[U][V]的值来确定物品选择了多少件
    */
    //predict_data:first为id,second为数量，将为0的项去除，predict_data视为剩余未分配量
    while(!is_vm_empty)
    {
        //首先初始化一个节点
        Allocat_server new_server = allocate_oneserver(server_number, server.core, server.mem);
        allocate_result.push(new_server);
        server_number++;

        //清除预测数据中value为0的项
        for(int i = vm_typenum; i>0; i--) {
            std::map<int, int>::iterator iter;
            iter = vmnum.find(i);
            if(iter == vmnum.end()){
                continue;
            }else if(iter->second == 0) {
                vmnum.erase(iter);
            }
        }

        std::map<int ,int >::iterator current_flavor = vmnum.begin();
        std::map<int,int> new_record;
        std::vector<std::vector <int> > dp (server.core+1, std::vector<int>(server.mem+1,0));
        std::vector<std::vector<std::vector<int> > > used(vm_typenum+1, std::vector<std::vector<int> >(server.core+1, std::vector<int>(server.mem+1,0)));


        //一次二维多重背包循环,pos表示前pos个物品
        for(int pos = 1; pos <= vm_typenum; pos++){
            //获取当前虚拟机的CPU和MEM限制，同时当前虚拟机id为current_flavor->first
            std::map<int, Vm>::iterator current_flavor_info;
            current_flavor_info =  vm_info.find(vm_typenum-pos);
            int core_need = current_flavor_info->second.core;
            int mem_need = current_flavor_info->second.mem;
            int item_value;
            if(type == 1){
                item_value = core_need*paramA[0] + mem_need*paramA[1];//物品价值
            }else if(type == 2){
                item_value =core_need + mem_need;//物品价值
            }
            int item_num = tmp_vm_num[pos];//可用的物品数量

            //void MultiplePack(int C, int D, int U, int V, int W, int M);
            //C表示物品费用1，D物品费用2，U背包费用1容量，V背包费用2容量，W物品价值，M物品数量
            MultiplePack(dp, used, core_need, mem_need, server.core, server.mem, item_value, item_num, pos);

        }
        std::vector<int> choose_vm_num;
        choose_vm_num = get_path(used, vm_info, server.core, server.mem);
        //处理数据，tmp_vm_num数据更新，同时对predict_data数据更新
        for(int i=1; i<=vm_typenum; i++){
            if(choose_vm_num[vm_typenum+1 - i] != 0){
                new_record[i] = choose_vm_num[vm_typenum+1 - i];
            }

            std::map<int ,int >::iterator iter;
            iter = vmnum.find(vm_typenum+1 - i);
            iter->second -= choose_vm_num[vm_typenum+1 - i];
            tmp_vm_num[i] -= choose_vm_num[i];
        }

        result_record.push_back(new_record);
        is_vm_empty = check_vmnum_empty(tmp_vm_num);
    }

    if(type == 1){
        score[0] = get_score(vm_num_copy, server, server_number);
    }else{
        score[1] = get_score(vmnum, server, server_number);
    }

    return result_record;

}

bool packing::check_vmnum_empty(std::vector<int> &temp) {
    bool isempty = true;
    for(int i=1; i<=vm_typenum; i++){
        if(temp[i] != 0){
            isempty = false;
        }
    }
    return isempty;
}


std::vector<int> packing::get_path(std::vector<std::vector<std::vector<int> > > &used, std::map<int,Vm> vminfo, int U, int V)
{
    //vm_typenum种物品选择了哪些
    std::vector<int> choose_num(vm_typenum+1, 0);
    std::map<int, Vm>::iterator current_flavor_info;
    for(int i=1; i<=vm_typenum; i++){
        choose_num[i] = used[i][U][V];
    }
    return choose_num;
}


void packing::CompletePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int pos)
{

    for(int u=C; u<=U; u++)
    {
        for(int v=D; v<=V; v++){
            if(C <= u && D <= v){
                //第i个物品可放入
                if(dp[u][v] < dp[u-C][v-D]+W){
                    dp[u][v] =  dp[u-C][v-D]+W;
                    for(int k=1; k<=vm_typenum; k++){
                        used[k][u][v] = used[k][u-C][v-D];
                    }
                    used[pos][u][v] += 1;
                }
            }
        }

    }

}

void packing::ZeroOnePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos)
{
  //  std::cout<<"ZeroPack("<<C*M<<", "<<D*M<<", "<<W*M<<", "<<pos<<")"<<std::endl;
    for(int u=U; u>=C*M; u--)
    {
        for(int v=V; v>=D*M; v--){
            if(C*M <= u && D*M <= v) {
                //第i个物品可放入
                if (dp[u][v] < dp[u - C * M][v - D * M] + W * M) {
                    dp[u][v] = dp[u - C * M][v - D * M] + W * M;
                    for(int k=1; k<=vm_typenum; k++){
                        used[k][u][v] = used[k][u-C*M][v-D*M];
                    }
                    used[pos][u][v] += M;
                }
            }
        }

    }


}

void packing::MultiplePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos)
{
    //物品容量大于背包容量，直接返回
    if(C >= U || D >= V)
        return;

    //物品数量为0，直接返回
    if(M <= 0)
        return;


    if(C*M >= U && D*M >= V){
        CompletePack(dp, used, C, D, U, V, W, pos);
        return;
    }

    for(int k=1; k<=M; k*=2)
    {
        ZeroOnePack(dp, used, C, D, U, V, W, k, pos);
        M = M - k;
    }
    if(M){
        ZeroOnePack(dp, used, C, D, U, V, W, M, pos);
    }
}


Allocat_server packing::allocate_oneserver(int id, int core, int mem)
{
    Allocat_server new_allocate;
    new_allocate.id = id;
    new_allocate.core = core;
    new_allocate.mem = mem;
    return new_allocate;
}

double packing::get_score(std::map<int, int>vmnum, Server sv, int sv_num){
    int total_allocate_cpu,total_allocate_mem;
    total_allocate_cpu = sv.core * sv_num;
    total_allocate_mem = sv.mem *sv_num;

    int total_need_cpu = 0;
    int total_need_mem = 0;
    for (int i = 1; i<=vm_typenum; i++)
    {
        std::map<int ,int >::iterator iter;
        iter = vmnum.find(i);
        if (iter == vmnum.end())
        {
            continue;
        }
        else
        {
            int target_need_cpu = 0;
            int target_need_mem = 0;
            std::map<int, Vm>::iterator current_flavor_info;
            current_flavor_info =  vm_info.find(i);

            target_need_cpu = current_flavor_info->second.core;
            target_need_mem = current_flavor_info->second.mem;

            total_need_cpu += iter->second * target_need_cpu;
            total_need_mem += iter->second * target_need_mem;
        }
    }
    double percent = ((total_need_cpu+0.0)/total_allocate_cpu + (total_need_mem+0.0)/total_allocate_mem)*0.5;
    printf("allocate score = %f\n", percent);
    return percent;
}

