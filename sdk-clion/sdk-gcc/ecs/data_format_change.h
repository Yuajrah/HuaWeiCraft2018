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

std::vector<Bin> vector_res_to_bins_res(std::vector<std::map<int,int>> packing_result);

void after_process_add_bin(std::vector<Bin> &allocate_result, std::vector<std::pair<int, Vm>> &order_vm_info, std::map<int, int> &predict_data);

#endif //SDK_GCC_DATA_FORMAT_CHANGE_H
