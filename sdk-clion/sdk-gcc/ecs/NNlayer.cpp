#include "NNlayer.h"
#include <cmath>


double DSIGMOID(double x)
{

	double dd=x*(1-x);
	//double dd=(0.66666667/1.7159*(1.7159+(x))*(1.7159-(x)));
	return dd;
}
NNlayer::~NNlayer(void)
{
}

void NNlayer::addNeurals(int num, int preNumNeural)//形参为该层的节点和上一层的节点数目
{
	for (int i = 0; i != num; i++) //本层几个神经元
	{
		NNneural sneural;
		sneural.output = 0;
		for (int k = 0; k != preNumNeural+1; k++) //每个神经元多个权重输入
		{
			NNconnection sconnection;
			sconnection.weightIdx = i*(preNumNeural + 1) + k; // 设置权重索引
			sconnection.neuralIdx = k;    // 设置前层结点索引
			sneural.m_connection.push_back(sconnection);
		}
		m_neurals.push_back(sneural);
	}
}





//输入第i层误差 输出第i-1层误差
void NNlayer::backPropagate(vector<double>& dErrWrtDxn,vector<double>& dErrWrtDxnm,double eta)
{
	double output;
	//最后一层 输出层计算的误差值 （t-o）*o*(1-o)
	vector<double> dErrWrtDyn(dErrWrtDxn.size());
	for (int i = 0; i != m_neurals.size(); i++)
	{
		output = m_neurals[i].output;
		dErrWrtDyn[i] = DSIGMOID(output)*dErrWrtDxn[i];
	}
	int ii(0);
	vector<NNneural>::iterator nit = m_neurals.begin();
	vector<double> dErrWrtDwn(m_weights.size(),0);//连接线上的误差

	while(nit != m_neurals.end())
	{
		for (int k = 0; k != (*nit).m_connection.size(); k++)
		{
			if (k == (*nit).m_connection.size() - 1)
				output = 1;
			else
				//(*nit).m_connection[k].neuralIdx
				//输出层的第K个连线的上一级神经元的序号 的输出
				output = preLayer->m_neurals[(*nit).m_connection[k].neuralIdx].output;
			//权重更新 连接线上误差
			dErrWrtDwn[(*nit).m_connection[k].weightIdx] += output*dErrWrtDyn[ii];
			//dErrWrtDwn 连接线上的误差
			//个数应该为连接线的个数
			//每一个连接线误差=连接的上一层网络神经元的输出* 该层网络神经元的误差
			//上一层网络神经元的输出 通过当前节点连接线直接就找到索引
			//preLayer->m_neurals[(*nit).m_connection[k].neuralIdx].output
		}
		++nit;
		++ii;
	}
	int j(0);
	nit = m_neurals.begin();
	while (nit != m_neurals.end())
	{
		for (int k = 0; k != (*nit).m_connection.size()-1; k++)
		{
			//统计上一层网络节点的传递误差
			//dErrWrtDxnm
			//dErrWrtDyn[j] 当前层神经元的误差
			//m_weights[(*nit).m_connection[k].weightIdx] 权重
			//二者相乘 得到上一层节点的误差 这个相当于误差反向传播
			dErrWrtDxnm[(*nit).m_connection[k].neuralIdx] += dErrWrtDyn[j] * m_weights[(*nit).m_connection[k].weightIdx];
		}
		++j;
		++nit;
	}
	for (int i = 0; i != m_weights.size(); i++)
	{
		m_weights[i] -= eta*dErrWrtDwn[i];
	}
}