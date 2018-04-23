#include "NNconnection.h"
#include <vector>
using namespace std;
//神经元类 节点类
//每个结点包含了一个输出和若干个连接线 其实就是一个简单的感知器
//若干连接线代表输入
class NNneural
{
public:
	NNneural(void);
	~NNneural(void);
	double output;
	vector<NNconnection> m_connection;
};