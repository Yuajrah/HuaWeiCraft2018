//
// Created by ruiy on 18-3-16.
//

#include "AR.h"
#include "math_utils.h"
#include <cfloat>
#include <cmath>

/**
 *依据已知样本值 x 1 , x 2 , L , x n 对 AR ( p ) 模型作出估计 称为自回归模型拟合自回归
 *模型拟合内容包括
 *1 AR ( p ) 模型阶数 p 的估计
 *2 AR ( p ) 模型中参数 α 1 , α 2 , L , α p 与 σ 2 的估计
 *3 对模型作拟合检验
 *
 */

AR::AR(std::vector<double> data):data(data){};

void AR::fit(int p_max) {
    if (p_max == -1) {
        p_max = data.size()-1;
    }
    p_max = p_max % data.size();


    std::vector<double> auto_cov = get_auto_cov(); // 得到自协方差
    std::vector<double> auto_cor = get_auto_cor(auto_cov); // 得到自协方差
    this->auto_cov = auto_cov;
    this->auto_cor = auto_cor;
    // 迭代得到矩阵a
    std::vector<std::vector<double>> aa;
    aa.push_back(std::vector<double>{auto_cov[1] / auto_cov[0]});
    for (int k=1;k<auto_cov.size()-1;k++) {
        double t1 = 0;
        double t2 = 0;
        for (int j=1;j<=k;j++) {
            t1 += auto_cov[k+1-j]*aa[k-1][j-1];
            t2 += auto_cov[j]*aa[k-1][j-1];
        }
        aa.push_back(std::vector<double>(k+1, -1));
        aa[k][k] = (auto_cov[k+1] - t1) / (auto_cov[0] - t2);
        for (int j=1;j<=k;j++) {
            aa[k][j-1] = (aa[k-1][j-1] - aa[k][k] * aa[k-1][k-j]);
        }
    }

    // 偏自相关系数,暂时用不到，以后可能有用
    std::vector<double> bias_cor;
    for (int k=0;k<auto_cov.size()-1;k++) {
        bias_cor.push_back(aa[k][k]);
    }
    this->bias_cor = bias_cor;

    // auto_cov的长度，就是输入数据的长度len
    // 白噪声的方差，从0开始计数，一直到len-1
    std::vector<double> noise_var;
    noise_var.push_back(auto_cov[0]);
    for (int k = 1; k < auto_cov.size(); k++) {
        noise_var.push_back(noise_var[k-1] * (1 - aa[k-1][k-1]));
    }
    this->noise_var = noise_var;

    // 找出滞后[1..p_max]中aic最小的p_max
    double min_aic = DBL_MAX;
    int best_p = -1;
    for (int k=1;k<=p_max;k++) {
        aic.push_back(log(noise_var[k]) + 2 * k / double(data.size()));
        if (aic.back() < min_aic) { // aic的计算公式
            best_p = k;
        }
    }
    this->best_p = best_p;
    this->a.assign(aa[best_p - 1].begin(), aa[best_p - 1].end());
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
std::vector<double> AR::least_squares(){
    std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> form_data = format_data();
    std::vector<std::vector<double>> x = form_data.first;
    std::vector<std::vector<double>> y = form_data.second;

    std::vector<std::vector<double> > a, tx,invx,tmp;
    tx = t(x);
    invx = inv(mulMat(tx, x));
    //std::cout<<invx.size()<<std::endl;

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
    a = t(a);
    return a[0];
}

std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> AR::format_data(){

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

    std::vector<double> tmpy;
    for(int i=best_p;i<data.size();i++){
        tmpy.push_back(data[i]);
        std::vector<double> tmp;
        for(int j=i-1;j>=i-best_p;j--){
            tmp.push_back(data[j]);
        }
        x.push_back(tmp);
    }
    y.push_back(tmpy);
    y = t(y);

    /**
    std::cout<<"X:"<<std::endl;
    for(int i=0;i<x.size();i++){
        for(int j=0;j<x[0].size();j++){
            std::cout<<x[i][j]<<" ";
        }
        std::cout<<std::endl;
    }

    std::cout<<"y:"<<std::endl;
    for(int i=0;i<y.size();i++){
        for(int j=0;j<y[0].size();j++){
            std::cout<<y[i][j]<<" ";
        }
        std::cout<<std::endl;
    }**/
    return {x, y};
}

/**
 *根据模型预测以后的数据，k表示第k个数据，这里k大于n,注意时间序列，必须先预测得到n，才能得到n+1，
 *如果给出的k>n，会预测[n,k]的所有位置，并添加大原数据上。
 */

std::vector<double> AR::predict(int k){
    std::vector<double> data_copy(data); // 拷贝构造
    for(int i=data_copy.size();i<k;i++){
        double s = 0;
        int t = data_copy.size();
        for(int j=0;j<best_p;j++){
            s += a[j] * data_copy[t-j-1];
        }
        data_copy.push_back(s);
    }
    std::vector<double> predict_res(data_copy.begin() + data.size(), data_copy.end());
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

