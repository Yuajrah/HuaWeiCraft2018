//
// Created by caocongcong on 18-4-14.
//

#include "FFOD.h"

std::vector<Bin> alloc_by_ffod (std::vector<Vm> objects)
{
    std::vector<Bin> result;
    int serverType = BasicInfo::server_infos.size();
    double usedRate = 1.1;
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
            double cpu_rate = (allocateVm[0].core+0.0)/BasicInfo::server_infos[k].core;
            //计算MeM的得分
            double mem_rate = (allocateVm[0].mem+0.0)/BasicInfo::server_infos[k].mem;
            //如果放置不下
            if(cpu_rate>1 || mem_rate >1)
            {
                continue;
            }
            double resouce_rate = std::max(cpu_rate, mem_rate);
            //为第一个选择一个箱子
            if (resouce_rate < rate)
            {
                fristBinType = k;
            }
        }
        //开辟第一个箱子
        Bin tmp_bin(fristBinType,BasicInfo::server_infos[fristBinType].core, BasicInfo::server_infos[fristBinType].mem);
        tmp_bin.put(allocateVm[0]);
        tmpBins.push_back(tmp_bin);
        //从第二个箱子开始放置
        for (int l = 1; l < allocateVm.size(); ++l) {
            //最后新建箱子种类
            int chooseTpye;
            //如果选择已有的箱子的下标
            int binIndex;
            double minRate = 1.1;
            //是否需要开辟新的箱子
            bool needNewBin = true;
            //对已有的箱子进行扫描
            for (int j = 0; j < tmpBins.size(); ++j) {
                //如果当前的箱子的
                if(tmpBins[j].cores >= allocateVm[l].core && tmpBins[j].mems >= allocateVm[l].mem)
                {
                    needNewBin = false;
                    double coreUsedRate = 1 - (tmpBins[j].cores+0.0) /BasicInfo::server_infos[tmpBins[j].type].core;
                    double memUsedRate = 1 - (tmpBins[j].mems+0.0) / BasicInfo::server_infos[tmpBins[j].type].mem;
                    double coreVmRate = (allocateVm[l].core+0.0) / BasicInfo::server_infos[tmpBins[j].type].core;
                    double memVmRate = (allocateVm[l].mem+0.0) / BasicInfo::server_infos[tmpBins[j].type].mem;
                    if(coreUsedRate > memUsedRate)
                    {
                        memVmRate -= coreUsedRate-memUsedRate;
                    }
                    else
                    {
                        coreVmRate -= memUsedRate - coreUsedRate;
                    }
                    double delta = std::max(memVmRate, coreVmRate);
                    if(delta < minRate)
                    {
                        minRate = delta;
                        binIndex = j;
                    }
                }
            }
            //判断是否要开辟新的箱子
            if(needNewBin) {
                for (int k = 0; k < serverType; ++k) {
                    //获取CPU的得分
                    double cpu_rate = (allocateVm[0].core + 0.0) / BasicInfo::server_infos[k].core;
                    //计算MeM的得分
                    double mem_rate = (allocateVm[0].mem + 0.0) / BasicInfo::server_infos[k].mem;
                    //如果放置不下
                    if (cpu_rate > 1 || mem_rate > 1) {
                        continue;
                    }
                    double resouce_rate = std::min(cpu_rate, mem_rate);
                    //为第一个选择一个箱子
                    if (resouce_rate < minRate) {
                        chooseTpye = k;
                        //needNewBin = true;
                    }
                }
            }
            if(needNewBin)
            {
                Bin tmp(chooseTpye, BasicInfo::server_infos[chooseTpye].core, BasicInfo::server_infos[chooseTpye].mem);
                tmp.put(allocateVm[l]);
                tmpBins.push_back(tmp);
            }
            else
            {
                tmpBins[binIndex].put(allocateVm[l]);
            }
        }
        //showAllocate(tmpBins);
        double tmpUsedRate = getUsedRate(tmpBins);
        if(tmpUsedRate > usedRate)
        {
            usedRate = tmpUsedRate;
            result = tmpBins;
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
    return result;
}

double getUsedRate(std::vector<Bin> bins)
{
    double coreTatol = 0;
    double memTatol = 0;
    double coreLeft = 0;
    double menLeft = 0;
    for (int i = 0; i < bins.size(); ++i) {
        coreTatol += BasicInfo::server_infos[bins[i].type].core;
        memTatol += BasicInfo::server_infos[bins[i].type].mem;
        coreLeft += bins[i].cores;
        menLeft += bins[i].mems;
    }
    double rate = 1.0- ((coreLeft/coreTatol)+(menLeft/memTatol))/2;
    printf("此次的资源利用率：%f\n", rate);
    return rate;
}
bool cmp(resouceTmp x,resouceTmp y)
{
    if(x.rate > y.rate)
    {
        return true;
    }
    else if  (x.rate == y.rate)
    {
        return x.index < y.index;
    }
    return false;
}
void showAllocate(std::vector<Bin> bins)
{
    for (int i = 0; i < bins.size() ; ++i) {
        printf("第%d个箱子,箱子种类%d\n", i, bins[i].type);
        for (int j = 0; j < bins[i].objects.size(); ++j) {
            printf(" %d", bins[i].objects[j].type);
        }
        printf("\n");
    }
}