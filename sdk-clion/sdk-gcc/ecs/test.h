//
// Created by ruiy on 18-3-17.
//

#ifndef SDK_GCC_TEST_H
#define SDK_GCC_TEST_H

#include <vector>
#include "svm.h"

void test_ar();
void test_random();
void gen_sample(std::vector<std::vector<double>> &x, std::vector<double>& y, long sample_num = 200, long dim = 10, double scale = 1);

svm_parameter init_svm_param();
svm_problem init_svm_problem(std::vector<std::vector<double>> train_x, std::vector<double> train_y);

svm_node* init_test_data(std::vector<double> test_x);

void test_svm();

#endif //SDK_GCC_TEST_H
