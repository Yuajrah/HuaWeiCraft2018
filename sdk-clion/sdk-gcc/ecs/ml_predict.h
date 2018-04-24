//
// Created by caocongcong on 18-3-31.
//

#ifndef SDK_CLION_ML_PREDICT_H
#define SDK_CLION_ML_PREDICT_H

#include <map>
#include <vector>
#include "type_def.h"
#include "data_preprocess.h"
#include "KNN.h"
#include "MnistPreProcess.h"
#include "CART.h"

#include "type_def.h"
#include "RandomForest.h"
#include "BasicInfo.h"
#include <algorithm>
#include <cmath>

#include "exponential.h"
#include "LinerRegression.h"
#include "svm.h"

//训练集划分的标准
const bool unchangale = false;

int get_split_window(std::vector<double> data);


//使用KNN进行预测
std::map<int, int> predict_by_knn (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);
std::map<int, int> predict_by_knn_method2 (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

//使用随机森林进行预测
std::map<int, int> predict_by_randomForest (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);
std::map<int, int> predict_by_randomForest_method2 (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);
//使用树回归进行预测
std::map<int, int> predict_by_cart (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);


std::map<int, int> predict_by_LR(std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);
//使用间隔的数据
std::map<int, int> predict_by_LR_intervel (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

std::map<int, int> predict_by_svm (std::map<int, std::vector<double>> train_data);

//使用指数回归进行拟合
std::map<int, int> predict_by_enponential (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);
//取结果前一半的平均值
int get_bigger_mean(std::vector<double> data, int num);

///////////////////////////////////////////////////
//// svm 相关函数 //////////////////////////////////
///////////////////////////////////////////////////

/**
 * 初始化参数, 默认参数
 * @return
 */
svm_parameter init_svm_parameter();


#endif //SDK_CLION_ML_PREDICT_H
