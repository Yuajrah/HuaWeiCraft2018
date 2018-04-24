#pragma once
#include "NNneural.h"
//单层神经网络类
#include <vector>


class NNlayer
{
public:
	NNlayer(){ preLayer = 0; }
	~NNlayer(void);


	NNlayer *preLayer; //一个指针成员来说明层与层之间的连接关系
	vector<NNneural> m_neurals; //每一层由大量节点构成
	vector<double> m_weights; //权值向量作为了每一层的成员

	void addNeurals(int num, int preNumNeurals);
	void backPropagate(vector<double>& dErrWrtDxn,vector<double>& dErrWrtDxnm,double eta);
};