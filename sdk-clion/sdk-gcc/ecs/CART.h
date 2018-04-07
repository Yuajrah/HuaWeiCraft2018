//
// Created by caocongcong on 18-4-2.
//

#ifndef SDK_CLION_CART_H
#define SDK_CLION_CART_H

#include <vector>
#include <map>
#include "Node.h"
#include <math.h>
class decision_tree
{
public:
    /*
     * max_depth: 树的最大深度
     * min_samples_split:叶节点需要划分的最小特征数
     * min_samples_leaf:也节点的最小特征数
     * max_feature:进行划分的时候的最大特征
     * max_leaf_number:叶节点的最大个数
     * min_inpurity_split:划分的最小不纯度
     */
    decision_tree(int max_depth = 6, int min_samples_split = 2, double min_impurity_split=0.0);
    ~decision_tree(){}
    //进行训练
    void train(std::vector<std::vector<double >>, std::vector<double>);
    //进行测试
    double predict(std::vector<double>);
private:
    int _max_depth;
    int _min_samples_split;
    double _min_impurity_split;
    int _max_node_len;

    std::vector<Node> nodes;
};
#endif //SDK_CLION_CART_H
