//
// Created by ruiy on 18-4-22.
//

#include "NeuralNetwork.h"
#include <math.h>
#include <cstdlib>

NeuralNetwork::NeuralNetwork(void)
{
}

NeuralNetwork::~NeuralNetwork(void)
{

}

double SIGMOID(double x)
{
    return 1 / (1 + exp(-1*x));
    //return (1.7159*tanh(0.66666667*x));
}

void NeuralNetwork::create(int num_layers,int * ar_nodes)  // 创建网络，设置网络参数：包括网络层数、每一层节点数目、权重学习率和迭代次数
{
    nLayer=num_layers; // 网络层数
    //每一层的节点数目
    for (int i=0;i<num_layers;i++)
    {
        nodes.push_back(ar_nodes[i]);
    }
    etaLearningRate=0.5;;    // 权值学习率
    iterNum=5000;    // 迭代次数


}


void NeuralNetwork::initializeNetwork()  // 初始化网络
{
    // 初始化网络，主要是创建各层和各层的结点，并给权重向量赋初值
    for (int i = 0; i != nLayer; i++)
    {
        //定义一个网络层
        NNlayer* ptrLayer = new NNlayer;
        //第一层 本节点是nodes[0] 上一层节点为0
        if (i == 0)
        {
            ptrLayer->addNeurals(nodes[i],0); //创建输入层的神经元和连线
        }
        else
        {
            //上一层关系
            ptrLayer->preLayer = m_layers[i - 1];
            ptrLayer->addNeurals(nodes[i],nodes[i-1]);
            int num_weights = nodes[i] * (nodes[i-1]+1); // 有一个是bias
            for (int k = 0; k != num_weights; k++)
            {
                // 初始化权重在0~0.05
                ptrLayer->m_weights.push_back(0.05*rand()/RAND_MAX);
            }
        }
        m_layers.push_back(ptrLayer);
    }

}

//前向网络计算，输入为单次样本和，输出样本的引用
void NeuralNetwork::forwardCalculate(vector<double>& invect, vector<double>& outvect)
{
    //首先清空输出
    actualOutput.clear();
    //迭代器获取网络的第一层 实际上就是输入层
    vector<NNlayer*>::iterator layerIt = m_layers.begin();
    while (layerIt != m_layers.end())
    {
        if (layerIt == m_layers.begin())//说明是第一层
        {
            // 第一层输出等于输入
            for (int k = 0; k != (*layerIt)->m_neurals.size(); k++)
            {
                (*layerIt)->m_neurals[k].output = invect[k];
            }
        }
        else
        {  //第二层隐藏层
            //迭代器获取隐藏层上所有节点
            vector<NNneural>::iterator neuralIt = (*layerIt)->m_neurals.begin();
            int neuralIdx = 0;
            while (neuralIt != (*layerIt)->m_neurals.end())//迭代判断是否到最后一个节点
            {
                //一个节点有多少个连接线，换句话就是单个感知器有多少个输入
                int num_connection = (*neuralIt).m_connection.size();
                //要获取偏置的索引
                //num_connection*(neuralIdx + 1) - 1
                //num_connection是单个神经元所有输入数目（包括偏置）也是上一层网络的神经元个数+偏置（1）
                //neuralIdx 该层神经网络的序号 从0开始++
                //第一个偏置是4 就是i*num_connection+num_connection-1
                double dsum = (*layerIt)->m_weights[num_connection*(neuralIdx + 1) - 1]; // 先将偏置加上
                //偏置与输入要相乘
                for (int i = 0; i != num_connection - 1; i++)
                {
                    // sum=sum of xi*wi
                    int wgtIndex = (*neuralIt).m_connection[i].weightIdx;
                    int neuIndex = (*neuralIt).m_connection[i].neuralIdx;
                    dsum += ((*layerIt)->preLayer->m_neurals[neuIndex].output*(*layerIt)->m_weights[wgtIndex]);
                }
                neuralIt->output = SIGMOID(dsum);
                neuralIdx++;
                neuralIt++;
            }
        }
        ++layerIt;//指向下一层网络
    }
    // 将最后一层的结果传递给输出
    NNlayer* lastLayer = m_layers[m_layers.size() - 1];
    vector<NNneural>::iterator neuralIt = lastLayer->m_neurals.begin();
    while (neuralIt != lastLayer->m_neurals.end())
    {
        outvect.push_back(neuralIt->output);
        ++neuralIt;
    }
}
//t 理想输出 o实际输出
void NeuralNetwork::backPropagate(vector<double>& tVect, vector<double>& oVect)
{
    // lit是最后一层的迭代器 就是输出层的迭代器
    vector<NNlayer*>::iterator lit = m_layers.end() - 1;
    // dErrWrtDxLast是最后一层所有结点的误差
    vector<double> dErrWrtDxLast((*lit)->m_neurals.size());
    // 所有层的误差
    vector<vector<double>> diffVect(nLayer);
    //先求最后一层每一个节点的误差 并放到 dErrWrtDxLast
    for (int i = 0; i != (*lit)->m_neurals.size();i++)
    {
        dErrWrtDxLast[i] = oVect[i] - tVect[i];
    }

    diffVect[nLayer - 1] = dErrWrtDxLast;
    // 先将其他层的误差都设为0
    for (int i = 0; i < nLayer - 1; i++)
    {
        diffVect[i].resize(m_layers[i]->m_neurals.size(),0.0);
    }

    int i = m_layers.size()-1;//网络层数
    for (lit; lit>m_layers.begin(); lit--)
    {
        (*lit)->backPropagate(diffVect[i],diffVect[i-1],etaLearningRate);
        //输入第i层误差 输出第i-1层误差
        --i;
    }
    diffVect.clear();
}