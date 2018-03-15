//
// Created by ruiy on 18-3-14.
//

/**
 *依据已知样本值 x 1 , x 2 , L , x n 对 AR ( p ) 模型作出估计 称为自回归模型拟合自回归
 *模型拟合内容包括
 *1 AR ( p ) 模型阶数 p 的估计
 *2 AR ( p ) 模型中参数 α 1 , α 2 , L , α p 与 σ 2 的估计
 *3 对模型作拟合检验
 */
#include "ar.h"
#include "math_utils.h"
#include <iostream>
#include <cmath>

/**
 * 根据计算aic自动选取p
 * 需要根据自协方差AutoCov和计算，并要事先定下p的上界p_max，从[1-p_max]中选取最小aic的p
 * aic(k) =  ln [ likehood(k) ^ 2 ] + 2 * k / n , 计算k从1开始计算. 因为aic(0) = auto_cov[0]
 * 注意：本程序中getBiasCor得到的是从滞后1开始的, getAutoCov是从滞后0开始的
 * likehood(k) ^ 2 = auto_cov[0] - [BiasCor(1) ... BiasCor(k)] * [AutoCov(1) ... AutoCov(k)]'
 *
 */
std::vector<double> get_p(std::vector<double> AutoCov, std::vector<double> BiasCor, int p_max){
    std::vector<double> aic;
    int n = AutoCov.size();
    aic.push_back(AutoCov[0]);
    for (int k = 1; k <= p_max; k++) {
        double likehood_square = AutoCov[0];
        for (int j = 1; j <= k ; j++) {
            likehood_square -= BiasCor[j-1] * AutoCov[j];
        }
        std::cout << likehood_square << std::endl;
        aic.push_back(log(likehood_square) + 2 * k / double(n));
    }

    std::cout<<"Aic：begin"<<std::endl;
    for(int k=0;k<aic.size();k++){
        std::cout<<aic[k]<<" ";
    }
    std::cout<<"Aic：end"<<std::endl;

    return aic;
}


void print_bais_cor(std::vector<double> data)
{
    double mean; //输入数据的均值
    std::vector<double> auto_cor;//自相关系数AutoCorrelation
    std::vector<double> bias_cor;//偏相关系数

    auto_cor = get_auto_cor(data); //得到的自相关系数
    bias_cor = get_bias_cor(auto_cor); // 得到偏相关系数

    for(int k=0;k<bias_cor.size();k++){
        std::cout<<bias_cor[k]<<"\t";
    }
}


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


