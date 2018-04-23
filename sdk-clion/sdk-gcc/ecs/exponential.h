//
// Created by caocongcong on 18-4-23.
//

#ifndef SDK_CLION_EXPONENTIAL_H
#define SDK_CLION_EXPONENTIAL_H

#include <vector>
#include <string>

//预测函数
std::vector<double> exponentialPredict(std::vector<double >, int predictDate, double alpha);
//进行一次平滑
std::vector<double> smooth(std::vector<double> data, double alpha);
//使用一阶平滑进行预测
std::vector<double> predictBySmooth1(std::vector<double> data, std::vector<double> Order1, int predictData, double alpha);
//使用二阶平滑进行预测
std::vector<double> predictBySmooth2(std::vector<double>Order1, std::vector<double> Order2, int predictData, double alpha);
#endif //SDK_CLION_EXPONENTIAL_H
