//
// Created by ruiy on 18-3-16.
//

#ifndef SDK_GCC_AR_H
#define SDK_GCC_AR_H

#include <vector>
#include <string>
class AR {
private:

    std::vector<double> get_auto_cov();
    std::vector<double> get_auto_cor(std::vector<double> auto_cov);

    std::vector<double> ic_vals;
    std::vector<double> auto_cov;
    std::vector<double> auto_cor;
    std::vector<double> bias_cor;
    std::vector<double> data;
    std::vector<double> noise_var; // 白噪声方差，从滞后0开始，一直到滞后len-1，len为数据长度
    int best_p; // 滞后阶数
    std::vector<double> a; // 存放滞后p阶时的自回归系数
    std::vector<double> res; // 存放滞后p阶时的自回归系数
public:


    AR(std::vector<double>);
    void fit(std::string ic="none", int p=-1, int p_max=-1);
    std::vector<double> predict(int k);
    void print_model_info();

    double get_bias();
    std::vector<double> least_squares();
    std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> format_data();
};


#endif //SDK_GCC_AR_H
