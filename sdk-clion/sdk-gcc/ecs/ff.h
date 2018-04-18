//
// Created by ruiy on 18-3-30.
//

#ifndef SDK_CLION_FF_H
#define SDK_CLION_FF_H

#include "type_def.h"
#include "Bin.h"
#include <vector>
#include <algorithm>

std::vector<Bin> ff(std::vector<Bin> bins, std::vector<Vm> objects);
bool mrp_ff(std::vector<Bin> &bins, std::vector<Vm> objects);
bool mrp_ffd(std::vector<Bin> &bins, std::vector<Vm> objects);

#endif //SDK_CLION_FF_H
