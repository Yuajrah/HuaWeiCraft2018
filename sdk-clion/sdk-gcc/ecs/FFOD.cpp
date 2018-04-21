//
// Created by caocongcong on 18-4-14.
//

#include "FFOD.h"

std::vector<Bin> alloc_by_ffod (std::vector<Vm> objects)
{
    std::vector<Bin> result;
    int serverType = BasicInfo::server_infos.size();
    for (int i = 0; i < serverType; ++i) {
        printf("当前开始服务器种类:%d\n", BasicInfo::server_infos[i].type);
        //保存当前放置结果
        std::vector<Bin> tmpBins;
        std::vector<int> reIndex = find_index(objects, BasicInfo::server_infos[i]);
        //按照对当前服务器重新构造一个需要处理服务器的序列
        std::vector<Vm> allocateVm;
        for (int j = 0; j < reIndex.size() ; ++j) {
            allocateVm.push_back(objects[reIndex[j]]);
        }
        //对于第一个为其选择一个合适的服务器
        int fristBinType;
        double rate = 1.1;
        for (int k = 0; k < serverType; ++k) {
            //获取CPU的得分
            double cpu_rate = allocateVm[0].core/BasicInfo::server_infos[k].core;
            //计算MeM的得分
            double mem_rate = allocateVm[0].mem/BasicInfo::server_infos[k].mem;
            double resouce_rate = std::min(cpu_rate, mem_rate);
            if (true)
            {}
        }
    }
    return result;
}

//使用服务器k对所有的虚拟机进行分配
//返回排序后的服务器顺序
std::vector<int> find_index(std::vector<Vm> objects, Server server_K)
{
    //首先获取目标服务器的核和内存
    double Core = server_K.core;
    double Mem = server_K.mem;
    std::vector<resouceTmp> TmpR;
    for (int i = 0; i < objects.size(); ++i) {
        double core_rate = objects[i].core/Core;
        double mem_rate = objects[i].mem/Mem;
        double rate = std::fabs(core_rate-mem_rate)/(core_rate+mem_rate);
        resouceTmp resouce;
        resouce.index = i;
        resouce.rate = rate;
        TmpR.push_back(resouce);
    }
    sort(TmpR.begin(), TmpR.end(), cmp);
    std::vector<int> result;
    for (int j = 0; j < TmpR.size(); ++j) {
        result.push_back(TmpR[j].index);
    }
}

bool cmp(resouceTmp x,resouceTmp y)
{
    return x.rate > y.rate;
}
