//
// Created by ruiy on 18-4-19.
//

#ifndef SDK_CLION_FF_UTILS_H
#define SDK_CLION_FF_UTILS_H


#include <vector>
#include <algorithm>
#include "Bin.h"
#include "type_def.h"
#include "BasicInfo.h"

std::vector<Bin> ff(std::vector<Bin> bins, std::vector<Vm> objects);

std::vector<double> calc_alpha(const std::vector<Bin> &bins);
std::vector<double> calc_beta(const std::vector<Vm> &objects);
std::vector<double> calc_alpha_div_beta(const std::vector<Bin> &bins, const std::vector<Vm> &objects);
void calc_object_size(std::vector<Vm> &objects, const std::vector<double> &params, bool is_priority=false);
void calc_bin_size(std::vector<Bin> &bins, const std::vector<double> &params, bool is_priority=false);
bool put_bins(const Vm &object, std::vector<Bin> &bins);

int get_min_size_bin(const std::vector<Bin> &bins);
int get_max_size_bin(const std::vector<Bin> &bins);

bool mrp_ff(std::vector<Bin> &bins, std::vector<Vm> objects);
bool mrp_ffd(std::vector<Bin> &bins, std::vector<Vm> objects);
bool mrp_bf(std::vector<Bin> &bins, std::vector<Vm> objects);
bool mrp_bfd_item_centric(std::vector<Bin> &bins, std::vector<Vm> objects);
bool mrp_bfd_bin_centric(std::vector<Bin> &bins, std::vector<Vm> objects);

bool is_feasible(std::vector<Bin> &bins);
std::vector<Bin> clear_empty_bin(std::vector<Bin> bins);

#endif //SDK_CLION_FF_UTILS_H
