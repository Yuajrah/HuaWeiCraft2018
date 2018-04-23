//
// Created by ruiy on 18-4-23.
//

#ifndef SDK_CLION_GM_H
#define SDK_CLION_GM_H
#include <vector>


std::vector<double> pre_precess(std::vector<double> input);
std::vector<std::vector<double>> gen_b_mat(std::vector<double> pre_processed);
std::vector<std::vector<double>> gen_y_mat(std::vector<double> origin);
std::vector<double> estau(std::vector<std::vector<double>> mat_b, std::vector<std::vector<double>> mat_y);
std::vector<std::vector<double>> gauss(std::vector<std::vector<double>> A);
double predict_x1(double a, double u, double x0_0, int k);
#endif //SDK_CLION_GM_H

