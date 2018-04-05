//
// Created by ruiy on 18-3-16.
//
#include <cstring>
#include "test.h"
#include "AR.h"
#include "type_def.h"
#include "Random.h"
#include "CxLibSVM.h"

void test_ar(){
    //北京1987-2014人口: 35
    std::vector<double> test_data = {871.5,897.1,904.3,919.2,935.0,950.0,965.0,981.0,1028.0,1047.0,1061.0,1075.0,
                                     1086.0,1094.0,1102.0,1112.0,1125.0,1251.1,1259.4,1240.0,1245.6,1257.2,1363.6,
                                     1385.1,1423.2,1456.4,1492.7,1538.0,1601.0,1676.0,1771.0,1860.0,1961.9,2018.6,2069.3
    };
    // std::vector<double> test_data = {2,3,4,3,7};

    AR ar_model(test_data);
    ar_model.fit("none_and_least_square");
    std::vector<double> res = ar_model.predict(2);

    ar_model.print_model_info();
}

/**
 * 随机数测试
 */
void test_random() {
    for (int i=0;i<10;i++) {
        int t = Random::random_int(1, 10);
        printf("%d ", t);
    }
}

/**
 *
 * @param param
 */
void init_svm_param(struct svm_parameter& param)
{
    //参数初始化，参数调整部分在这里修改即可
    // 默认参数
    param.svm_type = C_SVC;        //算法类型
    param.kernel_type = LINEAR;    //核函数类型
    param.degree = 3;    //多项式核函数的参数degree
    param.coef0 = 0;    //多项式核函数的参数coef0
    param.gamma = 0.5;    //1/num_features，rbf核函数参数
    param.nu = 0.5;        //nu-svc的参数
    param.C = 10;        //正则项的惩罚系数
    param.eps = 1e-3;    //收敛精度
    param.cache_size = 100;    //求解的内存缓冲 100MB
    param.p = 0.1;
    param.shrinking = 1;
    param.probability = 1;    //1表示训练时生成概率模型，0表示训练时不生成概率模型，用于预测样本的所属类别的概率
    param.nr_weight = 0;    //类别权重
    param.weight = NULL;    //样本权重
    param.weight_label = NULL;    //类别权重
}

/**
 *
 * @param x
 * @param y
 * @param sample_num
 * @param dim
 * @param scale
 */
void gen_sample(std::vector<std::vector<double>> &x, std::vector<double>& y, long sample_num, long dim, double scale)
{
    //long sample_num = 200;        //样本数
    //long dim = 10;    //样本特征维度
    //double scale = 1;    //数据缩放尺度

    srand((unsigned)time(NULL));//随机数
    //生成随机的正类样本
    for (int i = 0; i < sample_num; i++)
    {
        std::vector<double> rx;
        for (int j = 0; j < dim; j++)
        {
            rx.push_back(scale*(rand() % 10));
        }
        x.push_back(rx);
        y.push_back(1);
    }

    //生成随机的负类样本
    for (int i = 0; i < sample_num; i++)
    {
        std::vector<double> rx;
        for (int j = 0; j < dim; j++)
        {
            rx.push_back(-scale*(rand() % 10));
        }
        x.push_back(rx);
        y.push_back(2);
    }
}


svm_problem prob;
svm_parameter param;
/*train_x,train_y是我已经导入的数据，分别是样本及其对应的类别标签*/
void init_svm_problem(std::vector<std::vector<double>> train_x, std::vector<double> train_y)
{
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
}

void init_svm_parameter()
{
    param.svm_type = C_SVC;   // 即普通的二类分类
    param.kernel_type = RBF;  // 径向基核函数
    param.degree = 3;
    param.gamma = 0.01;
    param.coef0 = 0;
    param.nu = 0.5;
    param.cache_size = 1000;
    param.C = 0.09;
    param.eps = 1e-5;
    param.p = 0.1;
    param.shrinking = 1;
    param.probability = 0;
    param.weight_label = NULL;
    param.weight = NULL;
}

svm_node* init_test_data(std::vector<double> test_x){
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
 * 测试svm功能是否正常
 */
void test_svm(){

    /*1、准备训练数据*/
    std::vector<std::vector<double>> train_x; // 训练集
    std::vector<double> train_y; // 训练类别集
    gen_sample(train_x, train_y, 200, 10, 1);

    std::vector<std::vector<double>> test_x;    // 测试集
    std::vector<double> test_y; // 测试类别集
    gen_sample(test_x, test_y, 200, 10, 1);

    init_svm_problem(train_x, train_y);     // 打包训练样本
    init_svm_parameter();   // 初始化训练参数

    svm_model* model = svm_train(&prob, &param);
    svm_save_model("model", model);     // 保存训练好的模型，下次使用时就可直接导入
    int acc_num = 0;        // 分类正确数
//    svm_model* model = svm_load_model("model");
    for (int i = 0; i < test_x.size(); i++)
    {
        svm_node* node = init_test_data(test_x[i]);
        double pred = svm_predict(model, node);
        if (pred == test_y[i])
            acc_num++;
    }

    printf("accuracy: %f percent", acc_num*100.0 / test_x.size());
    printf("classification: %d / %d", acc_num, test_x.size());
}