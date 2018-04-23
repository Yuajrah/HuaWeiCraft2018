#include "NNlayer.h"
#include <vector>



//一个完整的神经网络类
class NeuralNetwork
{
public:
	NeuralNetwork(void);
	~NeuralNetwork(void);




	int nLayer; // 网络层数
	vector<int> nodes; // 每层的结点数
	vector<double> actualOutput; // 每次迭代的输出结果
	double etaLearningRate;    // 权值学习率
	int iterNum;    // 迭代次数


	vector<NNlayer*> m_layers;  // 整个网络层
	void create(int num_layers,int * ar_nodes);  // 创建网络
	void initializeNetwork();  // 初始化网络，包括设置权值等
	void forwardCalculate(vector<double>& invect,vector<double>& outvect); // 向前计算
	void backPropagate(vector<double>& tVect,vector<double>& oVect); //反向传播
	void train(vector<vector<double>>& inputVect,vector<vector<double>>& outputVect); //训练
	void classifer(vector<double>& inVect,vector<double>& outVect); // 分类
};