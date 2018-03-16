//
// Created by ruiy on 18-3-14.
//

#include "math_utils.h"
#include <iostream>
#include <cmath>
#include <cstdio>




/**
 *得到偏相关系数BiasCor[k,k]
 *BiasCor[0,0] = AutoCor[0]
 *BiasCor[k,k] = (AutoCor[k-1] - sum[j:0...k-1]{AutoCor[k-j]*BiasCor[j,k-1]}) / (1 - sum[j:0...k-1]AutoCor[j]*BiasCor[j,k-1])
 *BiasCor[j,k] = BiasCor[j,k-1] - BiasCor[k,k]*BiasCor[k-j,k-1] j = 0...k
 *
 */

/*
 * auto_cov的长度，就是输入数据的长度len
 * 偏自相关系数，没有滞后为0的时候的值，只有滞后1,一直到滞后len-1的时候的值，因此a的大小实际上是len-1 * len-1的矩阵
 * 对角线元素，即为滞后的偏自相关系数
 * 输入 auto_cov，从滞后0开始，一直到滞后数据长度-1
 */
std::vector<double> get_bias_cor(std::vector<double> auto_cov) {
    // 迭代得到矩阵a
    std::vector<std::vector<double>> a;
    a.push_back(std::vector<double>{auto_cov[1] / auto_cov[0]}); // a[0][0], 即为a11
    for (int k=1;k<auto_cov.size()-1;k++) {
        double t1 = 0;
        double t2 = 0;
        for (int j=1;j<=k;j++) {
            t1 += auto_cov[k+1-j]*a[k-1][j-1];
            t2 += auto_cov[j]*a[k-1][j-1];
        }
        a.push_back(std::vector<double>(k+1, -1));
        a[k][k] = (auto_cov[k+1] - t1) / (auto_cov[0] - t2);
        for (int j=1;j<=k;j++) {
            a[k][j-1] = (a[k-1][j-1] - a[k][k] * a[k-1][k-j]);
        }
    }

    for (int i=0;i<a.size();i++) {
        std::cout << std::endl;
        for (int j=0;j<a[i].size();j++) {
            std::cout << a[i][j] << " ";
        }
    }
    std::cout << std::endl;

    // 偏自相关系数
    std::vector<double> bias_cor;
    for (int k=0;k<auto_cov.size()-1;k++) {
        bias_cor.push_back(a[k][k]);
    }

    // auto_cov的长度，就是输入数据的长度len
    // 白噪声的方差，从0开始计数，一直到len-1
    std::vector<double> noise_var;
    noise_var.push_back(auto_cov[0]);
    for (int k = 1; k < auto_cov.size(); k++) {
        noise_var.push_back(noise_var[k-1] * (1 - a[k-1][k-1]));
    }
    return bias_cor;
}

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
std::vector<std::vector<double> > t(std::vector<std::vector<double> > x){
    //x的装置矩阵
    std::vector<std::vector<double> > tx;
    //tx初始化便于直接访问下标,这是原矩阵的转置的形式
    for(int i=0;i<x[0].size();i++){
        std::vector<double> tmp(x.size(),0);
        tx.push_back(tmp);
    }

    for(int i=0;i<x.size();i++){
        for(int j=0;j<x[0].size();j++){
            tx[j][i] = x[i][j];
        }
    }
    return tx;
}

/**
 *矩阵乘法
 */
std::vector<std::vector<double> > mulMat(std::vector<std::vector<double> > tx, std::vector<std::vector<double> > x){
    std::vector<std::vector<double> > res;
    //初始化结果矩阵的格式row(tx) X col(x)
    for(int i=0;i<tx.size();i++){
        std::vector<double> tmp(x[0].size(),0);
        res.push_back(tmp);
    }

//    std::cout<<res.size()<<" "<<res[0].size()<<std::endl;
    for(int i=0;i<tx.size();i++){
        for(int j=0;j<x[0].size();j++){
            for(int k=0;k<x.size();k++){
                res[i][j] += tx[i][k] * x[k][j];
            }
        }
    }
    return res;
}

/**
 *矩阵的行列式，行列变化为上三角矩阵
 */

