//
// Created by ruiy on 18-3-29.
//

#ifndef SDK_CLION_AR_VARIANT_H
#define SDK_CLION_AR_VARIANT_H

#include "date_utils.h"
#include "predict.h"
#include "AR.h"
#include "math_utils.h"
#include "test.h"
#include <cstdio>
#include "get_data.h"
#include "type_def.h"
#include "ar.h"
#include "ma.h"
#include <map>
#include "frist_fit.h"
#include "packing.h"
#include "lib_io.h"
#include "data_format_change.h"
#include "ARIMAModel.h"
#include <numeric>
#include <cfloat>


std::map<int, int> predict_by_ar_1th (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

std::map<int, int> predict_by_ar_2th (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

std::map<int, int> predict_by_ar_3th (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

std::map<int, int> predict_by_ar_4th(std::map<int, Vm> vm_info, std::map<int, std::vector<double>> fit_train_data, std::map<int, int> fit_test_data, std::map<int, std::vector<double>> train_data, int need_predict_day);

std::map<int, int> predict_by_ar_5th(std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

/**
 * 直接取最后七天
 * @param train_data
 * @param need_predict_day
 * @return
 */
std::map<int, int> predict_by_ar_6th(std::map<int, std::vector<double>> train_data, int need_predict_day);
#endif //SDK_CLION_AR_VARIANT_H
