//
// Created by txink on 18-4-21.
//

#ifndef SDK_CLION_PACKING_NEW_H
#define SDK_CLION_PACKING_NEW_H


#include "type_def.h"
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string>


std::map<int,int> onepacking(std::map<int, Vm> vm_info, Server server, std::map<int,int> predict_data);
std::map<int,int> onepacking(std::map<int, Vm> vm_info, std::map<int, Server> servers, std::map<int,int> predict_data);
std::vector<std::map<int,int>> packing_ad(std::map<int,Vm> vm_info, std::map<int, Server> servers, std::map<int, int> predict_data);
    std::vector<std::map<int,int>> packing(std::map<int,Vm> vm_info, Server server, std::map<int, int> predict_data);
std::vector<std::map<int,int>> packing(std::map<int,Vm> vm_info, Server server, std::map<int, int> predict_data, int value_type);
Allocat_server allocate_oneserver(int id, int core, int mem);

void MultiplePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos);

//计算分配资源得分函数
double get_scores_p(std::map<int, int>predict_data, std::vector<Allocat_server> servers, std::map<int, Vm> vm_info);
double get_score_one(Server server, std::map<int,int> record,std::map<int, Vm> vm_info);
std::vector<int> get_path(std::vector<std::vector<std::vector<int> > > &used, std::map<int,Vm> vminfo, int U, int V);

bool check_vmnum_empty(std::vector<int> &temp);
bool check_vmnum_empty(std::map<int, int> predict_data);

#endif //SDK_CLION_PACKING_NEW_H
