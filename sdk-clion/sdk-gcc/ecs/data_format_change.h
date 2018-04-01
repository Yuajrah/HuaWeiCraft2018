//
// Created by caocongcong on 18-3-19.
//

#ifndef SDK_GCC_DATA_FORMAT_CHANGE_H
#define SDK_GCC_DATA_FORMAT_CHANGE_H

#include <map>
#include <vector>
#include <string>
#include <random>
#include "Bin.h"

std::string format_predict_res(std::map<int, int>);
//转换函数转换成需要的char*
std::string format_allocate_res(std::vector<std::map<int, int>> result_code);
std::string format_allocate_res(std::vector<Bin> bins);

std::vector<Vm> serialize(std::map<int, int> predict_data);
std::vector<Vm> random_permutation(std::vector<Vm> objects);

#endif //SDK_GCC_DATA_FORMAT_CHANGE_H
