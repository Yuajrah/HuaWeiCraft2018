//
// Created by caocongcong on 18-4-5.
//

#include "RandomForest.h"

RandomForest::RandomForest(int tree_num, int split_feature_num, int max_depth, int min_samples_split, double min_impurity_split)
{
    for(int i =0; i<tree_num; i++)
    {
        decision_tree tmp(max_depth,min_samples_split, min_impurity_split);
        trees.push_back(tmp);
    }
    _split_feather_num = split_feature_num;
    _tree_num = tree_num;
}

//训练函数
void RandomForest::train(std::vector<std::vector<double>> train, std::vector<double> target)
{
    int primary_trian_feathe = train[0].size();
    for (int i = 0; i < _tree_num; ++i) {
        printf("当前训练的树：%d\n",i);
        //保存此次的训练数据
        std::vector<std::vector<double>> tmp_train;
        //获取训练数据的index
        std::vector<int> train_index = random_vector(primary_trian_feathe, _split_feather_num);
        //单个样本的训练数据
        for (int j = 0; j <train.size() ; ++j) {
            std::vector<double> tmp_sample;
            for (int k: train_index) {
                tmp_sample.push_back(train[j][k]);
            }
            tmp_train.push_back(tmp_sample);
        }
        trees[i].train(tmp_train,target);
    }
}

//预测函数
double RandomForest::predict(std::vector<double> predict_data) {
    double result;
    for (int i = 0; i < _tree_num; ++i) {
        result += trees[i].predict(predict_data);
    }
    return result/_tree_num;
}
//选取特针
//从max个数中随机抽取number个数字
std::vector<int> RandomForest::random_vector(int max, int number)
{
    std::vector<int> result;
    std::vector<int> data;
    for(int i=0; i<max;i++)
    {
        data.push_back(i);
    }
    int end_index = max-1;
    while(number > 0)
    {
        int index = Random::random_int(0,end_index);
        result.push_back(data[index]);
        int tmp = data[index];
        data[index] = data[end_index];
        data[end_index] = tmp;
        end_index--;
        number--;
    }
    return result;
}