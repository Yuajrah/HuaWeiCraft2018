//
// Created by ruiy on 18-4-3.
//

#ifndef SDK_CLION_SA_H
#define SDK_CLION_SA_H

#include "type_def.h"
#include "Bin.h"
#include <vector>

class SA {
    std::vector<Vm> objects;
    double t0;
    double t_min;
    double r;
    std::vector<Bin> best_solution;

public:
    SA(std::vector<Vm> objects, double t0, double t_min, double r);
    void start();
    inline std::vector<Bin> get_best_solution(){return best_solution;};
};


#endif //SDK_CLION_SA_H
