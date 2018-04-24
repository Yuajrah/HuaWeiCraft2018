//
// Created by caocongcong on 18-4-23.
//

#include "exponential.h"
std::vector<double> exponentialPredict(std::vector<double > data, int predictDate, double alpha)
{
    std::vector<double> order1 = smooth(data,alpha);
    std::vector<double> order2 = smooth(order1, alpha);
    return predictBySmooth2(order1, order2, predictDate, alpha);
}

std::vector<double> smooth(std::vector<double> data, double alpha)
{
    std::vector<double> result;
    int n = data.size();
    int initNum = 7;
    double initData = 0.0;
    for (int i = 0; i < initNum; ++i) {
        initData += data[i];
    }
    initData /= initNum;
    for (int j = 0; j < data.size(); ++j) {
        double tmp = 0.0;
        if(j ==0)
        {
            tmp = alpha*data[j] + (1-alpha)*initData;
        }
        else
        {
            tmp = alpha*data[j] +(1-alpha)*result[j-1];
        }
        result.push_back(tmp);
    }
    return result;
}


std::vector<double> predictBySmooth2(std::vector<double> Order1, std::vector<double> Order2,  int predictData, double alpha)
{
    std::vector<double> result;
//    double a = 2*Order1[Order1.size()-1]-Order2[Order2.size()-1];
//    double b = alpha/(1-alpha)*(Order1[Order1.size()-1]- Order2[Order2.size()-1]);
//    for (int i = 0; i < predictData ; ++i) {
//        double tmp = a + b *(i+1);
//        result.push_back(tmp);
//    }
    for(int i = Order1.size() - predictData; i< Order1.size(); i++)
    {
        result.push_back(Order1[i]);
    }
    return result;
}