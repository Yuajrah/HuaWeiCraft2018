//
// Created by ruiy on 18-3-16.
//
#include <cstring>
#include "test.h"
#include "AR.h"
#include "type_def.h"
#include "Random.h"
#include "ml_predict.h"
#include "date_utils.h"
#include "NeuralNetwork.h"
#include "gm.h"


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





void test_get_hours(){
    int hours = get_hours("2015-12-01 00:17:03", "2015-12-01 03:20:55");
    printf("\nsub hours = %d\n", hours);
};



// MyANN.cpp : 定义控制台应用程序的入口点。
//
//参考别人的算法写的BP 三层神经网络 加了很多注释 帮助自己理解 也帮助大家看 废话不多时 上代码
//详细的神经网络算法参考这个博文，写的非常好http://www.cnblogs.com/ronny/p/ann_02.html
//还有一本书 [游戏编程中的人工智能技术].pdf 该书第二部分主要讲解神经网络 也是零基础入门 非常好

//void test_ann()
//{
//    //定义一个神经网络类
//    NeuralNetwork m_ann;
//    //定义一个三层网络，其中数组中代表网络的节点
//    int ann_nodes[3]={4,20,4};
//    //创建一个三层神经网络，主要是设置网络参数
//    m_ann.create(3,ann_nodes);
//    //创建网络中的节点，主要是记录各连接线之间的关系（便于找到权重索引和输入索引）
//    m_ann.initializeNetwork();
//
//    //生成训练向量和理想输出向量 二者是一致的
//    vector<vector<double>>inputData;
//    vector<vector<double>>outData;
//    for (int i = 0; i < m_ann.iterNum; i++)
//    {
//        int index = i % 4;
//        vector<double> dvect(4, 0);
//        dvect[index] = 1;
//        outData.push_back(dvect);
//        for (int i = 0; i != dvect.size(); i++)
//        {
//            dvect[i] += (5e-3*rand() / RAND_MAX - 2.5e-3);
//        }
//        inputData.push_back(dvect);
//    }
//
//    //训练网络
//    //先前向计算输出
//    for (int i=0;i<m_ann.iterNum;i++)
//    {
//        vector<double> current_t;
//        current_t.clear();
//        m_ann.forwardCalculate(inputData.at(i),current_t);
//        m_ann.backPropagate(outData.at(i),current_t);
//
//        printf("input %.4f %.4f %.4f %.4f  \n",inputData.at(i).at(0),inputData.at(i).at(1),inputData.at(i).at(2),inputData.at(i).at(3));
//        printf("output %.4f %.4f %.4f %.4f  \n",current_t.at(0),current_t.at(1),current_t.at(2),current_t.at(3));
//    }
//
//    //测试
//    vector<double>curr_myinput;
//    curr_myinput.push_back(0.01);
//    curr_myinput.push_back(0.99);
//    curr_myinput.push_back(0.001);
//    curr_myinput.push_back(-0.05);
//    vector<double> current_output;
//    m_ann.forwardCalculate(curr_myinput,current_output);
//    printf("测试代码：----------\n");
//    printf("测试input %.4f %.4f %.4f %.4f  \n",curr_myinput.at(0),curr_myinput.at(1),curr_myinput.at(2),curr_myinput.at(3));
//    printf("测试output %.4f %.4f %.4f %.4f  \n",current_output.at(0),current_output.at(1),current_output.at(2),current_output.at(3));
//}
//
//
//void test_gm(){
//    std::vector<double> input = {2.67, 3.13, 3.25, 3.36, 3.56, 3.72};
//    std::vector<double> pre_processed = pre_precess(input);
//    printf("X0:\n");
//    for (int i=0;i<input.size();i++) {
//        printf("%f ", input[i]);
//    }
//    printf("\n\nX1:\n");
//    for (int i=0;i<pre_processed.size();i++) {
//        printf("%f ", pre_processed[i]);
//    }
//
//    std::vector<std::vector<double>> B = gen_b_mat(pre_processed);
//    printf("\n\nB:\n");
//    for (int i=0;i<B.size();i++) {
//        for (int j=0;j<B[i].size();j++) {
//            printf("%f ", B[i][j]);
//        }
//        printf("\n");
//    }
//
//    std::vector<std::vector<double>> Y = gen_y_mat(input);
//    printf("\nY:\n");
//    for (int i=0;i<Y.size();i++) {
//        printf("%f ", Y[i][0]);
//    }
//    printf("\n\n");
//
//    std::vector<double> au = estau(B, Y);
//    printf("a=%f u=%f\n",au[0], au[1]);
//    printf("\nmodel gen value x1:\n");
//    for(int i = 0; i < input.size() + 1; i++){
//        printf("%f ", predict_x1(au[0], au[1], input[0], i));
//    }
//    printf("\n");
//}