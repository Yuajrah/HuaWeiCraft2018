
//这个类是定义一个节点上的连接线
//这里连接线里保存是两个索引值，
//它表明条连接线的权重在整个权重向量中的索引与它连接的前面一层结点的索引。

class NNconnection
{
public:
	NNconnection(void);
	~NNconnection(void);
public:
	int weightIdx; //连接线的权重在整个权重向量中的索引
	int neuralIdx; //它连接的前面一层结点的索引
};