//
// Created by caocongcong on 18-3-31.
//
#include "data_preprocess.h"

double get_mean(std::vector<double> train_data,  double rate)
{
    double result = -1;
    int count = 0;
    double sum = 0.0;

    //统计不为0的个数以及总和
    for (auto data : train_data)
    {
        if (data > 0.1)
        {
            count++;
            sum += data;
        }
    }
    int num = train_data.size();
    double cul_rate = count/(num+0.0);
//    printf ("current rate: %f\n", cul_rate);
    if (cul_rate<=rate) {
        result = sum / num;
    }
    return result;
}

std::map<int, int> change_by_mean_vaule(std::map<int, std::vector<double>> trian_data, std::map<int, Vm> vm_info, double rate, int need_predict_day,
                            std::map<int, int> predict_data)
{
    for (auto &t: vm_info) {
        double tmp_result = get_mean(trian_data[t.first], rate);
        if (tmp_result >0)
        {
//            printf("flavor number %d\n", t.first);
            predict_data [t.first] =  tmp_result*need_predict_day;
        }
    }
    return predict_data;
}

