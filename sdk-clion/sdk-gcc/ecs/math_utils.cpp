//
// Created by ruiy on 18-3-14.
//

#include "math_utils.h"
#include "BasicInfo.h"
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
/*    for (auto t: x) {
        printf("%f ", t[0]);
    }*/
    //printf(" size = %d ", x[0].size());
    //x的装置矩阵
    std::vector<std::vector<double> > tx;
    //tx初始化便于直接访问下标,这是原矩阵的转置的形式
    for(int i=0;i<x[0].size();i++){
        std::vector<double> tmp(x.size(),0);
        tx.push_back(tmp);
    }
    //printf(" size = %d ", tx[0].size());

    for(int i=0;i<x.size();i++){
        for(int j=0;j<x[0].size();j++){
            tx[j][i] = x[i][j];
        }
    }
/*    for (auto t: tx[0]) {
        printf("%f ", t);
    }*/
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
            // double yin = -1 * x[k][i] / x[i][i] ;
            double yin = x[i][i] == 0 ? 0: (-1 * x[k][i] / x[i][i]) ;
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
    std::vector<std::vector<double> > res;

    //tx初始化便于直接访问下标,这是原矩阵的转置的形式
    for(int i=0;i<x.size();i++){
        std::vector<double> tp(x[0].size(),0);
        res.push_back(tp);
    }

    for(int i=0;i<x.size();i++){
        for(int j=0;j<x[0].size();j++){
            std::vector<std::vector<double>> tmp = delMat(x,i,j);

/*            for (auto t: tmp) {
                printf("\n");
                for (auto tt: t) {
                    printf("%f ", tt);
                }
            }*/
            res[i][j] = ((i+j)%2==0?1:-1) * det(tmp);

        }
    }
    return res;
}


std::vector<std::vector<double>> inv_lu(std::vector<std::vector<double>> a){
    int N = a.size();
    std::vector<std::vector<double>>  l(N, std::vector<double>(N, 0));
    std::vector<std::vector<double>>  u(N, std::vector<double>(N, 0));
    std::vector<std::vector<double>>  l_inv(N, std::vector<double>(N, 0));
    std::vector<std::vector<double>>  u_inv(N, std::vector<double>(N, 0));
    std::vector<std::vector<double>>  a_inv(N, std::vector<double>(N, 0));

    float s,t;

    for (int i=0;i<N;i++) {
        l[i][i] = 1;
    }

    for (int i=0;i<N;i++) {
        for (int j = i;j < N;j++)
        {
            s = 0;
            for (int k = 0;k < i;k++)
            {
                s += l[i][k] * u[k][j];
            }
            u[i][j] = a[i][j] - s;      //按行计算u值
        }

        for (int j = i + 1;j < N;j++)
        {
            s = 0;
            for (int k = 0; k < i; k++)
            {
                s += l[j][k] * u[k][i];
            }
            l[j][i] = (a[j][i] - s) / u[i][i];      //按列计算l值
        }
    }

    for (int i = 0;i < N;i++)        //按行序，行内从高到低，计算l的逆矩阵
    {
        l_inv[i][i] = 1;
    }

    for (int i= 1;i < N;i++)
    {
        for (int j = 0;j < i;j++)
        {
            s = 0;
            for (int k = 0;k < i;k++)
            {
                s += l[i][k] * l_inv[k][j];
            }
            l_inv[i][j] = -s;
        }
    }

/*    printf("test l_inverse:\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            s = 0;
            for (int k = 0; k < N; k++)
            {
                s += l[i][k] * l_inv[k][j];
            }

            printf("%f ", s);
        }
        putchar('\n');
    }*/

    for (int i = 0;i < N;i++)                    //按列序，列内按照从下到上，计算u的逆矩阵
    {
        u_inv[i][i] = 1 / u[i][i];
    }
    for (int i = 1;i < N;i++)
    {
        for (int j = i - 1;j >=0;j--)
        {
            s = 0;
            for (int k = j + 1;k <= i;k++)
            {
                s += u[j][k] * u_inv[k][i];
            }
            u_inv[j][i] = -s / u[j][j];
        }
    }

/*    printf("test u_inverse:\n");
    for (int i = 0;i < N;i++)
    {
        for (int j = 0;j < N;j++)
        {
            s = 0;
            for (int k = 0;k < N;k++)
            {
                s += u[i][k] * u_inv[k][j];
            }

            printf("%f ",s);
        }
        putchar('\n');
    }*/

    for (int i = 0;i < N;i++)            //计算矩阵a的逆矩阵
    {
        for (int j = 0;j < N;j++)
        {
            for (int k = 0;k < N;k++)
            {
                a_inv[i][j] += u_inv[i][k] * l_inv[k][j];
            }
        }
    }

/*    printf("test a:\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            s = 0;
            for (int k = 0; k < N; k++)
            {
                s += a[i][k] * a_inv[k][j];
            }

            printf("%f ", s);
        }
        putchar('\n');
    }*/

    return a_inv;

}
/**
 *矩阵的逆
 */
