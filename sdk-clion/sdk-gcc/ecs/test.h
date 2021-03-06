//
// Created by ruiy on 18-3-17.
//

#ifndef SDK_GCC_TEST_H
#define SDK_GCC_TEST_H

#include <vector>
#include "type_def.h"

void test_ar();
void test_random();
void gen_sample(std::vector<std::vector<double>> &x, std::vector<double>& y, long sample_num = 200, long dim = 10, double scale = 1);


void test_get_hours();
void test_ann();

void test_gm();

#endif //SDK_GCC_TEST_H
