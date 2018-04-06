//
// Created by caocongcong on 18-3-31.
//

#include <cstring>
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

//std::map<int, int> predict_by_randomForest (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
//{
//    std::map<int,int>result;
//    for (auto t: vm_info)
//    {
//        int index = t.first;
//        int tmp = predict_by_randomForest_once (vm_info, train_data, need_predict_day, index);
//        if(tmp<0)
//        {
//            tmp = 0;
//        }
//        result[index] = tmp;
//
//    }
//    return result;
//}
//
//int predict_by_randomForest_once (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day, int index)
//{
//    std::vector<double> ecs_data = train_data[index];
//    bool mv_flag = true;
//    std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
//    std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);
//
//    RandomForest randomForest(50,4,2,0);
//
//    float** train_set = get_float_train(train_data_need,split_windows);
//    float* train_label = get_float_test(train_data_need);
//
//    randomForest.train(train_set,train_label,train_data_need.size(),split_windows,10,true, 4);
//    float* frist_need_predict = get_frist_preditc(ecs_data, split_windows, mv_flag);
//    float tmp_result = 0.0;
//    double sum = 0.0;
//    for (int i=0; i<split_windows; i++)
//    {
//        randomForest.predict(frist_need_predict, tmp_result);
//        sum += tmp_result;
//        frist_need_predict = add_one_data(frist_need_predict,tmp_result,split_windows);
//
//
//    }
//    delete[](train_label);
//    delete[](frist_need_predict);
//    for (int j = 0; j < train_data_need.size(); ++j) {
//        delete[](train_set[j]);
//    }
//    return (int)sum;
//
//}

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
        decision_tree df(5,2,1,0.0);
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
    param.C = 0.13;
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
        for(int i=0; i < BasicInfo::need_predict_day; i++)
        {
            svm_node* node = feature_to_svm_node(frist_predict_data);
            double tmp_predict = svm_predict(model, node);

            /* 6. 构造新的预测所需特征 */
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);

            /* 7. 存储预测结果 */
            predict_ecs_data.push_back(tmp_predict);
        }

        double ecs_sum = std::accumulate(predict_ecs_data.begin(), predict_ecs_data.end(), 0.0);
        result[t.first] = round(std::max(0.0, ecs_sum));
//        result[t.first] = (int)(predict_ecs_data[BasicInfo::need_predict_day-1]*BasicInfo::need_predict_day);
    }

    return result;

};

