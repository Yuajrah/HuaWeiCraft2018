//
// Created by ruiy on 18-3-30.
//

#ifndef SDK_CLION_GGA_H
#define SDK_CLION_GGA_H

#include <vector>
#include "Chromo.h"

class GGA {
    std::vector<Vm> objects; // 要放置的物体
    Server server_info; // 箱子（物体服务器）的信息
    std::vector<Chromo> populations; // 种群
    int pop_size;
public:
    GGA(std::vector<Vm> objects, Server server_info, int pop_size);
    void initial(); // 初始化种群
    void select(); // 选择
    void cross(); // 交叉
    void mutation(); // 变异
};


#endif //SDK_CLION_GGA_H
