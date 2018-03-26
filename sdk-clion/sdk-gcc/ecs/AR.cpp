//
// Created by ruiy on 18-3-16.
//

#include "AR.h"
#include "math_utils.h"
#include "type_def.h"
#include <cfloat>
#include <cmath>
#include <cstdio>

/**
 *依据已知样本值 x 1 , x 2 , L , x n 对 AR ( p ) 模型作出估计 称为自回归模型拟合自回归
 *模型拟合内容包括
 *1 AR ( p ) 模型阶数 p 的估计
 *2 AR ( p ) 模型中参数 α 1 , α 2 , L , α p 与 σ 2 的估计
 *3 对模型作拟合检验
 *
 */

AR::AR(std::vector<double> data):data(data){};

/**
 * 拟合数据
 * ic代表不同的定阶方式：
 *      1. 若ic为none,则max_lag即为lag,若max_log为-1，则 max_lag为int(round(12 * pow((data.size() / 100.0), 1.0/4)))
 *      2. 若
 *
 */
void AR::fit(std::string ic, int max_lag) {

    if (max_lag == -1) {
        max_lag = int(round(12 * pow((data.size() / 100.0), 1.0/4)));
    }

    if (ic=="none") {
        this->best_p = max_lag;
        std::pair<std::vector<double>, double> a_ssr = least_squares(this->data, max_lag);
        this->a = a_ssr.first;
        return;
    } else if (ic=="aic" || ic=="bic" || ic=="hqic") {
        int len = this->data.size();
        double min_ic = DBL_MAX;
        for (int lag=1;lag<=max_lag;lag++) {
            std::pair<std::vector<double>, double> a_ssr = least_squares(std::vector<double>(this->data.begin()+max_lag-lag, this->data.end()), lag);
            double sigma2 = a_ssr.second / (len - max_lag);

            double ic_val;
            if (ic == "aic") {
                ic_val = log(sigma2) + 2 * (2.0 + lag) / (len - max_lag);
            } else if (ic == "bic") {
                ic_val = log(sigma2) + log(len - max_lag) * (2.0 + lag) / (len - max_lag);
            } else if (ic == "hqic") {
                ic_val = log(sigma2) + 2 * log(log(len - max_lag)) * (2.0 + lag) / (len - max_lag);
            }
            // printf("%f %f %f\n",  a_ssr.second, sigma2, aic);
            if (ic_val < min_ic) {
                min_ic = ic_val;
                this->best_p = lag;
            }
        }
        std::pair<std::vector<double>, double> a_ssr = least_squares(this->data, this->best_p);
        this->a = a_ssr.first;
        return;
    }
}

/**
 *自协方差 AutoCov[k] = E((x[i] - u)(x[i-k] - u))
 *自相关系数 AutoCov[k] = AutoCov[k] / AutoCov[0]
 */

std::vector<double> AR::get_auto_cov(){
    //计算自相关系数矩阵
    int n = data.size();

    double mean = 0; //数据的均值
    for(int i=0;i<n;i++){
        mean += data[i];
    }
    mean /= n;
    //std::cout<<"mean::"<<mean<<std::endl;
    //将每个数据都减去均值得到新的数据
    std::vector<double> prodata;

    for(int i=0;i<n;i++){
        prodata.push_back(data[i] - mean);
        //std::cout<<"prodata[i] "<<prodata[i]<<std::endl;
    }

    std::vector<double> AutoCov(n,0);//自协方差AutoCovariance
    for(int k=0;k<n;k++){
        for(int i=0;i<n-k;i++){
            AutoCov[k] += prodata[i] * prodata[i+k];
        }
        AutoCov[k] /= n - k;
    }

    return AutoCov;
}

std::vector<double> AR::get_auto_cor(std::vector<double> auto_cov){

    std::vector<double> auto_cor;

    for(int k=0;k<auto_cov.size();k++){
        auto_cor.push_back(auto_cov[k] / auto_cov[0]);
    }

    return auto_cor;
}


/**
 *最小二乘法求参数
 * a = inv(t(x) _*_ x) _*_ t(x) _*_ Y
 * e = sum(a) / (n-p)
 */
