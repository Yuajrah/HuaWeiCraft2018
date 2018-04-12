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
#include"MnistPreProcess.h"
#include "CART.h"
#include "svm.h"
#include "RandomForest.h"
#include "BasicInfo.h"
#include <algorithm>
#include <math.h>

const  int split_windows = 10;


//使用KNN进行预测
std::map<int, int> predict_by_knn (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);
std::map<int, int> predict_by_knn_method2 (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

//使用随机森林进行预测
std::map<int, int> predict_by_randomForest (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);
std::map<int, int> predict_by_randomForest_method2 (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);
//使用SVM进行预测
std::map<int, int> predict_by_svm (std::map<int, std::vector<double>> train_data);

//使用树回归进行预测
std::map<int, int> predict_by_cart (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day);

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

/**
 * 初始化svm问题
 * @param train_x
 * @param train_y
 * @return
 */
svm_problem init_svm_problem(std::vector<std::vector<double>> train_x, std::vector<double> train_y);


/**
 * 把对应的一行特征转化svm_node
 * @param test_x
 * @return
 */
svm_node* feature_to_svm_node(std::vector<double> test_x);

#endif //SDK_CLION_ML_PREDICT_H
