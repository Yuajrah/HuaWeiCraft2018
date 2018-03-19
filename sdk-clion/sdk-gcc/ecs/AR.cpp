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

AR::AR(std::vector<Double> data):data(data){};

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
        std::pair<std::vector<Double>, Double> a_ssr = least_squares(this->data, max_lag);
        this->a = a_ssr.first;
        return;
    } else if (ic=="aic" || ic=="bic") {
        int len = this->data.size();
        Double min_ic = DBL_MAX;
        for (int lag=1;lag<=max_lag;lag++) {
            std::pair<std::vector<Double>, Double> a_ssr = least_squares(std::vector<double>(this->data.begin()+max_lag-lag, this->data.end()), lag);
            Double sigma2 = a_ssr.second / (len - max_lag);

            Double ic_val;
            if (ic == "aic") {
                ic_val = log(sigma2) + 2 * (2.0 + lag) / (len - max_lag);
            } else if (ic == "bic") {
                ic_val = log(sigma2) + log(len - max_lag) * (2.0 + lag) / (len - max_lag);
            }
            // printf("%f %f %f\n",  a_ssr.second, sigma2, aic);
            if (ic_val < min_ic) {
                min_ic = ic_val;
                this->best_p = lag;
            }
        }
        std::pair<std::vector<Double>, Double> a_ssr = least_squares(this->data, this->best_p);
        this->a = a_ssr.first;
        return;
    }
}

/**
 *自协方差 AutoCov[k] = E((x[i] - u)(x[i-k] - u))
 *自相关系数 AutoCov[k] = AutoCov[k] / AutoCov[0]
 */

std::vector<Double> AR::get_auto_cov(){
    //计算自相关系数矩阵
    int n = data.size();

    Double mean = 0; //数据的均值
    for(int i=0;i<n;i++){
        mean += data[i];
    }
    mean /= n;
    //std::cout<<"mean::"<<mean<<std::endl;
    //将每个数据都减去均值得到新的数据
    std::vector<Double> prodata;

    for(int i=0;i<n;i++){
        prodata.push_back(data[i] - mean);
        //std::cout<<"prodata[i] "<<prodata[i]<<std::endl;
    }

    std::vector<Double> AutoCov(n,0);//自协方差AutoCovariance
    for(int k=0;k<n;k++){
        for(int i=0;i<n-k;i++){
            AutoCov[k] += prodata[i] * prodata[i+k];
        }
        AutoCov[k] /= n - k;
    }

    return AutoCov;
}

std::vector<Double> AR::get_auto_cor(std::vector<Double> auto_cov){

    std::vector<Double> auto_cor;

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
std::pair<std::vector<Double>, Double> AR::least_squares(std::vector<double> data, int lag){
    std::pair<std::vector<std::vector<Double>>, std::vector<std::vector<Double>>> form_data = format_data(data, lag);
    std::vector<std::vector<Double>> x = form_data.first;
    std::vector<std::vector<Double>> y = form_data.second;

    std::vector<std::vector<Double> > a, tx,invx,tmp;
    tx = t(x);
    invx = inv(mulMat(tx, x));

    /**
    std::cout<<"invx:"<<std::endl;
    std::cout<<invx.size()<<" "<<invx[0].size()<<std::endl;
    for(int i=0;i<invx.size();i++){
        for(int j=0;j<invx[0].size();j++){
            std::cout<<invx[i][j]<<" ";
        }
        std::cout<<std::endl;
    }

    std::cout<<"tx:"<<std::endl;
    std::cout<<tx.size()<<" "<<tx[0].size()<<std::endl;
    for(int i=0;i<tx.size();i++){
        for(int j=0;j<tx[0].size();j++){
            std::cout<<tx[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    **/
    a = mulMat(mulMat(invx,tx), y);
    std::vector<std::vector<Double>> ta = t(a);

    std::vector<std::vector<Double>> y_estimate = mulMat(x, a);
    Double ssr = 0;
    for (int i=0;i<y_estimate.size();i++) {
        ssr += pow(y_estimate[i][0] - y[i][0], 2);
    }

    return {ta[0], ssr};
}

std::pair<std::vector<std::vector<Double>>, std::vector<std::vector<Double>>> AR::format_data(std::vector<double> data, int lag){

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


    std::vector<std::vector<Double> > y; //y = [data[p+1, ..., n]]
    std::vector<std::vector<Double> > x;
    /**
  [ data[p, ..., 1] ]
  [ data[p+1, ..., 2] ]
  [ data[p+2, ..., 3] ]
  .
  .
  .
  [ data[n-1, ..., n-p] ]
 */
    std::vector<Double> tmpy;
    for(int i=lag;i<data.size();i++){
        tmpy.push_back(data[i]);
        std::vector<Double> tmp{1};
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

std::vector<Double> AR::predict(int k){ // 预测接下来k天的数据
    std::vector<Double> data_copy(data);
    for(int i=0;i<k;i++){
        Double s = a[0];
        int t = data_copy.size();
        for(int j=0;j<best_p;j++){
            s += a[j+1] * data_copy[t-j-1];
        }
        data_copy.push_back(s);
    }
    std::vector<Double> predict_res(data_copy.begin() + data.size(), data_copy.end());
    this->res.assign(predict_res.begin(), predict_res.end());

    // 对预测结果求和并四舍五入
    Double sum = 0;
    for (auto t: res) {
        sum += t;
    }
    this->sum = round(sum<0?0:sum);

    return predict_res;
}

/**
 *得到e
 */
Double AR::get_bias(){
    Double sum = 0;
    std::vector<Double> cal_pn(data.begin(),data.begin()+best_p); // 获取数据前p个值


    for(int i=best_p;i<data.size();i++){
        Double s = 0;
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
    printf("\n\nsum = %d\n", sum);
}