//
// Created by ruiy on 18-3-14.
//

#ifndef SDK_GCC_AR_H
#define SDK_GCC_AR_H

#include <vector>
typedef long double Double;
/**
 *依据已知样本值 x 1 , x 2 , L , x n 对 AR ( p ) 模型作出估计 称为自回归模型拟合自回归
 *模型拟合内容包括
 *1 AR ( p ) 模型阶数 p 的估计
 *2 AR ( p ) 模型中参数 α 1 , α 2 , L , α p 与 σ 2 的估计
 *3 对模型作拟合检验
 */



int Calculate_p(std::vector<Double> data);



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


/**
 *根据p得到数据矩阵形式
 */
/*std::vector<std::vector<Double> > y; //y = [data[p+1, ..., n]]
std::vector<std::vector<Double> > x;*/
/**
  [ data[p, ..., 1] ]
  [ data[p+1, ..., 2] ]
  [ data[p+2, ..., 3] ]
  .
  .
  .
  [ data[n-1, ..., n-p] ]
 */


void formatData(std::vector<Double> data,int p);

/**
 *最小二乘法求参数
 * a = inv(t(x) _*_ x) _*_ t(x) _*_ Y
 * e = sum(a) / (n-p)
 */
std::vector<Double> LeastSquares(std::vector<Double> data,int p);
/**
 *得到e
 */
Double getBias(std::vector<Double> data,std::vector<Double> a,int n,int p);


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

int calP_N(std::vector<Double> data,std::vector<Double> a,int p);

/**
 *根据模型预测以后的数据，k表示第k个数据，这里k大于n,注意时间序列，必须先预测得到n，才能得到n+1，
 *如果给出的k>n，会预测[n,k]的所有位置，并添加大原数据上。
 */

Double predict(std::vector<Double> &data,std::vector<Double> a,int k,int p);

void test_ar();

#endif //SDK_GCC_AR_H
