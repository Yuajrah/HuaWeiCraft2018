//
// Created by caocongcong on 18-3-31.
//

#include <cstring>
#include "ml_predict.h"
#include "SVR.h"


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


SvmParam init_svm_parameter()
{
    SvmParam param;

    // default values

    param.nu = 0.5;
    param.C = 0.13;
    param.eps = 1e-3;


    return param;
}



/**
 * 使用svm进行预测
 * @param train_data
 * @return
 */

std::map<int, int> predict_by_svm (std::map<int, std::vector<double>> train_data){

    std::map<int,int> result;

    for (auto &t: BasicInfo::vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        printf("训练第%d种服务器：\n",t.first);
        /* 1. 准备训练集合*/
        bool mv_flag = true;

        int split_windows = 10;
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train_x = get_vector_train(train_data_need);
        std::vector<double> train_y = get_vector_target(train_data_need);

        /* 2. 初始化问题*/
        SvmParam param = init_svm_parameter();   // 初始化训练参数

        param.C = 0.13;

        /* 3. 训练模型 */
        SVR svr(train_x, train_y, param);
        svr.train();

        /* 4. 获取所需要的特征 */
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);

        /* 5. 开始预测 */
        std::vector<double> predict_ecs_data;
        for(int i=0; i < BasicInfo::need_predict_cnt; i++)
        {
            double tmp_predict = svr.predict(frist_predict_data);

            /* 6. 构造新的预测所需特征 */
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);

            /* 7. 存储预测结果 */
            predict_ecs_data.push_back(tmp_predict);
        }

        double ecs_sum = std::accumulate(predict_ecs_data.begin(), predict_ecs_data.end(), 0.0) * 1.1;
        result[t.first] = round(std::max(0.0, ecs_sum));
//        result[t.first] = (int)(predict_ecs_data[BasicInfo::need_predict_day-1]*BasicInfo::need_predict_day);
    }

    return result;

};

