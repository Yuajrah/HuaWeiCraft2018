//
// Created by ruiy on 18-4-22.
//

#include "predict_by_svm.h"
#include "BasicInfo.h"
#include "data_preprocess.h"
#include "SVR.h"
#include <algorithm>

/**
 * 使用svm进行预测, NU_SVR, 线性核.
 * @param train_data
 * @return
 */

std::map<int, int> predict_by_svm_1th (std::map<int, std::vector<double>> train_data){

    std::map<int,int> result;

    for (auto &t: BasicInfo::vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        printf("训练第%d种服务器：\n",t.first);
        /* 1. 准备训练集合*/
        bool mv_flag = false;

        int split_windows = 10;
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train_x = get_vector_train(train_data_need);
        std::vector<double> train_y = get_vector_target(train_data_need);

        /* 2. 初始化问题*/
        SvmParam param;

        param.nu = 0.5;
        param.C = 0.3;
        param.eps = 1e-3;


        /* 3. 训练模型 */
        SVR svr(train_x, train_y, param);
        svr.train();

        /* 4. 获取所需要的特征 */
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);

        /* 5. 开始预测 */
        std::vector<double> predict_ecs_data;
        for(int i=0; i < BasicInfo::need_predict_cnt + BasicInfo::extra_need_predict_cnt; i++)
        {
            double tmp_predict = svr.predict(frist_predict_data);

            /* 6. 构造新的预测所需特征 */
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);

            /* 7. 存储预测结果 */
            predict_ecs_data.push_back(tmp_predict);
        }

        double ecs_sum = std::accumulate(predict_ecs_data.begin() + BasicInfo::extra_need_predict_cnt, predict_ecs_data.end(), 0.0);
        result[t.first] = round(std::max(0.0, ecs_sum));
//        result[t.first] = (int)(predict_ecs_data[BasicInfo::need_predict_day-1]*BasicInfo::need_predict_day);
    }

    return result;

};

/*train_x,train_y是我已经导入的数据，分别是样本及其对应的类别标签*/
svm_problem init_svm_problem(std::vector<std::vector<double>> train_x, std::vector<double> train_y)
{

    svm_problem prob;

    int train_size = train_y.size();
    int feature_size = train_x[0].size();

    prob.l = train_size;        // 训练样本数
    prob.y = new double[train_size];
    prob.x = new svm_node*[train_size];
    svm_node* node = new svm_node[train_size*(1 + feature_size)];

    memcpy(prob.y, &train_y[0], train_y.size()*sizeof(double));

//    prob.y = vec2arr(train_y);

    // 按照格式打包
    for (int i = 0; i < train_size; i++)
    {
        for (int j = 0; j < feature_size; j++)
        {   // 看不懂指针就得复习C语言了，类比成二维数组的操作
            node[(feature_size + 1) * i + j].index = j + 1;
            node[(feature_size + 1) * i + j].value = train_x[i][j];
        }
        node[(feature_size + 1) * i + feature_size].index = -1;
        prob.x[i] = &node[(feature_size + 1) * i];
    }
    return prob;
}

svm_parameter init_svm_parameter()
{
    svm_parameter param;

//    param.svm_type = C_SVC;   // 即普通的二类分类
//    param.kernel_type = RBF;  // 径向基核函数
//    param.degree = 3;
//    param.gamma = 0.01;
//    param.coef0 = 0;
//    param.nu = 0.5;
//    param.cache_size = 1000;
//    param.C = 0.09;
//    param.eps = 1e-5;
//    param.p = 0.1;
//    param.shrinking = 1;
//    param.probability = 0;
//    param.weight_label = NULL;
//    param.weight = NULL;
//    param.nr_weight = 0;

    // default values
    param.svm_type = NU_SVR;
    param.kernel_type = LINEAR;
    param.degree = 3;
    param.gamma = 0.01;	// 1/num_features
    param.coef0 = 0;
    param.nu = 0.5;
    param.cache_size = 100;
    param.C = 0.3;
    param.eps = 1e-3;
    param.p = 0.1;
    param.shrinking = 1;
    param.probability = 0;
    param.nr_weight = 0;
    param.weight_label = NULL;
    param.weight = NULL;

    return param;
}

svm_node* feature_to_svm_node(std::vector<double> test_x){
    int feature_size = test_x.size();

    svm_node* node = new svm_node[(1 + feature_size)];

    for (int j = 0; j < feature_size; j++) {
        node[j].index = j + 1;
        node[j].value = test_x[j];
    }

    node[feature_size].index = -1;
    return node;

}

/**
 * 使用svm进行预测, 各种核
 * @param train_data
 * @return
 */
std::map<int, int> predict_by_svm_2th (std::map<int, std::vector<double>> train_data){

    std::map<int,int> result;

    for (auto &t: BasicInfo::vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        printf("训练第%d种服务器：\n",t.first);
        /* 1. 准备训练集合*/
        bool mv_flag = false;
        int split_windows = 10;
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train_x = get_vector_train(train_data_need);
        std::vector<double> train_y = get_vector_target(train_data_need);

        /* 2. 初始化问题*/
        svm_problem prob = init_svm_problem(train_x, train_y);     // 打包训练样本
        svm_parameter param = init_svm_parameter();   // 初始化训练参数

        /* 3. 训练模型 */
        svm_model* model = svm_train(&prob, &param);

        /* 4. 获取所需要的特征 */
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);

        /* 5. 开始预测 */
        std::vector<double> predict_ecs_data;
        for(int i=0; i < BasicInfo::need_predict_cnt + BasicInfo::extra_need_predict_cnt; i++)
        {
            svm_node* node = feature_to_svm_node(frist_predict_data);
            double tmp_predict = svm_predict(model, node);

            /* 6. 构造新的预测所需特征 */
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);

            /* 7. 存储预测结果 */
            predict_ecs_data.push_back(tmp_predict);
        }

        double ecs_sum = std::accumulate(predict_ecs_data.begin() + BasicInfo::extra_need_predict_cnt, predict_ecs_data.end(), 0.0) * 1.4;
        result[t.first] = round(std::max(0.0, ecs_sum));
//        result[t.first] = (int)(predict_ecs_data[BasicInfo::need_predict_day-1]*BasicInfo::need_predict_day);
    }

    return result;

};