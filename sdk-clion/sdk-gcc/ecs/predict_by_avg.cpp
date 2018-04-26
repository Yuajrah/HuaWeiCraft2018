//
// Created by ruiy on 18-4-26.
//

#include "predict_by_avg.h"
#include "BasicInfo.h"
#include "data_preprocess.h"

#include "algorithm"

std::map<int, int> predict_by_avg_1th (std::map<int, std::vector<double>> train_data) {
    std::map<int,int> result;

    for (auto &t: BasicInfo::vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        printf("训练第%d种服务器：\n",t.first);
        /* 1. 准备训练集合*/

        int mvStep = 6;
        double alpha = 0.1;
        std::string Mode = "Ma";
//        std::string Mode = "Smooth2";
//        std::string Mode = "None";

        usedData useddata = getData(ecs_data, Mode, mvStep, alpha);
        std::vector<std::vector<double>> train_x = useddata.trainData;
        std::vector<double> train_y  = useddata.targetData;

        std::vector<double> frist_predict_data = useddata.fristPredictData;

        double ecs_sum = std::accumulate(frist_predict_data.begin() - BasicInfo::need_predict_cnt, frist_predict_data.end(), 0.0);
        if (BasicInfo::extra_need_predict_cnt > 0) {
            ecs_sum = ecs_sum * 1.2;
        } else {
            ecs_sum = ecs_sum * 0.34;
        }
        result[t.first] = round(std::max(0.0, ecs_sum));
//        result[t.first] = (int)(predict_ecs_data[BasicInfo::need_predict_day-1]*BasicInfo::need_predict_day);
    }
    return result;
};