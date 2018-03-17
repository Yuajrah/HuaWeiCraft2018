//
// Created by ruiy on 18-3-16.
//

#ifndef SDK_GCC_AR_H
#define SDK_GCC_AR_H

#include <vector>
#include <string>
#include "type_def.h"
class AR {
private:

    std::vector<Double> get_auto_cov();
    std::vector<Double> get_auto_cor(std::vector<Double> auto_cov);

    std::vector<Double> ic_vals;
    std::vector<Double> auto_cov;
    std::vector<Double> auto_cor;
    std::vector<Double> bias_cor;
    std::vector<Double> data;
    std::vector<Double> noise_var; // 白噪声方差，从滞后0开始，一直到滞后len-1，len为数据长度
    int best_p; // 滞后阶数
    std::vector<Double> a; // 存放滞后p阶时的自回归系数
    std::vector<Double> res; // 存放滞后p阶时的自回归系数
    int sum; // 预测k天的总和
public:


    AR(std::vector<Double>);
    void fit(std::string ic="none", int p=-1, int p_max=-1);
    std::vector<Double> predict(int k);
    std::vector<Double> predict_sum(int k); // 预测接下来几天的该vm的和
    void print_model_info();

    Double get_bias();
    std::vector<Double> least_squares();
    std::pair<std::vector<std::vector<Double>>, std::vector<std::vector<Double>>> format_data();

    inline int get_sum () {return sum;}
};


#endif //SDK_GCC_AR_H
