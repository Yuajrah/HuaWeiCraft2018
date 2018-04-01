//
// Created by caocongcong on 18-3-31.
//

#include "KNN.h"

class T {
public:
    double distance;
    int index;
    double value;
    T(double a, int b, double c) {
        distance = a;
        index = b;
        value =c;
    }
};
bool operator < (const T &t1, const T &t2)
{
    //首先选距离小的
    if (t1.distance > t2.distance)
    {
        return true;
    }
        //相同距离选择index大的
    else if(t1.distance == t2.distance)
    {
        t1.index < t2.index;
    }
    return true;
}

double knn_regresion_brust (std::map<std::vector<double>, double> train_data,std::vector<double> predict_data, int k  )
{
    double result ;
    std::priority_queue <T> order;
    int i = 0;
    for (auto data : train_data)
    {
        double tmp_distance = distance(data.first,predict_data);
        T tmp_T(tmp_distance, i, data.second);
        order.push(tmp_T);
        i++;
    }
    std::vector<double> tmp_result;
    for (int i =0; i< k; i++)
    {
        T tmp = order.top();
        tmp_result.push_back(tmp.value);
        order.pop();
    }
    double sum =0;
    for(auto data:tmp_result)
    {
        sum += data;
    }
    result = sum /tmp_result.size();
    return result;
}


//计算欧式距离
double distance(std::vector<double> data1, std::vector<double> data2)
{
    double sum =0.0;
    for(int i = 0; i<data1.size(); i++)
    {
        sum += (data1[i] - data2[i])*(data1[i]-data2[i]);
    }
    return sum/data1.size();
}
