//
// Created by root on 18-3-31.
//

#ifndef SDK_CLION_FFD_H
#define SDK_CLION_FFD_H

#include "type_def.h"
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string>


//进行分配函数
std::vector<std::map<int,int>> FFD_Dot(std::map<int, Vm> vm_info, Server server, std::map<int, int> predict_data,  char *opt_object);


//计算分配资源得分函数
void get_scores_f(std::map<int, int>, Server server, int number, int target, std::map<int, Vm>);
//根据服务器和虚拟机的关系进行分配

void merge_sort(int *data, int start, int end, int *result);

#endif //SDK_CLION_FFD_H
