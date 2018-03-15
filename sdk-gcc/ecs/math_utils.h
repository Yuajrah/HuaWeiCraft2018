//
// Created by ruiy on 18-3-14.
//

#ifndef SDK_GCC_MATH_UTILS_H
#define SDK_GCC_MATH_UTILS_H

#include <vector>
/**
 *自协方差 AutoCov[k] = E((x[i] - u)(x[i-k] - u))
 *自相关系数 AutoCov[k] = AutoCov[k] / AutoCov[0]
 */
std::vector<double> get_auto_cov(std::vector<double> data);
std::vector<double> get_auto_cor(std::vector<double> data);

/**
 *得到偏相关系数BiasCor[k,k]
 *BiasCor[0,0] = AutoCor[0]
 *BiasCor[k,k] = (AutoCor[k-1] - sum[j:0...k-1]{AutoCor[k-j]*BiasCor[j,k-1]}) / (1 - sum[j:0...k-1]AutoCor[j]*BiasCor[j,k-1])
 *BiasCor[j,k] = BiasCor[j,k-1] - BiasCor[k,k]*BiasCor[k-j,k-1] j = 0...k
 *
 */
std::vector<double> get_bias_cor(std::vector<double> auto_cov);

/**
 * a = inv(t(x) _*_ x) _*_ t(x) _*_ Y
 * t(x) : 对x球转置
 * r(x) : 对x求逆矩
 * -*- : 矩阵乘法
 * inv(x): 矩阵的逆
 */


/**
 *矩阵转置
 */
std::vector<std::vector<double> > t(std::vector<std::vector<double> > x);

/**
 *矩阵乘法
 */
std::vector<std::vector<double> > mulMat(std::vector<std::vector<double> > tx, std::vector<std::vector<double> > x);

/**
 *矩阵的行列式，行列变化为上三角矩阵
 */

double det(std::vector<std::vector<double> > x);

/**
 *删除矩阵的第r行，第c列
 */
std::vector<std::vector<double> > delMat(std::vector<std::vector<double> > x,int r,int c);


/**
 *求矩阵的伴随矩阵
 */
std::vector<std::vector<double> > A(std::vector<std::vector<double> > x);


/**
 *矩阵的逆
 */
std::vector<std::vector<double> > inv(std::vector<std::vector<double> > x);


/**
 *合并两个行相同的矩阵
 */
std::vector<std::vector<double> > ConRows(std::vector<std::vector<double> > x, std::vector<std::vector<double> > y);

/**
 *合并两个列相同的矩阵
 */
std::vector<std::vector<double> > ConCols(std::vector<std::vector<double> > x, std::vector<std::vector<double> > y);






/**
 *测试矩阵运算成功
 */
void test_Mat();

#endif //SDK_GCC_MATH_UTILS_H
