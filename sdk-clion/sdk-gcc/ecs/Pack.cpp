//
// Created by txink on 18-4-23.
//

#include "Pack.h"

//构造函数
Pack::Pack(std::map<int, Server> servers, std::map<int, int> vm_num, std::map<int, Vm> vm_info, int vm_typenum) {
    this->Servers = servers;
    this->vmNum = vm_num;
    this->vmInfo = vm_info;
    this->paramA[0] = 1;
    this->paramA[1] = 1;
    this->vmTypeNum = vm_typenum;
    this->finalScore = -1;
    this->onceScore = -1;
    this->bestServerType = -1;
}

//设置价值系数
void Pack::setA(int paramType) {
    //cpu+mem
    if(paramType == 0){
        paramA[0] = 1;
        paramA[1] = 1;
    }//k1*cpu+k2*mem
    else if(paramType == 1){
        int count = 0;
        for(auto &t: vmNum){
            paramA[0] += (vmInfo[t.first].core * t.second);
            paramA[1] += (vmInfo[t.first].mem * t.second);
            count += t.second;
        }
        paramA[0] = paramA[0] * 1.0 /count;
        paramA[1] = paramA[1] * 1.0 /count;
    }
}

//获取map中指定位置的值
int getItemNum(std::map<int, int> vm_num, int index){
    std::map<int, int>::iterator iter;
    iter = vm_num.find(index);
    if(iter == vm_num.end()){
        return 0;
    }else{
        return iter->second;
    }
}

//创建一个新Server
Server allocateOneServer(int type, int core, int mem)
{
    Server new_allocate;
    new_allocate.type = type;
    new_allocate.core = core;
    new_allocate.mem = mem;
    return new_allocate;
}


std::vector<std::map<int,int>> Pack::packStepBest(std::vector<Server> &serverResult) {

    std::map<int, int> vmNumTmp = vmNum;
    //保存最终结果的map，vector中的ID对应编号为多少的分配结果
    std::vector<std::map<int,int>>result_record;
    // 初始化服务器节点
    int server_number = 0;
    //归一化系数,参数为0或1，0表示cpu+mem，1表示k1*cpu+k2*mem
    setA(0);
    bool is_vm_empty;
    is_vm_empty = checkEmpty(vmNumTmp);


    while(!is_vm_empty)
    {
        //清除预测数据中value为0的项
        for(int i = 1; i<= vmTypeNum; i++) {
            std::map<int, int>::iterator iter;
            iter = vmNumTmp.find(i);
            if(iter == vmNumTmp.end()){
                continue;
            }else if(iter->second == 0) {
                vmNumTmp.erase(iter);
            }
        }

        //单次背包函数
        std::map<int,int> new_record = packOnceBest(vmNumTmp);

        //处理数据，对vmNumTmp数据更新
        for(int i=1; i<=vmTypeNum; i++) {
            std::map<int, int>::iterator iter;
            iter = new_record.find(i);
            if (iter == new_record.end()) {
                continue;
            } else if (iter->second != 0) {
                std::map<int, int>::iterator itert;
                itert = vmNumTmp.find(i);
                itert->second -= iter->second;
            }
        }
        //首先初始化一个节点
        Server sv = Servers[bestServerType];
        Server new_server = allocateOneServer(sv.type, sv.core, sv.mem);
        serverResult.push_back(new_server);
        server_number++;
        result_record.push_back(new_record);
        is_vm_empty = checkEmpty(vmNumTmp);
    }
    finalScore = getFinalScore(serverResult, vmNum);
    return result_record;
}


std::map<int,int> Pack::packOnceBest(std::map<int, int> vm_num) {
    std::map<int,int> record1;
    std::map<int,int> record;
    double max = 0;
    int index = -1;
    for(int i=0; i<3; i++){
        record1 = packOnce(vm_num,Servers[i]);
        if(onceScore > max){
            max = onceScore;
            index = i;
            record = record1;
        }
    }
    printf("chooseScore:%f   serverType:%d\n",max,index);
    if(index == 0){
        bestServerType = 0;
        return record;
    }else if(index == 1){
        bestServerType = 1;
        return  record;
    } else if(index == 2){
        bestServerType = 2;
        return  record;
    }
}


std::map<int,int> Pack::packOnce(std::map<int, int> vm_num, Server server) {
    //声明存储数据的数组
    std::map<int ,int >::iterator current_flavor = vm_num.begin();
    std::map<int,int> new_record;
    std::vector<std::vector <int> > dp (server.core+1, std::vector<int>(server.mem+1,0));
    std::vector<std::vector<std::vector<int> > > used(vmTypeNum+1, std::vector<std::vector<int> >(server.core+1, std::vector<int>(server.mem+1,0)));

    //一次二维多重背包循环,pos表示前pos个物品
    for(int pos = 1; pos <= vmTypeNum; pos++){
        //获取当前虚拟机的CPU和MEM限制，同时当前虚拟机id为current_flavor->first
        std::map<int, Vm>::iterator current_flavor_info;
        current_flavor_info =  vmInfo.find(vmTypeNum+1-pos);
        int core_need = current_flavor_info->second.core;
        int mem_need = current_flavor_info->second.mem;
        int item_value=core_need + mem_need;//物品价值;
        int item_num = getItemNum(vm_num, vmTypeNum+1-pos);//可用的物品数量

        //void MultiplePack(int C, int D, int U, int V, int W, int M);
        //C表示物品费用1，D物品费用2，U背包费用1容量，V背包费用2容量，W物品价值，M物品数量
        MultiplePack(dp, used, core_need, mem_need, server.core, server.mem, item_value, item_num, pos);
    }

    std::vector<int> choose_vm_num;
    choose_vm_num = getPath(used, vmInfo, server.core, server.mem);
    //处理数据，tmp_vm_num数据更新，同时对predict_data数据更新
    for(int i=1; i<=vmTypeNum; i++){
        if(choose_vm_num[vmTypeNum+1 - i] != 0){
            new_record[i] = choose_vm_num[vmTypeNum+1 - i];
        }
    }
    onceScore = getOnceScore(server,new_record);
    return new_record;
}



