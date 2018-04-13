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
            result[t.first] = (int) ecs_sum;
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
        result[t.first] = (int)(predict_ecs_data[need_predict_day-1]*need_predict_day);
    }

    return result;
}

std::map<int, int> predict_by_randomForest (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{
    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        //printf("训练第%d种服务器：\n",t.first);
        bool mv_flag = true;
        int split_windows = get_split_window(ecs_data);
//        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
//        std::vector<std::vector<double>> train = get_vector_train(train_data_need);
//        std::vector<double> target = get_vector_target(train_data_need);
        std::vector<std::vector<double>> train = timeseries_to_supervised_data(ecs_data, split_windows, mv_flag);
        std::vector<double> target = timeseries_to_supervised_target(ecs_data, split_windows, mv_flag);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);
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
        result[t.first] = (int)(predict_ecs_data[need_predict_day-1]*need_predict_day);
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
std::map<int, int> predict_by_LR(std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day)
{

    std::map<int,int>result;
    for (auto &t: vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
//        printf("训练第%d种服务器：\n",t.first);
        bool mv_flag = true;
        int split_windows = get_split_window(ecs_data);
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train = timeseries_to_supervised_data(ecs_data, split_windows, mv_flag);
        std::vector<double> target = timeseries_to_supervised_target(ecs_data, split_windows, mv_flag);
        LR liner(train, target);
        //直接最小二乘训练
        liner.train();
        //岭回归训练
//        liner.RidgeTrain(0.3);
        std::vector <double> predict_ecs_data;
        for(int i=0; i < need_predict_day; i++)
        {
            double tmp_predict = liner.predict(frist_predict_data);
            if (tmp_predict < 0.0)
            {
                tmp_predict = 0.0;
            }
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(tmp_predict);
            predict_ecs_data.push_back(tmp_predict);
        }
        result[t.first] =std::max(round(std::accumulate(predict_ecs_data.begin(), predict_ecs_data.end(), 0.0)), 0.0);
//        result[t.first] = (int)(predict_ecs_data[need_predict_day-1]*need_predict_day);
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
//std::map<int, int> predict_by_svm (std::map<int, std::vector<double>> train_data){
//    CxLibSVM svm;
//    svm_parameter param; // 使用了默认参数
//    int fold = 10;
//};

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
//    param.C = 0.13;
    param.C = 0.5;
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
//        printf("训练第%d种服务器：\n",t.first);
        /* 1. 准备训练集合*/
        bool mv_flag = true;
        int split_windows = get_split_window(ecs_data);
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train_x = get_vector_train(train_data_need);
        std::vector<double> train_y = get_vector_target(train_data_need);

        /* 2. 初始化问题*/
        svm_problem prob = init_svm_problem(train_x, train_y);     // 打包训练样本
        svm_parameter param = init_svm_parameter();   // 初始化训练参数


        param.svm_type = NU_SVR;
        param.kernel_type = LINEAR;
        param.degree = 3;
        param.gamma = 0.01;	// 1/num_features
        param.coef0 = 0;
        param.nu = 0.5;
        param.cache_size = 100;
        param.C = 1;
        param.eps = 1e-3;
        param.p = 0.1;
        param.shrinking = 1;
        param.probability = 0;
        param.nr_weight = 0;
        param.weight_label = NULL;
        param.weight = NULL;


        /* 3. 训练模型 */
        svm_model* model = svm_train(&prob, &param);

        /* 4. 获取所需要的特征 */
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);

        /* 5. 开始预测 */
        std::vector<double> predict_ecs_data;
        for(int i=0; i < BasicInfo::need_predict_cnt; i++)
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

}
std::map<int, int> predict_by_svm (std::map<int, std::vector<double>> train_data, int step){

    std::map<int,int> result;

    for (auto &t: BasicInfo::vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
//        printf("训练第%d种服务器：\n",t.first);
        /* 1. 准备训练集合*/
        bool mv_flag = true;
        int split_windows = get_split_window(ecs_data);
        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(ecs_data, split_windows, mv_flag);
        std::vector<std::vector<double>> train_x = get_vector_train(train_data_need);
        std::vector<double> train_y = get_vector_target(train_data_need);

        /* 2. 初始化问题*/
        svm_problem prob = init_svm_problem(train_x, train_y);     // 打包训练样本
        svm_parameter param = init_svm_parameter();   // 初始化训练参数
        param.C =step *0.1;
        /* 3. 训练模型 */
        svm_model* model = svm_train(&prob, &param);

        /* 4. 获取所需要的特征 */
        std::vector<double> frist_predict_data = get_frist_predict_data(ecs_data, split_windows, mv_flag);

        /* 5. 开始预测 */
        std::vector<double> predict_ecs_data;
        for(int i=0; i < BasicInfo::need_predict_cnt; i++)
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

}

