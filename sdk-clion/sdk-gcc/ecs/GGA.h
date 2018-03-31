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

    /**
     * 随机初始化种群
     */
    void initial();

    /**
     * 根据适应度计算概率
     */
    void calc_p();

    /**
     * 计算种群中各个个体的适应度
     */
    void calc_fitness();

    /**
     * 从种群中选出num个染色体
     * @param num 选择的染色体个数
     * @return
     */
    std::vector<Chromo> random_select(int num);

    /**
     * 选择算子 - 竞标赛
     * 每次从population中随机选取n个个体, 并从中挑出适应度最高的那个, 加入子代
     *
     */
    void tournament_select();

    /**
     * 选择算子 - 轮盘赌
     * 根据适应度计算概率, 然后通过轮盘赌 选择子代
     */

    void rolette_select();

    void cross();
    void mutation();
};


#endif //SDK_CLION_GGA_H
