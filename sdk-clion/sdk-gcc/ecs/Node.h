//
// Created by caocongcong on 18-4-3.
//

#ifndef SDK_CLION_NODE_H
#define SDK_CLION_NODE_H

#include <map>
#include <vector>
#include "type_def.h"
#include<algorithm>
class Node
{
public:
    Node();
    Node(std::vector<std::vector<double >>, std::vector<double>, double min_impurity_split,int min_samples_split);
    //进行寻找最佳的分配，如果寻找不到，返回false
    ~Node(){}
    bool find_best_sample();
    //最终选择的特征ID
    int get_feather_id();
    //选择特针的门
    double get_theta();
    //设置成也节点
    void set_leaf_node();
    //是否是也节点
    int is_leaf_node();
    //返回左节点的训练样本
    std::vector<std::vector<double>> get_left_train();
    //返回右节点的训练样本
    std::vector<std::vector<double>> get_right_train();
    //返回左节点的目标样本
    std::vector<double> get_left_target();
    //返回右节点的目标样本
    std::vector<double> get_right_target();
    //判断是否为null
    int is_null();
    //返回当前节点的平均值
    double get_mean_value();

private:
    //标记是否为叶节点
    int is_leaf;
    //标记最好的划分点
    int best_sample_index;
    //
    double best_sample_value;
    //不纯度最小下降之
    double max_decrease_impurity;

    //最小的不纯度
    double min_impurity;
    //训练数据
    std::vector<std::vector<double>> train_data;
    //目标zhi
    std::vector<double> target_data;
    double val(std::vector<double>);
    double mean_value;
    int feather_number;
    bool find_min_impurity(std::vector<Node_index> feather_value_index, double &theta, double&);
    double get_val(std::vector<Node_index> feather_value_index);
    double primary_impurity;
    std::vector<std::vector<double>> left_train;
    std::vector<std::vector<double>> right_train;
    std::vector<double> left_target;
    std::vector<double> right_target;
    void split_child_node();
    //判断是否为null
    int is_null_flag;
    int _min_samples_split;
};
#endif //SDK_CLION_NODE_H
