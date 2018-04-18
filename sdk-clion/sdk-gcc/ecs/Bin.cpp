//
// Created by ruiy on 18-3-30.
//

#include "Bin.h"

Bin::Bin(double cores, double mems): cores(cores), mems(mems) {}

/**
 * 如果可以放入, 则放入并返回true; 否则返回false表示不可放入
 * 若放入, 只更新剩余核心数cores和剩余内存mems
 * @param object
 * @return
 */
bool Bin::put(Vm object) {
    if (object.core <= cores && object.mem <= mems) {
        objects.push_back(object);
        cores -= object.core;
        mems -= object.mem;
        return true;
    }
    return false;
}