//判断是否还有可分配的VM
bool Pack::checkEmpty(std::map<int, int> vmnum) {
    bool isempty = true;
    for(int i=1; i<=vmTypeNum; i++) {
        std::map<int, int>::iterator iter;
        iter = vmnum.find(i);
        if (iter == vmnum.end()) {
            continue;
        } else{
            if(iter->second != 0){
                isempty = false;
            }
        }
    }
    return isempty;
}

//获取选择的物品
std::vector<int> Pack::getPath(std::vector<std::vector<std::vector<int> > > &used, std::map<int, Vm> vminfo, int U,
                               int V) {
    //N种物品选择了哪些
    std::vector<int> choose_num(vmTypeNum+1, 0);
    std::map<int, Vm>::iterator current_flavor_info;
    for(int i=1; i<=vmTypeNum; i++){
        choose_num[i] = used[i][U][V];
    }
    return choose_num;
}

//获取最终得分
double Pack::getFinalScore(std::vector<Server> serverResult, std::map<int, int> vm_num) {
    int total_allocate_cpu = 0;
    int total_allocate_mem = 0;

    for(int i=0; i<serverResult.size(); i++){
        total_allocate_cpu += serverResult[i].core;
        total_allocate_mem += serverResult[i].mem;
    }

    int total_need_cpu = 0;
    int total_need_mem = 0;
    for (int i = 1; i<=18; i++)
    {
        std::map<int ,int >::iterator iter;
        iter = vm_num.find(i);
        if (iter == vm_num.end())
        {
            continue;
        }
        else
        {
            int target_need_cpu = 0;
            int target_need_mem = 0;
            std::map<int, Vm>::iterator current_flavor_info;
            current_flavor_info =  vmInfo.find(i);;

            target_need_cpu = current_flavor_info->second.core;
            target_need_mem = current_flavor_info->second.mem;

            total_need_cpu += iter->second * target_need_cpu;
            total_need_mem += iter->second * target_need_mem;
        }
    }
    double percent = ((total_need_cpu+0.0)/total_allocate_cpu + (total_need_mem+0.0)/total_allocate_mem)*0.5;
    printf("Final allocate score = %f\n", percent);
    return percent;
}


//获取一次背包的得分
double Pack::getOnceScore(Server server, std::map<int, int> record) {
    int total_need_cpu = 0;
    int total_need_mem = 0;
    for (int i = 1; i<=vmTypeNum; i++)
    {
        std::map<int ,int >::iterator iter;
        iter = record.find(i);
        if (iter == record.end())
        {
            continue;
        }
        else
        {
            int target_need_cpu = 0;
            int target_need_mem = 0;
            std::map<int, Vm>::iterator current_flavor_info;
            current_flavor_info =  vmInfo.find(i);;
            target_need_cpu = current_flavor_info->second.core;
            target_need_mem = current_flavor_info->second.mem;

            total_need_cpu += iter->second * target_need_cpu;
            total_need_mem += iter->second * target_need_mem;

        }
    }
    double percent = ((total_need_cpu+0.0)/server.core + (total_need_mem+0.0)/server.mem)*0.5;
    printf("OnceScore = %f\n", percent);
    return percent;

}


void Pack::CompletePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int pos)
{

    for(int u=C; u<=U; u++)
    {
        for(int v=D; v<=V; v++){
            if(C <= u && D <= v){
                //第i个物品可放入
                if(dp[u][v] < dp[u-C][v-D]+W){
                    dp[u][v] =  dp[u-C][v-D]+W;
                    for(int k=1; k<=18; k++){
                        used[k][u][v] = used[k][u-C][v-D];
                    }
                    used[pos][u][v] += 1;
                }
            }
        }
    }
}
void Pack::ZeroOnePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos)
{
    //  std::cout<<"ZeroPack("<<C*M<<", "<<D*M<<", "<<W*M<<", "<<pos<<")"<<std::endl;
    for(int u=U; u>=C*M; u--)
    {
        for(int v=V; v>=D*M; v--){
            if(C*M <= u && D*M <= v) {
                //第i个物品可放入
                if (dp[u][v] < dp[u - C * M][v - D * M] + W * M) {
                    dp[u][v] = dp[u - C * M][v - D * M] + W * M;
                    for(int k=1; k<=18; k++){
                        used[k][u][v] = used[k][u-C*M][v-D*M];
                    }
                    used[pos][u][v] += M;
                }
            }
        }
    }
}
void Pack::MultiplePack(std::vector<std::vector<int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C,
                        int D, int U, int V, int W, int M, int pos) {
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


