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

std::map<int, int> predict_by_cart (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        printf("训练第%d种服务器：\n",t.first);
        bool mv_flag = true;
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train = get_vector_train(train_data_need);
        std::vector<double> target = get_vector_target(train_data_need);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);
        decision_tree df(5,2,0.0);
        df.train(train,target);
        double ecs_sum = 0.0;
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = df.predict(frist_predict_data);
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);
            predict_ecs_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
        result[t.first] = (int)(predict_ecs_data[need_predict_day-1]*need_predict_day);
    }

    return result;
}

std::map<int, int> predict_by_randomForest (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        printf("训练第%d种服务器：\n",t.first);
        bool mv_flag = true;
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train = get_vector_train(train_data_need);
        std::vector<double> target = get_vector_target(train_data_need);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);
        //依次是树的数量，每课树的特征，树的最大深度，每个叶节点的最大样本数，最小的下降不纯度
        RandomForest rf(50,6,9,3,1.0);
        rf.train(train,target);
        double ecs_sum = 0.0;
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = rf.predict(frist_predict_data);
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);
            predict_ecs_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
        result[t.first] = (int)(predict_ecs_data[need_predict_day-1]*need_predict_day);
    }

    return result;
}

//std::map<int, int> predict_by_svm (std::map<int, std::vector<double>> train_data){
//    CxLibSVM svm;
//    svm_parameter param; // 使用了默认参数
//    int fold = 10;
//};

