//
// Created by caocongcong on 18-4-3.
//

#include "Node.h"
Node::Node()
{
    is_null_flag = 1;
}
Node::Node(std::vector<std::vector<double >> given_train_data, std::vector<double>given_target_data, double min_impurity_split) {
    train_data = given_train_data;
    target_data = given_target_data;
    max_decrease_impurity = min_impurity_split;
    best_sample_index = 0;
    best_sample_value = 0.0;
    max_decrease_impurity = 0;
    is_leaf = 0;
    feather_number = train_data[0].size();
    primary_impurity = val(target_data);
    min_impurity = primary_impurity;
    is_null_flag = 0;
}

int Node::get_feather_id() {
    return best_sample_index;
}

double Node::get_theta() {
    return best_sample_value;
}

//对于每一个寻找最好的zhi
bool Node::find_best_sample() {
    if (train_data.size() <=1)
    {
        return false;
    }
    bool find_best = false;
    for(int i = 0; i<feather_number; i++)
    {
        std::vector<Node_index> feather_value_index;
        for(int j = 0; j < train_data.size(); j++)
        {
            Node_index tmp;
            tmp.value = train_data[j][i];
            tmp.id = j;
            tmp.target = target_data[j];
            feather_value_index.push_back(tmp);
        }
        double theta = 0.0;
        double tmp_min_impurity = 0.0;
        bool found = find_min_impurity(feather_value_index, theta,tmp_min_impurity);
        if (found) {
            if (tmp_min_impurity < min_impurity) {
                min_impurity = tmp_min_impurity;
                best_sample_index = i;
                best_sample_value = theta;
                find_best = true;
            }
        }
    }
    if(find_best)
    {
        split_child_node();
    }
    return find_best;
}

//在一个特征中选取出来下降最多的切割点
bool Node::find_min_impurity(std::vector<Node_index> feather_value_index, double &theta, double &tmp_min_impurity)
{
    double bias = 0.0;
    std::sort(feather_value_index.begin(),feather_value_index.end());
    tmp_min_impurity = primary_impurity;
    theta = feather_value_index[0].value;
    double tmp_theta = feather_value_index[0].value+bias;
    std::vector<Node_index> frist_split;
    int index = 0;
    while(index < feather_value_index.size())
    {
        //double型比大小，可能有问题
        if(feather_value_index[0].value <= tmp_theta)
        {
            Node_index tmp = feather_value_index[0];
            feather_value_index.erase(feather_value_index.begin());
            frist_split.push_back(tmp);
            index++;
        }
        else
        {
            //第一个序列的方差和
            double var1 = get_val(frist_split);
            //第二部分的方差和
            double var2 = get_val(feather_value_index);
            if(var1+var2 < tmp_min_impurity)
            {
                theta = tmp_theta;
                tmp_min_impurity = var1+var2;
            }
            index++;
            tmp_theta = feather_value_index[0].value+bias;
        }
    }
    if(tmp_min_impurity == primary_impurity)
    {
        return false;
    }
    else
    {
        return true;
    }
}


//计算均方误差
double Node::get_val(std::vector<Node_index> feather_value_index)
{
    double sum_val = 0.0;
    double sum = 0.0;
    for (int i = 0; i < feather_value_index.size(); ++i) {
        sum += feather_value_index[i].target;
    }
    double mean = sum/feather_value_index.size();
    for (int j = 0; j <feather_value_index.size() ; ++j) {
        sum_val += (feather_value_index[j].target -mean) *(feather_value_index[j].target - mean);
    }
    return sum_val;
}


int Node::is_leaf_node()
{
    return is_leaf;

}
//设置叶节点
void Node::set_leaf_node()
{
    is_leaf = 1;
    double sum = 0.0;
    for (int i = 0; i < target_data.size(); ++i) {
        sum += target_data[i];
    }
    mean_value = sum/target_data.size();
}

double Node::val(std::vector<double> data)
{
    double sum = 0.0;
    for (int i = 0; i <data.size() ; ++i) {
        sum += data[i];
    }
    double mean = sum/data.size();
    double val_value = 0.0;
    for (int j = 0; j < data.size(); ++j) {
        val_value+=(data[j]-mean)*(data[j]-mean);
    }
    return val_value;
}

//进行决策树的划分
void Node::split_child_node() {
    int id = best_sample_index;
    double value = best_sample_value;
    for (int i = 0; i < train_data.size(); ++i) {
        std::vector<double> tmp_train = train_data[i];
        double tmp_target = target_data[i];
        if(train_data[i][id] <= value)
        {
            left_train.push_back(tmp_train);
            left_target.push_back(tmp_target);
        }
        else
        {
            right_train.push_back(tmp_train);
            right_target.push_back(tmp_target);
        }
    }
}

//返回左节点train
std::vector<std::vector<double>> Node::get_left_train() {
    return left_train;
}

//返回右节点的train
std::vector<std::vector<double>> Node::get_right_train() {
    return right_train;
}

//返回左节点的test
std::vector<double> Node::get_left_target() {
    return left_target;
}

//返回左节点的train
std::vector<double> Node::get_right_target() {
    return right_target;
}

int Node::is_null()
{
    return is_null_flag;
}

//返回平均值
double Node::get_mean_value() {
    return mean_value;
}