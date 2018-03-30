//
// Created by txink on 18-3-23.
//

#include "packing.h"
#include <cstring>


std::vector<std::map<int,int>> packing(std::map<int,Vm> vm_info, Server server, std::map<int, int> predict_data, char *opt_object){
    std::map<int, int> predict_data_tmp = predict_data;
    //首先确定优化目标
    int target;
    const char * target_string  = "CPU\n";
    if (strcmp(target_string, opt_object) == 0){
        target = 0;//CPU
    }else{
        target = 1;//MEM
    }

    //保存最终结果的map，vector中的ID对应编号为多少的分配结果
    std::vector<std::map<int,int>>result_record;
    // 初始化服务器节点
    int server_number = 0;
    std::priority_queue<Allocat_server> allocate_result;


    //用来保存虚拟机的剩余数量，反向对应，比如tmp_vm_num[15]对应vm1，tmp_vm_num[1]对应vm15
    std::vector<int> tmp_vm_num(16,0);
    bool is_vm_empty;
    for(int i=1; i<=15; i++){
        std::map<int, int>::iterator iter;
        iter = predict_data.find(16-i);
        if(iter == predict_data.end()){
            tmp_vm_num[i] = 0;
        }else {
            tmp_vm_num[i] = iter->second;
        }

    }

    is_vm_empty = check_vmnum_empty(tmp_vm_num);
    /*对于预测文档的数据进行分配,每次装满一个二维多重背包，背包容量分别为服务器CPU核数U和MEM大小V(转化为GB)，物品数量最大值N为15，
     * 物品费用分别为CPU核数和MEM大小(转化为GB)，状态转移数组dp[N+1][max{U+1,V+1}]，同时利用一个二维数组记录选择了哪些物品used[U+1][V+1],
     * 最后通过读取use[U][V]的值来确定物品选择了多少件
    */
    //predict_data:first为id,second为数量，将为0的项去除，predict_data视为剩余未分配量
    while(!is_vm_empty)
    {
        //首先初始化一个节点
        Allocat_server new_server = allocate_oneserver(server_number, server.core, server.mem, target);
        allocate_result.push(new_server);
        server_number++;

        //清除预测数据中value为0的项
        for(int i = 15; i>0; i--) {
            std::map<int, int>::iterator iter;
            iter = predict_data.find(i);
            if(iter == predict_data.end()){
                continue;
            }else if(iter->second == 0) {
                predict_data.erase(iter);
            }
        }


        std::map<int ,int >::iterator current_flavor = predict_data.begin();
        std::map<int,int> new_record;
        std::vector<std::vector <int> > dp (server.core+1, std::vector<int>(server.mem+1,0));
        std::vector<std::vector<std::vector<int> > > used(16, std::vector<std::vector<int> >(server.core+1, std::vector<int>(server.mem+1,0)));
       // std::vector<int> path_cpu(PACKING_N, 0);
       // std::vector<int> path_mem(PACKING_N, 0);


        //一次二维多重背包循环,pos表示前pos个物品
        for(int pos = 1; pos <= 15; pos++){
            //获取当前虚拟机的CPU和MEM限制，同时当前虚拟机id为current_flavor->first
            std::map<int, Vm>::iterator current_flavor_info;
            current_flavor_info =  vm_info.find(16-pos);
            int core_need = current_flavor_info->second.core;
            int mem_need = current_flavor_info->second.mem/1024;
            int item_value = core_need + mem_need;//物品价值
            int item_num = tmp_vm_num[pos];//可用的物品数量

            //void MultiplePack(int C, int D, int U, int V, int W, int M);
            //C表示物品费用1，D物品费用2，U背包费用1容量，V背包费用2容量，W物品价值，M物品数量
            MultiplePack(dp, used, core_need, mem_need, server.core, server.mem, item_value, item_num, pos);

        }
        std::vector<int> choose_vm_num;
        choose_vm_num = get_path(used, vm_info, server.core, server.mem);
        //处理数据，tmp_vm_num数据更新，同时对predict_data数据更新
        for(int i=1; i<=15; i++){
            if(choose_vm_num[16 - i] != 0){
                new_record[i] = choose_vm_num[16 - i];
            }

            std::map<int ,int >::iterator iter;
            iter = predict_data.find(16 - i);
            iter->second -= choose_vm_num[16 - i];
            tmp_vm_num[i] -= choose_vm_num[i];
        }

        result_record.push_back(new_record);
        is_vm_empty = check_vmnum_empty(tmp_vm_num);
    }
    get_scores_p(predict_data_tmp, server, server_number, target, vm_info);
    return result_record;
}

