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
public:
    Chromo(std::vector<Bin> genes);
};


#endif //SDK_CLION_CHROMO_H
