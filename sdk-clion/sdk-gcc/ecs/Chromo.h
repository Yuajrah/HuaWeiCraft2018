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

    /**
     * 染色体变异, 随机选取mutation_num个基因, 并删除, 将缺少的物体重新插入
     * @param mutation_num, 变异的基因数目, 即要消除的箱子数量
     */
    void mutation(int mutation_num);

    /**
     * 逆序操作, 任意交换两个基因的位置
     */
    void inversion();

    inline double get_fitness() {return fitness;}
    inline double get_p() {return p;}
    inline void set_p(double p) {this->p  = p;}
    inline int get_bin_num(){return genes.size();}
    inline std::vector<Bin> get_bin() {return genes;}
};


#endif //SDK_CLION_CHROMO_H
