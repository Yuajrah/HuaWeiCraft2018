//
// Created by ruiy on 18-3-17.
//

#ifndef SDK_GCC_TEST_H
#define SDK_GCC_TEST_H

#include <vector>
#include "svm.h"

void test_ar();
void test_random();
void gen_train_sample(std::vector<std::vector<double>> &x, std::vector<double>& y, long sample_num = 200, long dim = 10, double scale = 1);
void gen_test_sample(std::vector<double>& x, long sample_num = 200, long dim = 10, double scale = 1);
void init_svm_param(struct svm_parameter& param);

void test_svm();

#endif //SDK_GCC_TEST_H
