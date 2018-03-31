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

    double p_mutation; // 变异概率
    double p_cross; // 交叉概率

public:
    GGA(std::vector<Vm> objects, Server server_info, int pop_size, double p_mutation, double p_cross);

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
     * 从种群中选出num个染色体(个体)
     * @param num 选择的染色体个数
     * @return
     */
    std::vector<Chromo> random_select(int num);

    /**
     * 选择算子 - 竞标赛
     * 每次从population中随机选取n个个体, 并从中挑出适应度最高的那个, 加入子代
     * 注: 在执行此函数之前, 务必要计算每个个体的适应度(比如执行calc_fitness()函数)
     *
     */
    void tournament_select();

    /**
     * 选择算子 - 轮盘赌
     * 根据适应度计算概率, 然后通过轮盘赌 选择子代
     */

    void rolette_select();

    /**
     * 交叉算子 - GGA的交叉:
     * 1. 依次两两之间交叉
     * 2. 交叉方式: 比如说有个体(染色体)1 和 个体(染色体)2
     *    (a) 个体1随机截取一段位置(即产生两个交叉点), 个体2亦如此
     *    (b) 将个体1截取的那段插入到个体2的第一个交叉点处, 此时由于个体2重复来包含插入的基因(实际上是一个基因即是一个箱子)中物体, 需要去除个体2中包含重复物体的箱子, 由于被去除的箱子中还包含不重复的物体, 因此这些物体还需要按照ffd重新插入...产生一个子代
     *    (c) 对个体2作同样的操作, 产生另一个子代
     */
    void cross();


    void mutation();
};


#endif //SDK_CLION_GGA_H
