//
// Created by caocongcong on 18-3-31.
//

#include <cstring>
#include "ml_predict.h"
#include "SVR.h"

std::map<int, int> predict_by_knn (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        bool mv_flag = true;
        int split_windows = get_split_window(ecs_data);
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);
//        result[t.first] = 1;
        double ecs_sum = 0.0;
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = knn_regresion_brust(train_data_need,frist_predict_data, 4);
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);
            predict_ecs_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
            //result[t.first] = predict_ecs_data[0]*need_predict_day*2;
        ecs_sum = std::accumulate(predict_ecs_data.begin() + BasicInfo::extra_need_predict_cnt, predict_ecs_data.end(), 0.0);
        result[t.first] = round(std::max(0.0, ecs_sum));
    }

    return result;
}

std::map<int, int> predict_by_knn_method2 (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        bool mv_flag = true;
        int split_windows = get_split_window(ecs_data);
        std::vector<std::vector<double>> train_serial = timeseries_to_supervised_data(ecs_data, split_windows, mv_flag);
        std::vector<double> target_serial = timeseries_to_supervised_target(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train = get_vector_train_method2(train_serial,need_predict_day);
        std::vector<double> target = get_vector_target_method2(target_serial,need_predict_day);
        std::vector<std::vector<double>> test = get_vector_test_method2(train_serial,need_predict_day);
        std::map<std::vector<double>, double> train_data_use;
        for(int i=0; i<train.size(); i++)
        {
            train_data_use[train[i]] = target[i];
        }
        //printf("获取数据成功\n");
        //依次是树的数量，每课树的特征，树的最大深度，每个叶节点的最大样本数，最小的下降不纯度
        //printf("训练成功\n");
        double ecs_sum = 0.0;
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = knn_regresion_brust(train_data_use, test[i],4);
            predict_ecs_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
        result[t.first] = (int)ecs_sum;
        //result[t.first] = (int)(predict_ecs_data[need_predict_day-1]*need_predict_day);
        //result[t.first] = get_bigger_mean(predict_ecs_data, need_predict_day/2)*need_predict_day;
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
        int split_windows = get_split_window(ecs_data);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train = timeseries_to_supervised_data(ecs_data, split_windows, mv_flag);
        std::vector<double> target = timeseries_to_supervised_target(ecs_data, split_windows, mv_flag);
        decision_tree df(6,2,0.0);
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
        ecs_sum = std::accumulate(predict_ecs_data.begin() + BasicInfo::extra_need_predict_cnt, predict_ecs_data.end(), 0.0);
        result[t.first] = round(std::max(0.0, ecs_sum));
    }

    return result;
}

std::map<int, int> predict_by_randomForest (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        int mvStep = 6;
        double alpha = 0.1;
        //std::string Mode = "Ma";
//        std::string Mode = "Smooth1";
        std::string Mode = "Smooth2";
        //std::string Mode = "None";
        usedData useddata = getData(ecs_data, Mode, mvStep, alpha);
        std::vector<std::vector<double>> train = useddata.trainData;
        std::vector<double> target = useddata.targetData;
        std::vector<double> frist_predict_data = useddata.fristPredictData;
        //依次是树的数量，每课树的特征，树的最大深度，每个叶节点的最大样本数，最小的下降不纯度
        //50,4,7,1,1.0 72.6
        RandomForest rf(50,4,7,1,1.0, sqrt(train.size()));
        //RandomForest rf(100,7,5,3,1.0, sqrt(train.size()));
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
        ecs_sum = std::accumulate(predict_ecs_data.begin() + BasicInfo::extra_need_predict_cnt, predict_ecs_data.end(), 0.0);
        result[t.first] = round(std::max(0.0, ecs_sum));
        //result[t.first] = get_bigger_mean(predict_ecs_data, need_predict_day/2)*need_predict_day;
    }

    return result;
}

std::map<int, int> predict_by_randomForest_method2 (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        //printf("训练第%d种服务器：\n",t.first);
        bool mv_flag = true;
        int split_windows = get_split_window(ecs_data);
        std::vector<std::vector<double>> train_serial = timeseries_to_supervised_data(ecs_data, split_windows, mv_flag);
        std::vector<double> target_serial = timeseries_to_supervised_target(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train = get_vector_train_method2(train_serial,need_predict_day);
        std::vector<double> target = get_vector_target_method2(target_serial,need_predict_day);
        std::vector<std::vector<double>> test = get_vector_test_method2(train_serial,need_predict_day);
        //printf("获取数据成功\n");
        //依次是树的数量，每课树的特征，树的最大深度，每个叶节点的最大样本数，最小的下降不纯度
        RandomForest rf(50,3,5,2,1.0,train.size()-train.size()/4);
        rf.train(train,target);
        //printf("训练成功\n");
        double ecs_sum = 0.0;
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = rf.predict(test[i]);
            predict_ecs_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
        result[t.first] = (int)ecs_sum;
        //result[t.first] = (int)(predict_ecs_data[need_predict_day-1]*need_predict_day);
        //result[t.first] = get_bigger_mean(predict_ecs_data, need_predict_day/2)*need_predict_day;
    }

    return result;
}


