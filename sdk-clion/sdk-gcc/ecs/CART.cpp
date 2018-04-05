//
// Created by caocongcong on 18-4-2.
//

#include "CART.h"
decision_tree::decision_tree(int max_depth, int min_samples_split, int min_samples_leaf, double min_impurity_split)
{
    _max_depth = max_depth;
    _min_samples_leaf = min_samples_leaf;
    _min_samples_split = min_samples_split;
    _min_impurity_split = min_impurity_split;
    _max_node_len = pow(2,_max_depth);
}

void decision_tree::train(std::vector<std::vector<double >>train_data, std::vector<double> target_data)
{
    Node head(train_data, target_data,_min_impurity_split);
    for(int i=0; i< _max_node_len; i++)
    {
        Node tmp;
        nodes.push_back(tmp);
    }
    nodes[1] = head;
    for(int i =1; i<_max_node_len; i++)
    {
        printf("训练到第%d个节点:\n",i);
        //如果树为空，直接跳过
        if(nodes[i].is_null()==1)
        {
            continue;
        }
        //判断是否超过最大层
        if(i*2+1>_max_node_len)
        {
            nodes[i].set_leaf_node();
            continue;
        }
        if(nodes[i].find_best_sample())
        {
            //得到左右节点，并进行重复建树
            std::vector<std::vector<double>> left_train = nodes[i].get_left_train();
            std::vector<std::vector<double>> right_train = nodes[i].get_right_train();
            std::vector<double> left_target = nodes[i].get_left_target();
            std::vector<double> right_target = nodes[i].get_right_target();
            Node left(left_train,left_target,_min_impurity_split);
            Node right(right_train,right_target,_min_impurity_split);
            nodes[2*i] = left;
            nodes[2*i+1] = right;
        }
        else
        {
            nodes[i].set_leaf_node();
        }
    }
}

double decision_tree::predict(std::vector<double> input)
{
    int index = 1;
    while(index<_max_node_len)
    {
        if(nodes[index].is_null() == 1)
        {
            printf("*********************************************\n");
            printf("******GGGGGGGGGGGGGGGGGGGGGGGGGGGGG**********\n");
            printf("*********************************************\n");
        }
        if(nodes[index].is_leaf_node()==1)
        {
            return nodes[index].get_mean_value();
        }
        else
        {
            if(input[nodes[index].get_feather_id()] < nodes[index].get_theta())
            {
                index *=2;
            }
            else
            {
                index *=2;
                index +=1;
            }
        }
    }
    printf("没有找到啊！！！！！！！！！！！！！！！！");
    return -1;
}