void formatData(std::vector<double> data,int p){
    std::vector<double> tmpy;
    for(int i=p;i<data.size();i++){
        tmpy.push_back(data[i]);
        std::vector<double> tmp;
        for(int j=i-1;j>=i-p;j--){
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

}

/**
 *最小二乘法求参数
 * a = inv(t(x) _*_ x) _*_ t(x) _*_ Y
 * e = sum(a) / (n-p)
 */
std::vector<double> LeastSquares(std::vector<double> data,int p){
    formatData(data,p);

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

/**
 *得到e
 */
double getBias(std::vector<double> data,std::vector<double> a,int n,int p){
    double sum = 0;
    std::vector<double> calPN(data.begin(),data.begin()+p);


    for(int i=p;i<data.size();i++){
        double s = 0;
        int t = calPN.size();
        for(int j=0;j<p;j++){
            s += a[j] * calPN[t-j-1];
        }
        calPN.push_back(s);
    }

    //std::cout<<calPN.size()<<std::endl;
    //计算残差
    for(int i=p;i<calPN.size();i++){
        sum += (data[i] - calPN[i]);
    }

    return sum / (n-p);
}


/**
 *检验模型
 *1、提出假设H
 *2、根据得到的参数，求出 data[p+1, ..., n]
 *3、计算残差，
 *4、球残差的自协方差系数，判断H是否独立
 *
 *x[t] = sum[j: 0...p]{a[j]*data[t-j]} + e
 *若 { ρ(ε), k = 0,1,2, ... , n } 中 约 有 68.3% 的 点 落 在 纵 坐 标ρ = ± 1 / n 内
 *约 有 95.4% 的 点 落 在 纵 坐 标 ρ = ± 2 / n 内
 *( ε[p+1] , ε[p+2] ,..., ε[n]) 为独立序列样本值 此时接受H0 否则拒绝H0

 */

int calP_N(std::vector<double> data,std::vector<double> a,int p){

    int n = data.size();
    std::vector<double> calPN(data.begin(),data.begin()+p);

    for(int i=p;i<data.size();i++){
        double s = 0;
        int t = calPN.size();
        for(int j=0;j<p;j++){
            s += a[j] * calPN[t-j-1];
        }
        calPN.push_back(s);
    }

    //std::cout<<calPN.size()<<std::endl;
    std::vector<double> var;
    //计算残差
    for(int i=p;i<calPN.size();i++){
        var.push_back(data[i] - calPN[i]);
    }

    std::vector<double> Avar;//自相关系数AutoCorrelation
    std::vector<double> Bvar;//偏相关系数


    /**
    for(int k=0;k<Avar.size();k++){
        std::cout<<Avar[k]<<"\t";

    */
    freopen("rvar16.xls", "w", stdout);

    std::cout<<"自相关系数:"<<std::endl;
    for(int k=0;k<Avar.size();k++){
        std::cout<<Avar[k]<<"\t";
    }
    std::cout<<std::endl;

    //检验是否有 68.3% 的 点 落 在 纵 坐 标ρ = ± 1 / n 内
    //约 有 95.4% 的 点 落 在 纵 坐 标 ρ = ± 2 / n 内
    int k1 = 0,k2 = 0;
    double p1 = 1.0 / Avar.size(),p2 = 2.0 / Avar.size();
    for(int k=0;k<Avar.size();k++){
        if(Avar[k] >= -p1 && Avar[k] <= p1) k1++;
        if(Avar[k] >= -p2 && Avar[k] <= p2) k2++;
    }
    std::cout<<"ρ = ± 1 / n："<<k1*1.0 / Avar.size()<<std::endl;
    std::cout<<"ρ = ± 2 / n："<<k2*1.0 / Avar.size()<<std::endl;

    /**
     *检测之后发现对于该数据序列：只有
     *有 16.7% 的 点 落 在 纵 坐 标ρ = ± 1 / n 内
     *有 22.2% 的 点 落 在 纵 坐 标 ρ = ± 2 / n 内
     *故H0的假设不能被接受，可以在会换个数据进行测试。
     */

    return 0;
}

/**
 *根据模型预测以后的数据，k表示第k个数据，这里k大于n,注意时间序列，必须先预测得到n，才能得到n+1，
 *如果给出的k>n，会预测[n,k]的所有位置，并添加大原数据上。
 */

double predict(std::vector<double> &data,std::vector<double> a,int k,int p){
    double res;
    for(int i=data.size();i<k;i++){
        double s = 0;
        int t = data.size();
        for(int j=0;j<p;j++){
            s += a[j] * data[t-j-1];
        }
        data.push_back(s);
    }
    return data[k-1];
}


void test_ar() {
    //北京1987-2014人口: 35
    std::vector<double> data = {871.5,897.1,904.3,919.2,935.0,950.0,965.0,981.0,1028.0,1047.0,1061.0,1075.0,
                   1086.0,1094.0,1102.0,1112.0,1125.0,1251.1,1259.4,1240.0,1245.6,1257.2,1363.6,
                   1385.1,1423.2,1456.4,1492.7,1538.0,1601.0,1676.0,1771.0,1860.0,1961.9,2018.6,2069.3
    };
    //读入数据
    int p = 16;//原则上p可以选择[16-N],但是由于p大于18时，计算会超过精度，又考虑到模型尽可能简单，参数尽可能少，选择p=[15,16,17]
    //这里分别用excel纪录了图像描述
    std::vector<double> a;

    std::vector<double> auto_cov = get_auto_cov(data);

    std::vector<double> bias_cor = get_bias_cor(auto_cov); // 得到偏相关系数
    std::cout<<"bias cor: 偏自相关系数：begin"<<std::endl;
    std::cout<<bias_cor.size()<<std::endl;
    for(int k=0;k<bias_cor.size();k++){
        std::cout<<bias_cor[k]<<" ";
    }
    std::cout<<std::endl;
    std::cout<<"bias cor: 偏自相关系数：end"<<std::endl;

    get_p(auto_cov, bias_cor, 30);

    a = LeastSquares(data,p);

    std::cout<<"参数a个数:  "<<a.size()<<std::endl;
    for(int i=0;i<a.size();i++){
        std::cout<<"a["<<i<<"] = "<<a[i]<<std::endl;
    }

    std::cout<<std::endl;


    //std::cout<<e<<std::endl;

    //检验算法的性能,可以设置p=[16,17]通过作图，观测算法的性能
    calP_N(data,a,p);

    //预测第37个数据的值
    double x = predict(data,a,37,p);
    std::cout<<x<<std::endl;
}
