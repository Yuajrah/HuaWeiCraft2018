//
// Created by ruiy on 18-3-30.
//

#include "ff.h"
#include "BasicInfo.h"

/**
 *
 * 将若干物体(objects) 放入 配置为server_info的箱子中去
 * 其中objects的类型为vm, 每个元素代表一个vm, 格式为 [ vm1, vm4, vm2, vm1, vm1, vm3] 并无顺序要求
 *
 * @param bins 初始状态的箱子, 如果初始状态没有箱子, 则传入参数{}即可
 * @param objects 需要放入的物体
 * @param server_info, 箱子的各维度资源的约束信息
 * @return 返回装箱结果
 */
std::vector<Bin> ff(std::vector<Bin> bins, std::vector<Vm> objects) {
    for (auto &object: objects) {
        bool is_allocated = false; // 用于判断现有箱子是否可以满足分配需求
        for (auto &bin: bins) {
            is_allocated = bin.put(object);
            if (is_allocated) break;
        }
        if (is_allocated == false) { // 若现有箱子无法满足分配需求, 则新开一个箱子
            bins.push_back(Bin(BasicInfo::server_info.core, BasicInfo::server_info.mem)); // 打开一个新箱子
            bins.back().put(object);
        }
    }
    return bins;
}