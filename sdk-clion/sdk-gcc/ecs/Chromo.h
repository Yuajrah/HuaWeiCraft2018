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

    inline double get_fitness() {return fitness;}
    inline double get_p() {return p;}
    inline void set_p(double p) {this->p  = p;}
};


#endif //SDK_CLION_CHROMO_H
