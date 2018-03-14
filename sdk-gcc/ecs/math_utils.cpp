//
// Created by ruiy on 18-3-14.
//

#include "math_utils.h"
#include <iostream>
#include <cmath>
#include <cstdio>

/**
 *自协方差 AutoCov[k] = E((x[i] - u)(x[i-k] - u))
 *自相关系数 AutoCov[k] = AutoCov[k] / AutoCov[0]
 */

std::vector<Double> getAutoCov(std::vector<Double> data){
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


std::vector<Double> getAutoCor(std::vector<Double> data){

    std::vector<Double> AutoCor,AutoCov;//自相关系数AutoCorrelation，注意下标从0开始
    AutoCov = getAutoCov(data);

    /**
    std::cout<<"AutoCor:自协方差：begin"<<std::endl;
    for(int k=0;k<AutoCov.size();k++){
    	std::cout<<AutoCov[k]<<" ";
    }
    std::cout<<"AutoCor:自协方差：end"<<std::endl;
    **/

    for(int k=0;k<data.size()-1;k++){
        AutoCor.push_back(AutoCov[k+1] / AutoCov[0]);
    }

    /**
    std::cout<<"AutoCor:自协方差系数：begin"<<std::endl;
    for(int k=0;k<AutoCor.size();k++){
        std::cout<<AutoCor[k]<<" ";
    }
    std::cout<<"AutoCor:自协方差系数：end"<<std::endl;
    **/
    return AutoCor;
}


/**
 *得到偏相关系数BiasCor[k,k]
 *BiasCor[0,0] = AutoCor[0]
 *BiasCor[k,k] = (AutoCor[k-1] - sum[j:0...k-1]{AutoCor[k-j]*BiasCor[j,k-1]}) / (1 - sum[j:0...k-1]AutoCor[j]*BiasCor[j,k-1])
 *BiasCor[j,k] = BiasCor[j,k-1] - BiasCor[k,k]*BiasCor[k-j,k-1] j = 0...k
 *
 */
std::vector<Double> getBiasCor(std::vector<Double> AutoCor){
    //计算BiasCor[i,j],为了直接访问下标，首先初始化
    std::vector< std::vector<Double> > BiasCor;
    for(int i=0;i<AutoCor.size();i++){
        std::vector<Double> tmp(AutoCor.size(),0);
        BiasCor.push_back(tmp);
    }

    BiasCor[0][0] = AutoCor[0];

    for(int k=1;k<AutoCor.size();k++){
        BiasCor[k][k] = AutoCor[k];
        Double t1,t2;
        for(int j=0;j<=k-1;j++){
            t1 = AutoCor[k-j] * BiasCor[j][k-1];
            t2 = AutoCor[j] * BiasCor[j][k-1];

            BiasCor[j][k] = BiasCor[j][k-1] - BiasCor[k][k] * BiasCor[k-j][k-1];

        }
        BiasCor[k][k] = (BiasCor[k][k] - t1) / t2;
        for(int j=0;j<=k-1;j++){
            BiasCor[k][j] = BiasCor[j][k] = BiasCor[j][k-1] - BiasCor[k][k] * BiasCor[k-j][k-1];
        }
    }
    std::vector<Double> res;
    for(int k=0;k<AutoCor.size();k++){
        res.push_back(BiasCor[k][k]);
    }

    return res;

}

/**
 * 根据计算aic自动选取p
 * 需要根据自协方差AutoCov和计算，并要事先定下p的上界p_max，从[1-p_max]中选取最小aic的p
 * aic(k) =  ln [ likehood(k) ^ 2 ] + 2 * k / n , 计算k从1开始计算. 因为aic(0) = auto_cov[0]
 * 注意：本程序中getBiasCor得到的是从1开始的，getAutoCor也是1开始的
 * likehood(k) ^ 2 = auto_cov[0] - [BiasCor(1) ... BiasCor(k)] * [AutoCov(1) ... AutoCov(k)]'
 *
 */
std::vector<Double> get_p(std::vector<Double> AutoCov, std::vector<Double> BiasCor, int p_max){
    std::vector<Double> aic;
    int n = AutoCov.size();
    aic.push_back(AutoCov[0]);
    for (int k = 1; k <= p_max; k++) {
        Double sum = 0;
        for (int j = 0; j < k ; j++) {
            sum += BiasCor[j] * AutoCov[j];
        }
        Double likehood_square = AutoCov[0] - sum;
        aic.push_back(log(likehood_square) + 2 * k / Double(n));
    }
    for (auto a: aic) {
        printf("%f ", a);
    }
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
std::vector<std::vector<Double> > t(std::vector<std::vector<Double> > x){
    //x的装置矩阵
    std::vector<std::vector<Double> > tx;
    //tx初始化便于直接访问下标,这是原矩阵的转置的形式
    for(int i=0;i<x[0].size();i++){
        std::vector<Double> tmp(x.size(),0);
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
std::vector<std::vector<Double> > mulMat(std::vector<std::vector<Double> > tx, std::vector<std::vector<Double> > x){
    std::vector<std::vector<Double> > res;
    //初始化结果矩阵的格式row(tx) X col(x)
    for(int i=0;i<tx.size();i++){
        std::vector<Double> tmp(x[0].size(),0);
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

Double det(std::vector<std::vector<Double> > x){
    //只有一个元素
    //if(x.size() == 1 && x[0].size() == 1) return x[0][0];

    Double det = 1;
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
            Double yin = -1 * x[k][i] / x[i][i] ;
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
std::vector<std::vector<Double> > delMat(std::vector<std::vector<Double> > x,int r,int c){
    std::vector<std::vector<Double> > Ax;
    for(int i=0;i<x.size();i++){
        std::vector<Double> tmp;
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
std::vector<std::vector<Double> > A(std::vector<std::vector<Double> > x){
    std::vector<std::vector<Double> > tmp(x),res;

    //tx初始化便于直接访问下标,这是原矩阵的转置的形式
    for(int i=0;i<x.size();i++){
        std::vector<Double> tp(x[0].size(),0);
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
std::vector<std::vector<Double> > inv(std::vector<std::vector<Double> > x){
    std::vector<std::vector<Double> > res = A(x);
    Double dets = det(x);
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
std::vector<std::vector<Double> > ConRows(std::vector<std::vector<Double> > x, std::vector<std::vector<Double> > y){
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
std::vector<std::vector<Double> > ConCols(std::vector<std::vector<Double> > x, std::vector<std::vector<Double> > y){
    //列相同，添加行
    for(int i=0;i<y.size();i++){
        std::vector<Double> row;
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
    std::vector<std::vector<Double> > data,tdata,res,Ax;
    //data = getdata();
    Double x[] = {2,1,-1,2,1,0,1,-1,1};

    for(int i=0;i<3;i++){
        std::vector<Double> tmp;
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
    std::vector<Double> data;
    test_Mat();
    return 0;
}
**/