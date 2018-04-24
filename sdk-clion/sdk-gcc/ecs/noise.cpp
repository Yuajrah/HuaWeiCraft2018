//
// Created by ruiy on 18-4-4.
//
#include "noise.h"
#include <algorithm>

/**
 * 按照平均值去噪, 没有什么效果, 不用
 * @param data
 * @return
 */
std::map<int, std::vector<double>> remove_noise_1th(std::map<int, std::vector<double>> data){
    std::map<int, std::vector<double>> res;
    for (auto &d: data) {
        res[d.first] = remove_noise_by_avg(d.second, 3);
    }
    return res;
};

std::vector<double> remove_noise_by_avg(std::vector<double> data, int day){
    std::vector<double> avg;
    for (int i=0;i<data.size()-day+1;i++) {
        avg.push_back(std::accumulate(data.begin()+i, data.begin()+i+day, 0.0) / static_cast<double>(day));
    }
    int start = day/2;
    int threshold = 6;
    for (int i=0;i<data.size()-day+1;i++) {
        if (abs(data[start + i] - avg[i]) > threshold) {
            data[start + i] = avg[i];
        };
    }
    return data;
};

/**
 * 箱型图 去噪
 * @return
 */
std::vector<double> remove_noise_by_box(std::vector<double> data){
    std::vector<double> res(data);

    std::sort(data.begin(), data.end(), [](const double &o1, const double &o2){
        return o1 < o2;
    });

    // 计算第一四分位数
    double w  = (data.size()+1) / 4;
    int y = floor(w); // w - 整数部分
    double z = w - y; // w - 小数部分
    double q1 = data[y-1] + z * (data[y] - data[y-1]);


    // 计算第二四分位数
    w = (data.size() + 1) / 2;
    y = floor(w);
    z = w - y;
    double q2 = data[y-1] + z * (data[y] - data[y-1]);

    // 计算第三四分位数
    w = 3 * (data.size() + 1) / 4;
    y = floor(w);
    z = w - y;
    double q3 = data[y-1] + z * (data[y] - data[y-1]);

    double iqr = q3 - q1;
    double low_bounder = q1 - 1.5 * iqr;
    double upper_bounder = q3 + 1.5 * iqr;

    for (int i=0;i<res.size();i++) {
        if (res[i] < low_bounder || res[i] > upper_bounder) { // 如果是异常值, 则替代掉
            if (i == 0) {
                res[i] = res[i+1];
            } else if (i == res.size() - 1) {
                res[i] = res[i-1];
            } else {
                res[i] = (res[i-1] + res[i+1]) / 2;
            }
        }
    }
    return res;
}