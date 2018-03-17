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

void AR::fit(std::string ic, int p, int p_max) {



    std::vector<Double> auto_cov = get_auto_cov(); // 得到自协方差
    std::vector<Double> auto_cor = get_auto_cor(auto_cov); // 得到自协方差
    this->auto_cov = auto_cov;
    this->auto_cor = auto_cor;
    // 迭代得到矩阵a
    std::vector<std::vector<Double>> aa;
    aa.push_back(std::vector<Double>{auto_cov[1] / auto_cov[0]});
    for (int k=1;k<auto_cov.size()-1;k++) {
        Double t1 = 0;
        Double t2 = 0;
        for (int j=1;j<=k;j++) {
            t1 += auto_cov[k+1-j]*aa[k-1][j-1];
            t2 += auto_cov[j]*aa[k-1][j-1];
        }
        aa.push_back(std::vector<Double>(k+1, -1));
        aa[k][k] = (auto_cov[k+1] - t1) / (auto_cov[0] - t2);
        for (int j=1;j<=k;j++) {
            aa[k][j-1] = (aa[k-1][j-1] - aa[k][k] * aa[k-1][k-j]);
        }
    }

    // 偏自相关系数,暂时用不到，以后可能有用
    std::vector<Double> bias_cor;
    for (int k=0;k<auto_cov.size()-1;k++) {
        bias_cor.push_back(aa[k][k]);
    }
    this->bias_cor = bias_cor;

    // auto_cov的长度，就是输入数据的长度len
    // 白噪声的方差，从0开始计数，一直到len-1
    std::vector<Double> noise_var;
    noise_var.push_back(auto_cov[0]);
    for (int k = 1; k < auto_cov.size(); k++) {
        noise_var.push_back(noise_var[k-1] * (1 - aa[k-1][k-1]));
    }
    this->noise_var = noise_var;

    // 找出滞后[1..p_max]中aic最小的p_max
    Double min_aic = DBL_MAX;
    if (ic=="none") { // 如果传入none，则根据数据长度瞎jb定
        this->best_p = int(round(12 * pow((data.size() / 100), 1/4)));
    } else if (ic=="aic") { // 如果不传入p，则默认值为-1，这是使用aic定阶，这时p_max才有用
        if (p_max == -1) {
            p_max = data.size()-1;
        }
        p_max = p_max % data.size();
        int best_p = -1;
        for (int k=1;k<=p_max;k++) {
            ic_vals.push_back(log(noise_var[k]) + 2 * k / Double(data.size()));
            if (ic_vals.back() < min_aic) { // aic的计算公式
                best_p = k;
                min_aic = ic_vals.back();
            }
        }
        this->best_p = best_p;
    } else if (ic=="bic") {
        if (p_max == -1) {
            p_max = data.size()-1;
        }
        p_max = p_max % data.size();
        int best_p = -1;
        for (int k=1;k<=p_max;k++) {
            ic_vals.push_back(log(noise_var[k]) + k * log(data.size()) / Double(data.size()));
            if (ic_vals.back() < min_aic) { // aic的计算公式
                best_p = k;
                min_aic = ic_vals.back();
            }
        }
        this->best_p = best_p;
    } else if (ic=="manual") { // 手动赋值
        if (p == -1) {
            this->best_p = 1;
        }
        this->best_p = p;
    } else if (ic=="none_and_least_square") { // 佛性定阶并使用最小二乘法计算自回归系数a[1..p]
        this->best_p = int(round(12 * pow((data.size() / 100), 1/4)));
        std::vector<Double> a = least_squares();
        this->a.assign(a.begin(), a.end());
        return;
    }
    this->a.assign(aa[this->best_p - 1].begin(), aa[this->best_p - 1].end());
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
std::vector<Double> AR::least_squares(){
    std::pair<std::vector<std::vector<Double>>, std::vector<std::vector<Double>>> form_data = format_data();
    std::vector<std::vector<Double>> x = form_data.first;
    std::vector<std::vector<Double>> y = form_data.second;

    std::vector<std::vector<Double> > a, tx,invx,tmp;
    tx = t(x);
    invx = inv(mulMat(tx, x));
    for (auto t: inv(mulMat(tx, x))) {
        printf("\n");
        for (auto tt: t) {
            printf("%Lf ", tt);
        }
    }
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
    printf("\nDebug begin\n");
    a = mulMat(mulMat(invx,tx), y);
/*    printf("\nsize: %d %d \n", a.size(), a[0].size());
    for (auto t: a) {
        printf("%f ", t[0]);
    }*/

    a = t(a);
    // printf("%d size = %d", a.size());
    printf("\nDebug end\n");
    return a[0];
}

std::pair<std::vector<std::vector<Double>>, std::vector<std::vector<Double>>> AR::format_data(){

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

    std::vector<Double> tmpy;
    for(int i=best_p;i<data.size();i++){
        tmpy.push_back(data[i]);
        std::vector<Double> tmp;
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

std::vector<Double> AR::predict(int k){ // 预测接下来k天的数据
    std::vector<Double> data_copy(data); // 拷贝构造
    for(int i=0;i<k;i++){
        Double s = 0;
        int t = data_copy.size();
        for(int j=0;j<best_p;j++){
            s += a[j] * data_copy[t-j-1];
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

void AR::print_model_info() {
    printf("最佳滞后阶：best_p = %d", best_p);
    printf("\n\nauto_cov size：size = %d\n", auto_cov.size());
    for (auto t: auto_cov) {
        printf("%f ", t);
    }
    printf("\n\nauto_cor size：size = %d\n", auto_cor.size());
    for (auto t: auto_cor) {
        printf("%f ", t);
    }
    printf("\n\nbias_cor size：size = %d\n", bias_cor.size());
    for (auto t: bias_cor) {
        printf("%f ", t);
    }
    printf("\n\nic_vals size：size = %d\n", ic_vals.size());
    for (auto t: ic_vals) {
        printf("%f ", t);
    }
    printf("\n\nnoise_var size：size = %d\n", noise_var.size());
    for (auto t: noise_var) {
        printf("%f ", t);
    }
    printf("\n\nres size：size = %d\n", res.size());
    for (auto t: res) {
        printf("%f ", t);
    }
    printf("\n\nsum = %d", sum);
}