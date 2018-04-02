//
// Created by caocongcong on 18-3-31.
//

#include "ml_predict.h"

std::map<int, int> predict_by_knn (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, true);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, true);
//        result[t.first] = 1;
        double ecs_sum = 0.0;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = knn_regresion_brust(train_data_need,frist_predict_data, 5);
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
        result[t.first] = (int)ecs_sum;
    }

    return result;
}