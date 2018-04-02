//
// Created by caocongcong on 18-3-31.
//

#ifndef SDK_CLION_KNN_H
#define SDK_CLION_KNN_H

#include <map>
#include <vector>
#include <queue>
double  knn_regresion_brust (std::map<std::vector<double>, double> train_data,std::vector<double> predict_data, int k =5 );
double distance(std::vector<double> data1, std::vector<double> data2);
#endif //SDK_CLION_KNN_H