bool check_vmnum_empty(std::vector<int> &temp)
{
    bool isempty = true;
    for(int i=1; i<=15; i++){
        if(temp[i] != 0){
            isempty = false;
        }
    }
    return isempty;
}


std::vector<int> get_path(std::vector<std::vector<std::vector<int> > > &used, std::map<int,Vm> vminfo, int U, int V)
{
    //15种物品选择了哪些
    std::vector<int> choose_num(16, 0);
    std::map<int, Vm>::iterator current_flavor_info;
    for(int i=1; i<=15; i++){
        choose_num[i] = used[i][U][V];
    }

//    //path_CPU = pos*PACKING_N + pos - C*M  所以s1 = pos , s2 = pos - C*M
//    //path_MEM = pos*PACKING_N + pos -D*M  所以s1 = pos , s2 = pos - D*M
//    int sum = U+V;
//    while(sum){
//        int s1 = path[sum]/PACKING_N;
//        int s2 = path[sum]%PACKING_N;
//        current_flavor_info =  vminfo.find(s1);
//        choose_num[s1] = (sum - s2) / (current_flavor_info->second.core + current_flavor_info->second.mem/1024);
//        sum = s2;
//    }
    return choose_num;
}

void CompletePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int pos)
{
//    for v <-- C to V
//        F(v) <-- max{F(v), F(v-C)+W}
 //   std::cout<<"CompetePack("<<C<<", "<<D<<", "<<W<<", "<<pos<<")"<<std::endl;
    for(int u=C; u<=U; u++)
    {
        for(int v=D; v<=V; v++){
            if(C <= u && D <= v){
                //第i个物品可放入
                if(dp[u][v] < dp[u-C][v-D]+W){
                    dp[u][v] =  dp[u-C][v-D]+W;
                    for(int k=1; k<=15; k++){
                        used[k][u][v] = used[k][u-C][v-D];
                    }
                    used[pos][u][v] += 1;
                }
            }
        }

    }

}

void ZeroOnePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos)
{
  //  std::cout<<"ZeroPack("<<C*M<<", "<<D*M<<", "<<W*M<<", "<<pos<<")"<<std::endl;
    for(int u=U; u>=C*M; u--)
    {
        for(int v=V; v>=D*M; v--){
            if(C*M <= u && D*M <= v) {
                //第i个物品可放入
                if (dp[u][v] < dp[u - C * M][v - D * M] + W * M) {
                    dp[u][v] = dp[u - C * M][v - D * M] + W * M;
                    for(int k=1; k<=15; k++){
                        used[k][u][v] = used[k][u-C*M][v-D*M];
                    }
                    used[pos][u][v] += M;
                }
            }
        }

    }


}

void MultiplePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos)
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




Allocat_server allocate_oneserver(int id, int core, int mem, int target)
{
    Allocat_server new_allocate;
    new_allocate.id = id;
    new_allocate.core = core;
    new_allocate.mem = mem;
    new_allocate.target = target;
    return new_allocate;
}

void get_scores_p(std::map<int, int>predict_data, Server server, int number, int target, std::map<int, Vm> vm_info)
{
    int total_allocate;
    if (target == 0)
    {
        total_allocate = server.core * number;
    }
    else{
        total_allocate = server.mem *number;
    }
    int total_need = 0;
    for (int i = 1; i<=15; i++)
    {
        std::map<int ,int >::iterator iter;
        iter = predict_data.find(i);
        if (iter == predict_data.end())
        {
            continue;
        }
        else
        {
            int target_need = 0;
            std::map<int, Vm>::iterator current_flavor_info;
            current_flavor_info =  vm_info.find(i);;
            if (target == 0)
            {
                target_need = current_flavor_info->second.core;
            }
            else
            {
                target_need = current_flavor_info->second.mem;
            }
            total_need += iter->second * target_need;
        }
    }
    double percent = (total_need+0.0)/total_allocate;
    printf("allocate score = %f\n", percent);
}