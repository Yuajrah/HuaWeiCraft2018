#ifndef SDK_GCC_FRIST_FIT_H
#define SDK_GCC_FRIST_FIT_H

#include "type_def.h"
#include <iostream>
#include <vector>
#include <map>
#include <queue> 

//进行分配函数
std::vector<std::vector<int>> frist_fit(std::map<int, Vm> vm_info, Server server, std::map<int, int> predict_data,  char *opt_object);

//分配一个新的服务器
Allocat_server allocate_one(int id, int core, int storage, int target);

//合并两个
void merge_allocate (std::priority_queue<Allocat_server> &,std::priority_queue<Allocat_server> &);
//计算分配资源得分函数
void get_scores(std::map<int, int>, Server server, int number, int target, std::map<int, Vm>);
#endif //SDK_GCC_FRIST_FIT_H
