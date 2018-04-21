//
// Created by caocongcong on 18-4-13.
//

#ifndef SDK_CLION_LINERREGRESSION_H
#define SDK_CLION_LINERREGRESSION_H
#include <vector>
#include "math_utils.h"
class LR
{
public:
    LR(std::vector<std::vector<double>> train, std::vector<double> target);
    //正常线性回归
    void train();
    //岭回归
    void RidgeTrain(double alpha);
    double predict(std::vector<double> predict_data);
    //对一个矩阵对象对角线加   alpha
    // X+aI
    std::vector<std::vector<double>> addMul(std::vector<std::vector<double>>, double alpha);


private:
    std::vector<std::vector<double>> train_data;
    std::vector<std::vector<double>> target_data;
    std::vector<std::vector<double>> weight;

};
#endif //SDK_CLION_LINERREGRESSION_H
