//
// Created by caocongcong on 18-3-31.
//

#include "ml_predict.h"

std::map<int, int> predict_by_knn (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        bool mv_flag = true;
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);
//        result[t.first] = 1;
        double ecs_sum = 0.0;
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = knn_regresion_brust(train_data_need,frist_predict_data, 5);
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);
            predict_ecs_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
        if (mv_flag)
        {
            result[t.first] = (int)(predict_ecs_data[need_predict_day-1]*need_predict_day);
        }
        else {
            result[t.first] = (int) ecs_sum;
        }
    }

    return result;
}

std::map<int, int> predict_by_randomForest (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        bool mv_flag = true;
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);

        RandomForest randomForest(50,10,2,0);

        float** train_set = get_float_train(train_data_need,split_windows);
        float* train_label = get_float_test(train_data_need);

        randomForest.train(train_set,train_label,train_data_need.size(),split_windows,10,true,3);
        float* frist_need_predict = get_frist_preditc(ecs_data, split_windows, mv_flag);
        float tmp_result = 0.0;
        double sum = 0.0;
        for (int i=0; i<split_windows; i++)
        {
            randomForest.predict(frist_need_predict, tmp_result);
            sum += tmp_result;
            frist_need_predict = add_one_data(frist_need_predict,tmp_result,split_windows);

        }
//        if (mv_flag)
//        {
//            result[t.first] = floor(tmp_result*split_windows);
//        }
//        else
//        {
        result[t.first] = floor(sum);
//        }
        delete[](train_label);
        delete[](frist_need_predict);
        for (int j = 0; j < train_data_need.size(); ++j) {
            delete[](train_set[j]);
        }
    }

    return result;
}