//
// Created by ruiy on 18-3-30.
//

#ifndef SDK_CLION_BIN_H
#define SDK_CLION_BIN_H

/**
 * 基因(箱子)类: 一个基因代表一个箱子, 每个箱子里会装有若干物品(object)
 * Server类(结构体)有所不足(无法满足现有编程需求)，在这里另起Bin类, 作为对Server类的补充
 */

#include <vector>
#include "type_def.h"

class Bin {
public:
    std::vector<Vm> objects;
    int type;
    double cores; // [剩余]核心数目, 之所以用double, 是因为可能会对各个维度资源做一些预处理(如归一化等等)
    double mems; // [剩余]内存大小
    double size; // 多维度计算得到一维度
    Bin() = default;
    Bin(int type, double cores, double mems);

    bool put(Vm object);
};

#endif //SDK_CLION_BIN_H