//
// Created by caocongcong on 18-3-31.
//

#ifndef SDK_CLION_DATA_PREPROCESS_H
#define SDK_CLION_DATA_PREPROCESS_H
//该文件通用处理数据
#include "type_def.h"
#include <vector>
#include <map>
#include "ma.h"
//对于低频计算计算平均值
double get_mean(std::vector<double> train_data, double rate);
std::map<int, int> change_by_mean_vaule(std::map<int, std::vector<double>> train_data, std::map<int, Vm> vm_info, double rate, int need_predict_day,
                                        std::map<int, int> predict_data);
std::map<std::vector<double>, double> timeseries_to_supervised(std::vector<double>, int split_windows, bool mv = false);
std::vector<double>  get_frist_predict_data(std::vector<double>, int split_windows, bool mv = false);
//将数据转换成需要的float型
float* get_float_test(std::map<std::vector<double>, double> input);
float** get_float_train(std::map<std::vector<double>, double> input, int split_windows);
float* get_frist_preditc(std::vector<double>ecs_data, int split_windows, bool mv);
float* add_one_data(float* primary_data, float need_add, int len);

//将数据转化成需要的格式，train 为vector<vector<double>> target为vector<double>
std::vector<std::vector<double >> get_vector_train(std::map<std::vector<double>, double> input);
std::vector<double>  get_vector_target(std::map<std::vector<double>, double> input);
#endif //SDK_CLION_DATA_PREPROCESS_H
