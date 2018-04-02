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


std::vector<std::map<int,int>> packing(std::map<int,Vm> vm_info, Server server, std::map<int, int> predict_data, char *opt_object);

Allocat_server allocate_oneserver(int id, int core, int mem, int target);

void MultiplePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos);

//计算分配资源得分函数
void get_scores_p(std::map<int, int>, Server server, int number, int target, std::map<int, Vm>);

std::vector<int> get_path(std::vector<std::vector<std::vector<int> > > &used, std::map<int,Vm> vminfo, int U, int V);

bool check_vmnum_empty(std::vector<int> &temp);

#endif //SDK_CLION_PACKING_H