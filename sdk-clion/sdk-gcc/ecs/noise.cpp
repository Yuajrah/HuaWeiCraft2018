//
// Created by ruiy on 18-4-4.
//
#include "noise.h"
#include <algorithm>

/**
 * 按照平均值去噪, 没有什么效果, 不用
 * @param data
 * @return
 */
std::map<int, std::vector<double>> remove_noise_1th(std::map<int, std::vector<double>> data){
    std::map<int, std::vector<double>> res;
    for (auto &d: data) {
        res[d.first] = remove_noise_by_avg(d.second, 3);
    }
    return res;
};

std::vector<double> remove_noise_by_avg(std::vector<double> data, int day){
    std::vector<double> avg;
    for (int i=0;i<data.size()-day+1;i++) {
        avg.push_back(std::accumulate(data.begin()+i, data.begin()+i+day, 0.0) / static_cast<double>(day));
    }
    int start = day/2;
    int threshold = 6;
    for (int i=0;i<data.size()-day+1;i++) {
        if (abs(data[start + i] - avg[i]) > threshold) {
            data[start + i] = avg[i];
        };
    }
    return data;
};