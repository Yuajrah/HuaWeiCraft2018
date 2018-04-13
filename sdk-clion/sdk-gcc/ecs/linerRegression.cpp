//
// Created by caocongcong on 18-4-12.
//

#include "linerRegression.h"

LR::LR(std::vector<std::vector<double>> train, std::vector<double> target) {
    train_data = train;
    //对于每一个训练集，最后加上1
    for (int i = 0; i < train_data.size() ; ++i) {
        train_data[i].push_back(1);
    }
    for (int j = 0; j < target.size() ; ++j) {
        std::vector<double> tmp;
        tmp.push_back(target[j]);
        target_data.push_back(tmp);
    }
}

void LR::train() {
    std::vector<std::vector<double> > a, t_data,invx,tmp;
    t_data = t(train_data);
    invx = inv_lu(mulMat(t_data, train_data));
    weight = mulMat(mulMat(invx,t_data), target_data);
}

void LR::RidgeTrain(double alpha)
{
    std::vector<std::vector<double> > a, t_data,invx,tmp, tmp_mul;
    t_data = t(train_data);
    tmp_mul = mulMat(t_data, train_data);
    for (int i = 0; i < tmp_mul.size(); ++i) {
        tmp_mul[i][i] += alpha;
    }
    invx = inv_lu(tmp_mul);
    weight = mulMat(mulMat(invx,t_data), target_data);
}
double LR::predict(std::vector<double> predict_data) {
    std::vector<std::vector<double>> need_data ,need_data_T;
    predict_data.push_back(1);
    need_data.push_back(predict_data);
    //need_data_T = t(need_data);
    double result = 0.0;
//    printf("计算的矩阵大小:(%d,%d)\n",result.size(), result[0].size());
    for (int i = 0; i < weight.size(); ++i) {
        result += predict_data[i]*weight[i][0];
    }


//    printf("%f\n",result);
    return result;
}
