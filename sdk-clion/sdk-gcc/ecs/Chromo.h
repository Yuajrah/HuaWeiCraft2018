//
// Created by ruiy on 18-3-30.
//

#ifndef SDK_CLION_CHROMO_H
#define SDK_CLION_CHROMO_H

#include "Bin.h"
#include <vector>

/**
 * 染色体类
 */
class Chromo {
    std::vector<Bin> genes; // 染色体由基因组成, 每个基因代表一个箱子(类)
    double fitness; // 存放适应度
    double p; // 轮盘赌用到的概率
public:
    Chromo() = default;
    Chromo(std::vector<Bin> genes);

    /**
     * 计算适应函数
     * 公式: sum()
     */
    void calc_fitness();

    /**
     * 两条染色体的具体的交叉操作
     */
    void operator*(Chromo &b);

    /**
     * 交叉算子中用到的插入
     * 1. 先统计要本体中重复包含物体的箱子, 并统计其中未重复的物体
     * 2. 去除重复包含物体的箱子
     * 3. 将1中统计的物体按照ffd重新插入
     * @param index 插入位置
     * @param genes 插入的基因片段
     */
    void insert(int index, std::vector<Bin> genes);


    inline double get_fitness() {return fitness;}
    inline double get_p() {return p;}
    inline void set_p(double p) {this->p  = p;}
};


#endif //SDK_CLION_CHROMO_H
