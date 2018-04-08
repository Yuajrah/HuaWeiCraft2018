//
// Created by caocongcong on 18-4-5.
//

#ifndef SDK_CLION_RANDOMFOREST_H
#define SDK_CLION_RANDOMFOREST_H

#include "Random.h"
#include "CART.h"
#include <vector>
class RandomForest
{
public:
    //初始化函数
    /*
     * tree_num: 森林中树的个数
     * max_depth: 树的最大深度
     * split_feature_num： 每棵树的特征个数
     * min_samples_split：最小样本划分
     * min_inpurity_split:划分的最小不纯度
 */
    RandomForest(int tree_num,  int split_feature_num, int max_depth, int min_samples_split, double min_impurity_split);

    //训练函数
    void train(std::vector<std::vector<double>> train, std::vector<double> target);

    //预测函数
    double predict(std::vector<double> predict_data);
    std::vector<int> random_vector(int max, int number);
private:
    std::vector<decision_tree> trees;

    //每颗树的训练特征数目
    double _split_feather_num;

    //树的数目
    int _tree_num;
};
#endif //SDK_CLION_RANDOMFOREST_H
