//
// Created by ruiy on 18-4-22.
//

#ifndef SDK_CLION_PREDICT_BY_SVM_H
#define SDK_CLION_PREDICT_BY_SVM_H

#include <map>
#include "type_def.h"

std::map<int, int> predict_by_svm_1th (std::map<int, std::vector<double>> train_data);
std::map<int, int> predict_by_svm_2th (std::map<int, std::vector<double>> train_data);

#endif //SDK_CLION_PREDICT_BY_SVM_H