//使用线性回归进行预测

std::map<int, int> predict_by_LR (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        //printf("训练第%d种服务器：\n",t.first);
        int mvStep = 6;
        double alpha1 = 0.1;
//        std::string Mode = "Ma";
        //std::string Mode = "Smooth1";
        std::string Mode = "Smooth2";
//        std::string Mode = "Smooth3";
//        std::string Mode = "None";
        //正常获取
        usedData useddata = getData(ecs_data, Mode, mvStep, alpha1);
        std::vector<std::vector<double>> train = useddata.trainData;
        std::vector<double> target = useddata.targetData;
        std::vector<double> frist_predict_data = useddata.fristPredictData;
        LR lr(train, target);
        lr.train();
        double ecs_sum = 0.0;
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = lr.predict(frist_predict_data);
            if(tmp_predict<0.0)
            {
                tmp_predict = 0.0;
            }
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);
            predict_ecs_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
        //ecs_sum = std::accumulate(predict_ecs_data.begin(), predict_ecs_data.begin()+ BasicInfo::need_predict_day, 0.0);
        ecs_sum = std::accumulate(predict_ecs_data.begin() + BasicInfo::extra_need_predict_cnt, predict_ecs_data.end(), 0.0);
        if(ecs_sum < 0){
            ecs_sum = 0;
        }
        if (BasicInfo::extra_need_predict_day >0) {
            //smooth2的最好成绩
            result[t.first] = round(std::max(0.0, ecs_sum)*1.6);
//            result[t.first] = round(std::max(0.0, ecs_sum));
        }
        else
        {
            result[t.first] = round(std::max(0.0, ecs_sum));
        }
//        result[t.first] = result[t.first];
    }

    return result;
}

//指数平滑进行预测
std::map<int, int> predict_by_enponential (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int, int> result;
    for (auto &t: vm_info)
    {
        std::vector<double> ecs_data = train_data[t.first];
        double alpha = 0.3;
        std::vector <double> predict_ecs_data =  exponentialPredict(ecs_data, need_predict_day, alpha);
        double ecs_sum = std::accumulate(predict_ecs_data.begin() + BasicInfo::extra_need_predict_cnt, predict_ecs_data.end(), 0.0);
        result[t.first] = round(std::max(0.0, ecs_sum));
    }
    return result;
}
//线性回归增加间隔
std::map<int, int> predict_by_LR_intervel (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        //printf("训练第%d种服务器：\n",t.first);
        int mvStep = 6;
        double alpha = 0.3;
//        std::string Mode = "Ma";
//        std::string Mode = "Smooth1";
//        std::string Mode = "Smooth2";
        std::string Mode = "None";
        //正常获取
        int intervel = need_predict_day;
        usedDataIntervel useddata = getIntervelData(ecs_data, Mode, mvStep, alpha, intervel);
        std::vector<std::vector<double>> train = useddata.trainData;
        std::vector<double> target = useddata.targetData;
        std::vector<std::vector<double>> frist_predict_data = useddata.PredictData;
        LR lr(train, target);
        lr.train();
        double ecs_sum = 0.0;
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = lr.predict(frist_predict_data[i]);
            if(tmp_predict<0.0)
            {
                tmp_predict = 0.0;
            }
            predict_ecs_data.push_back(tmp_predict);
            ecs_sum += tmp_predict;
        }
        ecs_sum = std::accumulate(predict_ecs_data.begin(), predict_ecs_data.end(), 0.0);
        result[t.first] = round(std::max(0.0, ecs_sum));
    }

    return result;
}
int get_split_window(std::vector<double> data)
{
    if (unchangale)
    {
        return 10;
    }
    else
    {
        int tmp;
        tmp = int(round(12 * pow((data.size() / 100.0), 1.0/4)));
        return tmp;
    }
}


int get_bigger_mean(std::vector<double> data, int num)
{
    std::sort(data.begin(),data.end());
    double sum = 0.0;
    for(int i=0; i< num; i++)
    {
        sum+=data[i];
    }
    return (int)sum/num;
}


