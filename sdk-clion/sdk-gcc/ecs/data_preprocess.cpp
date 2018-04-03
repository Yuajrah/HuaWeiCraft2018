//
// Created by caocongcong on 18-3-31.
//
#include "data_preprocess.h"

double get_mean(std::vector<double> train_data,  double rate)
{
    double result = -1;
    int count = 0;
    double sum = 0.0;

    //统计不为0的个数以及总和
    for (auto data : train_data)
    {
        if (data > 0.1)
        {
            count++;
            sum += data;
        }
    }
    int num = train_data.size();
    double cul_rate = count/(num+0.0);
//    printf ("current rate: %f\n", cul_rate);
    if (cul_rate<=rate) {
        result = sum / num;
    }
    return result;
}

std::map<int, int> change_by_mean_vaule(std::map<int, std::vector<double>> trian_data, std::map<int, Vm> vm_info, double rate, int need_predict_day,
                            std::map<int, int> predict_data)
{
    for (auto &t: vm_info) {
        double tmp_result = get_mean(trian_data[t.first], rate);
        if (tmp_result >0)
        {
//            printf("flavor number %d\n", t.first);
            predict_data [t.first] =  tmp_result*need_predict_day;
        }
    }
    return predict_data;
}
std::map<std::vector<double>, double> timeseries_to_supervised(std::vector<double> ecs_data, int split_windows, bool mv )
{
    std::map<std::vector<double>, double> result;
    std::vector<double> used_data = ecs_data;
    if (mv)
    {
        used_data = ma(ecs_data,6);
    }
    std::vector<double> tmp_train;
    int index = 0;
    while(index < split_windows) {
        tmp_train.push_back(used_data[index]);
        index++;
    }
    while(index < used_data.size())
    {
        double tmp_test = used_data[index];
        result[tmp_train] = tmp_test;
        tmp_train.erase(tmp_train.begin());
        tmp_train.push_back(used_data[index]);
        index++;
    }
    return result;
}
std::vector<double>  get_frist_predict_data(std::vector<double>ecs_data, int split_windows, bool mv )
{
    std::vector<double> result;
    std::vector<double> used_data = ecs_data;
    if (mv)
    {
        used_data = ma(ecs_data,6);
    }
    int n = used_data.size();
    for (int i = n - split_windows; i < n; i++)
    {
        result.push_back(used_data[i]);
    }
    return result;
}

float* get_frist_preditc(std::vector<double>ecs_data, int split_windows, bool mv)
{
    float* result = new float[ecs_data.size()];
    std::vector<double> used_data = ecs_data;
    if (mv)
    {
        used_data = ma(ecs_data,6);
    }
    int n = used_data.size();
    for (int i = n - split_windows; i < n; i++)
    {
        result[i] = (float)used_data[i];
    }
    return result;
}

float** get_float_train(std::map<std::vector<double>, double> input, int split_windows)
{
    float** result = new float*[input.size()];
    int index =0;
    for (auto item:input)
    {
        result[index] = new float[split_windows];
        for (int i=0; i<split_windows; i++)
        {
            result[index][i] = (float)item.first[i];
        }
        index++;
    }
    return result;
}

float* get_float_test(std::map<std::vector<double>, double> input)
{
    float* result = new float[input.size()];
    int index =0;
    for (auto item:input)
    {
        result[index] = (float)item.second;
    }
    return result;
}

float* add_one_data(float* primary_data, float need_add, int len)
{
    float* tmp = new float[len];
    for (int i=0; i<len-1; i++)
    {
        tmp[i] = primary_data[i+1];
    }
    tmp[len-1] = need_add;
    return tmp;
}