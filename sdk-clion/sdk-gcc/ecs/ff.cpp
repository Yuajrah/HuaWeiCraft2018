//
// Created by ruiy on 18-3-30.
//

#include "ff.h"

std::vector<Bin> ff(std::vector<Vm> objects, Server server_info) {
    std::vector<Bin> bins; // 存放箱子
    for (auto &object: objects) {
        bool is_allocated = false; // 用于判断现有箱子是否可以满足分配需求
        for (auto &bin: bins) {
            if (is_allocated = bin.put(object)) break;
        }
        if (is_allocated == false) { // 若现有箱子无法满足分配需求, 则新开一个箱子
            bins.push_back(Bin(server_info.core, server_info.mem)); // 打开一个新箱子
        }
    }
    return bins;
}