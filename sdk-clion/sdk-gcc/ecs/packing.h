//
// Created by txink on 18-3-23.
//

#ifndef SDK_CLION_PACKING_H
#define SDK_CLION_PACKING_H

#include "type_def.h"
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string>

//基础背包：背包数量为1个，背包算法只执行一次
//完全背包：背包数量无限制，背包算法循环执行直到分配完毕

class packing {
    Server server;              //服务器参数
    std::map<int, int> vm_num;  //预测得出的vm数量
    std::map<int,Vm> vm_info;   //vm的容量参数

    int vm_typenum;
    double score[2];
    double paramA[2];
    //获取权重系数
    void set_paramA();
    //基础背包方法，type为不同权重系数
    std::map<int,int> onepack_base(Server server, std::map<int, int> vm_num, int type);
    //完全背包方法，type为不同权重系数
    std::vector<std::map<int,int>> pack_base(Server, std::map<int, int> vm_num, int type);
    //完全背包方法，type为paramA_type
    std::vector<std::map<int,int>> pack_base(Server, std::map<int, int> vm_num);
    //一次背包函数
    void MultiplePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos);
    void ZeroOnePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos);
    void CompletePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int pos);

    std::vector<int> get_path(std::vector<std::vector<std::vector<int> > > &used, std::map<int,Vm> vminfo, int U, int V);
    Allocat_server allocate_oneserver(int id, int core, int mem);
    bool check_vmnum_empty(std::vector<int> &temp);
public:
    //构造函数
    packing(Server server, std::map<int, int> vm_num, std::map<int,Vm> vm_info);
    packing(Server server, std::map<int, int> vm_num);
    packing(Server server, std::map<int, int> vm_num, std::map<int,Vm> vm_info, int vmtypenum);

    //一次基础背包方法，使用默认参数
    std::map<int,int> onepack_item();
    //一次基础背包方法，自定义参数,包括server信息和vm数量
    std::map<int,int> onepack_item(Server server, std::map<int, int> vm_num);


    //一次背包方法，使用默认参数,type = 2
    std::vector<std::map<int,int>> pack_item();
    //一次背包方法，自定义参数,包括server信息和vm数量,type分为1和2,1表示比例和，2表示直和
    std::vector<std::map<int,int>> pack_item(Server server, std::map<int, int> vm_num, int type);
    //一次背包方法，执行两次，paramA系数不同，返回得分高的结果
    std::vector<std::map<int,int>> pack_item_compareA(Server server, std::map<int, int> vm_num);


    //获取背包方法的得分
    double get_score(std::map<int, int>vmnum, Server sv, int sv_num);



};


#endif //SDK_CLION_PACKING_H