//
// Created by caocongcong on 18-3-31.
//

#ifndef SDK_CLION_DATA_PREPROCESS_H
#define SDK_CLION_DATA_PREPROCESS_H
//该文件通用处理数据
#include "type_def.h"
#include <vector>
#include <map>
//对于低频计算计算平均值
double get_mean(std::vector<double> train_data, double rate);
std::map<int, int> change_by_mean_vaule(std::map<int, std::vector<double>> train_data, std::map<int, Vm> vm_info, double rate, int need_predict_day,
                            std::map<int, int> predict_data);

#endif //SDK_CLION_DATA_PREPROCESS_H
