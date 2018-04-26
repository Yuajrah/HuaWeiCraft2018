//
// Created by ruiy on 18-4-4.
//

#ifndef SDK_CLION_NOISE_H
#define SDK_CLION_NOISE_H

#include <map>
#include <vector>

std::map<int, std::vector<double>> remove_noise_1th(std::map<int, std::vector<double>> data);

std::vector<double> remove_noise_by_avg(std::vector<double> data, int day);

std::vector<double> remove_noise_by_box(std::vector<double> data);

#endif //SDK_CLION_NOISE_H
