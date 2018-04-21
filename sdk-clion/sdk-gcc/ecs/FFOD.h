//
// Created by caocongcong on 18-4-14.
//

#ifndef SDK_CLION_FFOD_H
#define SDK_CLION_FFOD_H

#include "type_def.h"
#include "Bin.h"
#include "BasicInfo.h"
#include "cmath"
#include "algorithm"
struct resouceTmp
{
    int index;
    double rate;
};
//排序函数
//使得按照节点的降序排序
bool cmp(resouceTmp x,resouceTmp y);
//计算当前分配结果的得分
double getUsedRate(std::vector<Bin> bins);
//显示分配结果
void showAllocate(std::vector<Bin> bins);
//按照某种服务器的资源对需要分配的对象进行排序
std::vector<int> find_index(std::vector<Vm> objects, Server server_K);
std::vector<Bin> alloc_by_ffod (std::vector<Vm> objects);
#endif //SDK_CLION_FFOD_H
