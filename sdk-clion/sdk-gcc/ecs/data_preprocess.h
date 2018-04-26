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
#include "BasicInfo.h"
#include <math.h>
#include "BasicInfo.h"
#include <string>

//划分数据集返回的数据
struct usedData
{
    std::vector<std::vector<double>> trainData;
    std::vector<double> targetData;
    std::vector<double> fristPredictData;
};
//划分有间隔数据集返回的数据
struct usedDataIntervel
{
    std::vector<std::vector<double>> trainData;
    std::vector<double> targetData;
    std::vector<std::vector<double>> PredictData;
};
//对于低频计算计算平均值


//该恒为false

const bool split_choosed = true;
const bool split_high_flag = false;
const int split_rate = 3;
const int move_step = 6;
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

//直接使用vector返回
std::vector<std::vector<double>> timeseries_to_supervised_data(std::vector<double> ecs_data, int split_windows, bool mv);
std::vector<double> timeseries_to_supervised_target(std::vector<double> ecs_data, int split_windows, bool mv);


//另外一种获取数的方式
std::vector<std::vector<double >> get_vector_train_method2(std::vector<std::vector<double>>input,int predict_need_date);
std::vector<double>  get_vector_target_method2(std::vector<double> input, int predict_need_date);
std::vector<std::vector<double>>  get_vector_test_method2(std::vector<std::vector<double>> input, int predict_need_date);

//利用平均值削去高峰
std::vector<double> split_high(std::vector<double>, double rate);
//获取数据
usedData getData(std::vector<double>, std::string Mode, int moveStep, double alpha1);
//获取间隔数据
usedDataIntervel getIntervelData(std::vector<double>data, std::string Mode, int moveStep, double alpha, int intervel);
//使用一结的指数平滑
std::vector<double> smoothOrderOne(std::vector<double>, double alpha, int initNum);

//使用二阶指数平滑
std::vector<double> smoothOrderTwo(std::vector<double>, double alpha1,double alpha2, int initNum);

//使用三阶指数平滑
std::vector<double>smoothOrderThree(std::vector<double>, double alpha,int initNum);

std::pair<std::vector<std::vector<double>>, std::vector<double>> format_data(std::vector<double> data, int lag, int interval, bool constant);
#endif //SDK_CLION_DATA_PREPROCESS_H
