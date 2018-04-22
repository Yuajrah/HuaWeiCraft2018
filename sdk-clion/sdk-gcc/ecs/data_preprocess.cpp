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
    int tmp_split = split_windows;
    std::map<std::vector<double>, double> result;
    std::vector<double> used_data = ecs_data;

    if (mv)
    {
        used_data = ma(ecs_data,move_step);
    }
    if(split_high_flag)
    {
        used_data = split_high(used_data, split_rate);
    }
    if(split_choosed)
    {
        tmp_split = int(round(12 * pow((used_data.size() / 100.0), 1.0/4)));
    }
    else
    {
        tmp_split = split_windows;
    }

    std::vector<double> tmp_train;
    int index = 0;
    while(index < tmp_split) {
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

std::vector<std::vector<double>> timeseries_to_supervised_data(std::vector<double> ecs_data, int split_windows, bool mv )
{
    int tmp_split = split_windows;
    std::vector<std::vector<double>> result;
    std::vector<double> used_data = ecs_data;
    if (mv)
    {
        used_data = ma(ecs_data,move_step);
    }
    if(split_high_flag)
    {
        used_data = split_high(used_data, split_rate);
    }
    if(split_choosed)
    {
        tmp_split = int(round(12 * pow((used_data.size() / 100.0), 1.0/4)));
    }
    else
    {
        tmp_split = split_windows;
    }
    std::vector<double> tmp_train;
    int index = 0;
    while(index < tmp_split) {
        tmp_train.push_back(used_data[index]);
        index++;
    }
    while(index < used_data.size())
    {
        result.push_back(tmp_train);
        tmp_train.erase(tmp_train.begin());
        tmp_train.push_back(used_data[index]);
        index++;
    }
    return result;
}

std::vector<double> timeseries_to_supervised_target(std::vector<double> ecs_data, int split_windows, bool mv)
{
    int tmp_split = split_windows;
    std::vector<double> result;
    std::vector<double> used_data = ecs_data;
    if (mv)
    {
        used_data = ma(ecs_data,move_step);
    }
    if(split_high_flag)
    {
        used_data = split_high(used_data, split_rate);
    }
    if(split_choosed)
    {
        tmp_split = int(round(12 * pow((used_data.size() / 100.0), 1.0/4)));
    }
    else
    {
        tmp_split = split_windows;
    }
    int index = 0;
    while(index < tmp_split) {
        index++;
    }
    while(index < used_data.size())
    {
        double tmp_test = used_data[index];
        result.push_back(tmp_test);
        index++;
    }
    return result;
}
std::vector<double>  get_frist_predict_data(std::vector<double>ecs_data, int split_windows, bool mv )
{
    int tmp_split = split_windows;
    std::vector<double> result;
    std::vector<double> used_data = ecs_data;
    if (mv)
    {
        used_data = ma(ecs_data,move_step);
    }
    if(split_high_flag)
    {
        used_data = split_high(used_data, split_rate);
    }
    if(split_choosed)
    {
        tmp_split = int(round(12 * pow((used_data.size() / 100.0), 1.0/4)));
    }
    else
    {
        tmp_split = split_windows;
    }
    int n = used_data.size();
    for (int i = n - tmp_split; i < n; i++)
    {
        result.push_back(used_data[i]);
    }
    return result;
}


std::vector<std::vector<double >> get_vector_train(std::map<std::vector<double>, double> input)
{
    std::vector<std::vector<double>> result;
    for(auto t:input)
    {
        result.push_back(t.first);
    }
    return result;
}
std::vector<double>  get_vector_target(std::map<std::vector<double>, double> input)
{
    std::vector<double> result;
    for(auto t:input)
    {
        result.push_back(t.second);
    }
    return result;
}


std::vector<std::vector<double >> get_vector_train_method2(std::vector<std::vector<double>>input,int predict_need_date)
{
    std::vector<std::vector<double>> result;
    for(int i =0; i<input.size()-predict_need_date; i++)
    {
        result.push_back(input[i]);
    }
    return result;
}
std::vector<double>  get_vector_target_method2(std::vector<double> input, int predict_need_date)
{
    std::vector<double> result;
    for (int i = predict_need_date; i < input.size(); ++i) {
        result.push_back(input[i]);
    }
    return result;
}

std::vector<std::vector<double>>  get_vector_test_method2(std::vector<std::vector<double>> input, int predict_need_date)
{
    std::vector<std::vector<double>> result;
    for(int i =input.size()-predict_need_date; i<input.size(); i++)
    {
        result.push_back(input[i]);
    }
    return result;
}

std::vector<double> split_high(std::vector<double>data, double rate)
{
    double sum = 0.0;
    double mean = 0.0;
    std::vector<double> result;
    for (int i = 0; i < data.size(); ++i) {
        sum += data[i];
    }
    mean = sum/data.size();
    double bias = mean*rate;
    for (int j = 0; j <data.size() ; ++j) {
        if(data[j] > bias)
        {
            result.push_back(bias);
        } else{
            result.push_back(data[j]);
        }
    }
    return result;
}

//一阶指数平滑
// data 原始数据 alpha占比 initNum: 初始值的个数
std::vector<double> smoothOrderOne(std::vector<double> data, double alpha, int initNum = 3) {
    std::vector<double> result;
    double initData = 0.0;
    for (int i = 0; i < initNum; ++i)
    {
        initData += data[i];
    }
    initData /= initNum;
    for (int j = 0; j < data.size(); ++j) {
        double tmp = 0.0;
        if(j==0)
        {
            tmp = alpha*data[j]+(1-alpha)*initData;
        }
        else
        {
            tmp = alpha*data[j]+(1-alpha)*result[j-1];
        }
        result.push_back(tmp);
    }
    return result;

}
/**
 *
 * @param data
 * @param lag
 * @param interval
 * @return
 */
std::pair<std::vector<std::vector<double>>, std::vector<double>> format_data(std::vector<double> data, int lag, int interval, bool constant){


    std::vector<double> y; //y = [data[p+1, ..., n]]
    std::vector<std::vector<double> > x;
/**
[ data[p, ..., 1] ] p+1
[ data[p+1, ..., 2] ] p+2
[ data[p+2, ..., 3] ] p+3
.
.
.
[ data[n-1, ..., n-p] ] n
*/

/**
[ data[1, ..., p] ] p+interval+1
[ data[2, ..., p+1] ] p+interval+2
[ data[3, ..., p+2] ] p+interval+3
.
.
.
[ data[n-p-interval, ..., n-1-interval] ] n
*/
    for(int i=lag;i<data.size()-interval;i++){
        y.push_back(data[i+interval]);

        std::vector<double> tmp;
        for(int j=i-lag;j<=i-1;j++){
            tmp.push_back(data[j]);
        }

        if (constant) {
            tmp.push_back(1);
        }

        x.push_back(tmp);
    }
    return {x, y};
}


//将所有的划分集合到一个函数里面
usedData getData(std::vector<double>ecs_data, std::string Mode, int moveStep, double alpha)
{
    usedData result;
    std::vector<double> used_data = ecs_data;
    if(Mode == "Ma")
    {
        used_data = ma(ecs_data, moveStep);
    }
    else if(Mode == "Smooth1")
    {
        used_data = smoothOrderOne(ecs_data, alpha);
    }
    int tmp_split = int(round(12 * pow((used_data.size() / 100.0), 1.0/4)));
    std::vector<std::vector<double>> train;
    std::vector<double> tmp_train;
    int index = 0;
    while(index < tmp_split) {
        tmp_train.push_back(used_data[index]);
        index++;
    }
    while(index < used_data.size())
    {
        train.push_back(tmp_train);
        tmp_train.erase(tmp_train.begin());
        tmp_train.push_back(used_data[index]);
        index++;
    }
    std::vector<double> target;
    index = 0;
    while(index < tmp_split) {
        index++;
    }
    while(index < used_data.size())
    {
        double tmp_test = used_data[index];
        target.push_back(tmp_test);
        index++;
    }
    std::vector<double> fristPredict;
    int n = used_data.size();
    for (int i = n - tmp_split; i < n; i++)
    {
        fristPredict.push_back(used_data[i]);
    }
    result.fristPredictData = fristPredict;
    result.trainData = train;
    result.targetData = target;
    return result;
}