std::pair<std::vector<double>, double> AR::least_squares(std::vector<double> data, int lag){
    std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> form_data = format_data(data, lag);
    std::vector<std::vector<double>> x = form_data.first;
    std::vector<std::vector<double>> y = form_data.second;

    std::vector<std::vector<double> > a, tx,invx,tmp;
    tx = t(x);
    invx = inv_lu(mulMat(tx, x));

    a = mulMat(mulMat(invx,tx), y);
    std::vector<std::vector<double>> ta = t(a);



    std::vector<std::vector<double>> y_estimate = mulMat(x, a);
    double ssr = 0;
    for (int i=0;i<y_estimate.size();i++) {
        ssr += pow(y_estimate[i][0] - y[i][0], 2);
    }

    return {ta[0], ssr};
}

std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> AR::format_data(std::vector<double> data, int lag){

    /**
*统计后可以明显看到在k=16之后，|BiasCor[k]| < 5,因此选择p = 16之后的数都可以
*之后开始球参数，并检验数据
*使用最小二乘法，求参数a 和 e，
* a = inv(t(x) _*_ x) _*_ t(x) _*_ Y
* e = sum(a) / (n-p)
* t(x) : 对x球转置
* r(x) : 对x求逆矩
* -*- : 矩阵乘法
* inv(x): 矩阵的逆
*/


    std::vector<std::vector<double> > y; //y = [data[p+1, ..., n]]
    std::vector<std::vector<double> > x;
    /**
  [ data[p, ..., 1] ]
  [ data[p+1, ..., 2] ]
  [ data[p+2, ..., 3] ]
  .
  .
  .
  [ data[n-1, ..., n-p] ]
 */
    std::vector<double> tmpy;
    for(int i=lag;i<data.size();i++){
        tmpy.push_back(data[i]);
        std::vector<double> tmp{1};
        for(int j=i-1;j>=i-lag;j--){
            tmp.push_back(data[j]);
        }
        x.push_back(tmp);
    }
    y.push_back(tmpy);
    y = t(y);
    return {x, y};
}

/**
 *
 * 根据模型预测以后的数据，k表示要预测接下来的k个数据
 *
 */

std::vector<double> AR::predict(int k, int diff_day){ // 预测接下来k天的数据
    std::vector<double> data_copy(data);
    for(int i=0;i<k;i++){
        double s = a[0];
        int t = data_copy.size();
        for(int j=0;j<best_p;j++){
            s += a[j+1] * data_copy[t-j-1];
        }
        if (diff_day > 0) { // 如果做了差分, 则负值不归0
            data_copy.push_back(s);
        } else {
            data_copy.push_back(s < 0? 0: s);
        }
    }


    std::vector<double> predict_res(data_copy.begin() + data.size(), data_copy.end());
    this->res.assign(predict_res.begin(), predict_res.end());

    return predict_res;
}

/**
 *得到e
 */
double AR::get_bias(){
    double sum = 0;
    std::vector<double> cal_pn(data.begin(),data.begin()+best_p); // 获取数据前p个值


    for(int i=best_p;i<data.size();i++){
        double s = 0;
        int t = cal_pn.size();
        for(int j=0;j<best_p;j++){
            s += a[j] * cal_pn[t-j-1];
        }
        cal_pn.push_back(s);
    }

    //std::cout<<calPN.size()<<std::endl;
    //计算残差
    for(int i=best_p;i<cal_pn.size();i++){
        sum += (data[i] - cal_pn[i]);
    }

    return sum / (data.size()-best_p);
}

/**
 * 打印模型的相关信息
 */
void AR::print_model_info() {
    printf("最佳滞后阶：best_p = %d", best_p);
//    printf("\n\nauto_cov size：size = %d\n", auto_cov.size());
//    for (auto t: auto_cov) {
//        printf("%f ", t);
//    }
//    printf("\n\nauto_cor size：size = %d\n", auto_cor.size());
//    for (auto t: auto_cor) {
//        printf("%f ", t);
//    }
//    printf("\n\nbias_cor size：size = %d\n", bias_cor.size());
//    for (auto t: bias_cor) {
//        printf("%f ", t);
//    }
//    printf("\n\nic_vals size：size = %d\n", ic_vals.size());
//    for (auto t: ic_vals) {
//        printf("%f ", t);
//    }
//    printf("\n\nnoise_var size：size = %d\n", noise_var.size());
//    for (auto t: noise_var) {
//        printf("%f ", t);
//    }
    printf("\n\npredict res size：size = %d\n", res.size());
    for (auto t: res) {
        printf("%f ", t);
    }
    printf("\n\na size：size = %d\n", a.size());
    for (auto t: a) {
        printf("%f ", t);
    }
}