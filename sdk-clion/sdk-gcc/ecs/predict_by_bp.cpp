//
// Created by ruiy on 18-4-22.
//

// MyANN.cpp : 定义控制台应用程序的入口点。
//
//参考别人的算法写的BP 三层神经网络 加了很多注释 帮助自己理解 也帮助大家看 废话不多时 上代码
//详细的神经网络算法参考这个博文，写的非常好http://www.cnblogs.com/ronny/p/ann_02.html
//还有一本书 [游戏编程中的人工智能技术].pdf 该书第二部分主要讲解神经网络 也是零基础入门 非常好

#include <map>
#include <numeric>
#include "BasicInfo.h"
#include "data_preprocess.h"
#include "NeuralNetwork.h"

/**
 * bp 预测
 */

std::map<int, int> predict_by_bp_1th(std::map<int, std::vector<double>> train_data)
{
    std::map<int,int> result;

    for (auto &t: BasicInfo::vm_info) {
        std::vector<double> ecs_data = train_data[t.first];
        printf("训练第%d种服务器：\n",t.first);
        /* 1. 准备训练集合*/


        //printf("训练第%d种服务器：\n",t.first);
        int mvStep = 6;
        double alpha = 0.1;
        //std::string Mode = "Ma";
//        std::string Mode = "Smooth2";
        std::string Mode = "None";
        usedData useddata = getData(ecs_data, Mode, mvStep, alpha);
        std::vector<std::vector<double>> train_x = useddata.trainData;
        std::vector<double> train_y  = useddata.targetData;


        /* 2. 初始化问题*/

        //定义一个神经网络类
        NeuralNetwork m_ann;
        //定义一个三层网络，其中数组中代表网络的节点
        int ann_nodes[3]={train_x[0].size(), 5, 1};
        //创建一个三层神经网络，主要是设置网络参数
        m_ann.create(3,ann_nodes);
        //创建网络中的节点，主要是记录各连接线之间的关系（便于找到权重索引和输入索引）
        m_ann.initializeNetwork();

        /* 3. 训练模型 */

        //训练网络
        //先前向计算输出
        for (int i=0;i<train_y.size();i++)
        {
            std::vector<double> current_t;
            current_t.clear();
            m_ann.forwardCalculate(train_x[i],current_t);
            std::vector<double> tmp(1, train_y[i]);
            m_ann.backPropagate(tmp, current_t);

            printf("input %.4f %.4f %.4f %.4f \n",train_x[i][0], train_x[i][1], train_x[i][2], train_x[i][3]);
            printf("output %.4f  \n", tmp[0]);
        }

        /* 4. 获取所需要的特征 */
        std::vector<double> frist_predict_data = useddata.fristPredictData;

        /* 5. 开始预测 */
        std::vector<double> predict_ecs_data;
        for(int i=0; i < BasicInfo::need_predict_cnt + BasicInfo::extra_need_predict_cnt; i++)
        {
            vector<double> current_output;
            m_ann.forwardCalculate(frist_predict_data, current_output);

            /* 6. 构造新的预测所需特征 */
            frist_predict_data.erase(frist_predict_data.begin());
            frist_predict_data.push_back(current_output.back());

            /* 7. 存储预测结果 */
            predict_ecs_data.push_back(current_output.back());
        }

        double ecs_sum = std::accumulate(predict_ecs_data.begin() + BasicInfo::extra_need_predict_cnt, predict_ecs_data.end(), 0.0);
        result[t.first] = round(std::max(0.0, ecs_sum));
//        result[t.first] = (int)(predict_ecs_data[BasicInfo::need_predict_day-1]*BasicInfo::need_predict_day);
    }

    return result;
}
