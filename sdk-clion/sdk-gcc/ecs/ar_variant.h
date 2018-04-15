//
// Created by ruiy on 18-3-29.
//

#ifndef SDK_CLION_AR_VARIANT_H
#define SDK_CLION_AR_VARIANT_H

#include "date_utils.h"
#include "predict.h"
#include "AR.h"
#include "math_utils.h"
#include <cstdio>
#include "get_data.h"
#include "type_def.h"
#include "ar.h"
#include "ma.h"
#include <map>
#include "packing.h"
#include "lib_io.h"
#include "data_format_change.h"
#include <numeric>
#include <cfloat>
#include "data_preprocess.h"


std::map<int, int> predict_by_ar_1th (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

#endif //SDK_CLION_AR_VARIANT_H