std::vector<std::vector<double> > inv(std::vector<std::vector<double> > x){
    std::vector<std::vector<double> > res = A(x);

    double dets = det(x);
    // printf("\ndet(x) = %f\n", dets);
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

/**
 * 打印预测结果和结果之间的差异，以及预测部分的得分情况
 */
void print_predict_score (std::map<int, int> actual_data, std::map<int, int> predict_data) {
    float t1=0, t2=0, t3=0;
    for (auto &t: actual_data) {
        printf("flavor%2d %4d %4d\n", t.first, t.second, predict_data[t.first]);
        t1 += pow(t.second - predict_data[t.first], 2);
        t2 += pow(t.second, 2);
        t3 += pow(predict_data[t.first], 2);
    }
    int len = actual_data.size();
    float score = 1 - sqrt(t1 / len) / ( sqrt(t2 / len) + sqrt(t3 / len) );
    printf("predict score = %f\n", score);
}

/**
 * 差分函数
 * @param period
 * @return
 */
std::vector<double> do_diff(std::vector<double> data, int diff_day){
    if(diff_day>=data.size()-1){
        diff_day=0;
    }
    if(diff_day == 0){
        return data;
    }
    std::vector<double> res;
    for(int i=0;i<data.size()-diff_day;i++){
        double tmpData =data[i+diff_day]-data[i];
        res.push_back(tmpData);
    }
    return res;
}

/**
 *
 * @param before_diff_data 差分前的数据
 * @param after_diff_day 差分后的数据
 * @param diff_day 差分的天数
 * @param predict_data 根据差分结果预测出来的数据
 * @return
 */
std::vector<double> reset_diff(std::vector<double> before_diff_data, int diff_day, std::vector<double> predict_data){

    if (diff_day == 0) return predict_data;

    int before_diff_day =  before_diff_data.size();
    std::copy(predict_data.begin(), predict_data.end(), std::back_inserter(before_diff_data));
    for (int i=before_diff_day;i<before_diff_data.size();i++) {
        before_diff_data[i] += before_diff_data[i - diff_day];
    }
    return std::vector<double>(before_diff_data.begin() + before_diff_day, before_diff_data.end());
}


void get_scores_f(std::map<int, int>&predict_data, Server server, int number)
{
    int total_allocate;
    if (BasicInfo::is_cpu())
    {
        total_allocate = server.core * number;
    }
    else{
        total_allocate = server.mem *number;
    }
    int total_need = 0;
    for (int i = 1; i<=15; i++)
    {
        std::map<int ,int >::iterator iter;
        iter = predict_data.find(i);
        if (iter == predict_data.end())
        {
            continue;
        }
        else
        {
            int target_need = 0;
            std::map<int, Vm>::iterator current_flavor_info;
            current_flavor_info =  BasicInfo::vm_info.find(i);;
            if (BasicInfo::is_cpu())
            {
                target_need = current_flavor_info->second.core;
            }
            else
            {
                target_need = current_flavor_info->second.mem;
            }
            total_need += iter->second * target_need;
        }
    }
    double percent = (total_need+0.0)/total_allocate;
    printf("allocate score = %f\n", percent);
}

/**
 * 计算分配得分
 * @param bins
 */
double calc_alloc_score(std::vector<Bin> bins){
    double core_residual = 0.0;
    double mem_residual= 0.0;

    double core_sum = 0.0;
    double mem_sum = 0.0;
    for (Bin &bin: bins) {
        if (bin.objects.empty()) {
            continue;
        }
        core_residual += bin.cores;
        mem_residual += bin.mems;
        core_sum += BasicInfo::server_infos[bin.type].core;
        mem_sum += BasicInfo::server_infos[bin.type].mem;
    }
    double res = ((core_sum - core_residual) / core_sum + (mem_sum - mem_residual) / mem_sum ) / 2.0;
    return res;
}

