//
// Created by ruiy on 18-3-30.
//

#include "Bin.h"

Bin::Bin(int type, double cores, double mems): type(type), cores(cores), mems(mems) {}

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

/**
 * 不管能否放入, 强行塞入
 */
void Bin::put_force(const Vm &object) {
    objects.push_back(object);
    cores -= object.core;
    mems -= object.mem;
}

/**
 * 去除该箱子中第index个物体
 * @param index
 */
void Bin::remove(int index) {
    cores += objects[index].core;
    mems += objects[index].mem;
    objects.erase(objects.begin() + index);

}

/**
 * 判断Bin是否可行
 */
bool Bin::is_feasible() {
    // 如果剩余资源均大于等于0, 则可行; 否则不可行
    return cores >=0 && mems >= 0;
}