double det(std::vector<std::vector<double> > x){
    //只有一个元素
    //if(x.size() == 1 && x[0].size() == 1) return x[0][0];

    double det = 1;
    //交换数组指定的两行，即进行行变换（具体为行交换）
    int iter = 0;  //记录行变换的次数（交换）
    for(int i=0;i<x.size();i++){
        if(x[i][i] == 0){
            for(int j=i+1;j<x.size();j++){
                if(x[j][i] != 0){
                    swap(x[i],x[j]);//交换两行
                    iter ++;
                }
            }
        }
        for(int k=i+1;k<x.size();k++){
            double yin = -1 * x[k][i] / x[i][i] ;
            for(int u=0; u<x[0].size(); u++){
                x[k][u] = x[k][u] + x[i][u] * yin;
            }
        }
    }

    /**
       std::cout<<"上三角矩阵："<<std::endl;
       for(int i=0;i<x.size();i++){
        for(int j=0;j<x[0].size();j++){
            std::cout<<x[i][j]<<" ";
        }
        std::cout<<std::endl;
    }**/
    for(int i=0;i<x.size();i++){//求对角线的积 即 行列式的值
        det = det * x[i][i];
    }
    //行变换偶数次符号不变
    if(iter%2 == 1)  det= -det;

    return det;
}

/**
 *删除矩阵的第r行，第c列
 */
std::vector<std::vector<double> > delMat(std::vector<std::vector<double> > x,int r,int c){
    std::vector<std::vector<double> > Ax;
    for(int i=0;i<x.size();i++){
        std::vector<double> tmp;
        for(int j=0;j<x[0].size();j++){
            if(i != r && j != c) tmp.push_back(x[i][j]);
        }
        if(i != r) Ax.push_back(tmp);
    }
    return Ax;
}


/**
 *求矩阵的伴随矩阵
 */
std::vector<std::vector<double> > A(std::vector<std::vector<double> > x){
    std::vector<std::vector<double> > tmp(x),res;

    //tx初始化便于直接访问下标,这是原矩阵的转置的形式
    for(int i=0;i<x.size();i++){
        std::vector<double> tp(x[0].size(),0);
        res.push_back(tp);
    }

    for(int i=0;i<x.size();i++){
        for(int j=0;j<x[0].size();j++){
            tmp = x;
            tmp = delMat(tmp,i,j);
            res[i][j] = ((i+j)%2==0?1:-1) * det(tmp);

        }
    }
    return res;
}


/**
 *矩阵的逆
 */
std::vector<std::vector<double> > inv(std::vector<std::vector<double> > x){
    std::vector<std::vector<double> > res = A(x);
    double dets = det(x);
    for(int i=0;i<res.size();i++){
        for(int j=0;j<res[0].size();j++){
            res[i][j] /= dets;
        }
    }
    return res;
}


/**
 *合并两个行相同的矩阵
 */
std::vector<std::vector<double> > ConRows(std::vector<std::vector<double> > x, std::vector<std::vector<double> > y){
    //行相同，添加列
    for(int i=0;i<y.size();i++){
        for(int j=0;j<y[0].size();j++){
            x[i].push_back(y[i][j]);
        }
    }
    return x;
}

/**
 *合并两个列相同的矩阵
 */
std::vector<std::vector<double> > ConCols(std::vector<std::vector<double> > x, std::vector<std::vector<double> > y){
    //列相同，添加行
    for(int i=0;i<y.size();i++){
        std::vector<double> row;
        for(int j=0;j<y[0].size();j++){
            row.push_back(y[i][j]);
        }
        x.push_back(row);
    }
    return x;
}






/**
 *测试矩阵运算成功
 */
void test_Mat(){
    std::vector<std::vector<double> > data,tdata,res,Ax;
    //data = getdata();
    double x[] = {2,1,-1,2,1,0,1,-1,1};

    for(int i=0;i<3;i++){
        std::vector<double> tmp;
        data.push_back(tmp);
        for(int j=0;j<3;j++){
            data[i].push_back(x[i*3+j]);
        }
    }

    /**
    tdata = t(data);
    for(int i=0;i<tdata.size();i++){
        for(int j=0;j<tdata[0].size();j++){
            std::cout<<tdata[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    res = mulMat(tdata,data);
    for(int i=0;i<res.size();i++){
        for(int j=0;j<res[0].size();j++){
            std::cout<<res[i][j]<<" ";
        }
        std::cout<<std::endl;
    }

    std::cout<<det(data)<<std::endl;
    */

    //Ax = inv(data);
    std::cout<<det(data)<<std::endl;
    std::cout<<"逆矩阵:"<<std::endl;
    for(int i=0;i<Ax.size();i++){
        for(int j=0;j<Ax[0].size();j++){
            std::cout<<Ax[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}

/**
int main()
{
    std::vector<double> data;
    test_Mat();
    return 0;
}
**/