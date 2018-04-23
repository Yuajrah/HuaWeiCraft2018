//
// Created by ruiy on 18-4-23.
//

#include "gm.h"
#include "math_utils.h"
#include <cmath>

std::vector<double> pre_precess(std::vector<double> input){
    double sum = 0;
    int n = input.size();
    std::vector<double> output(n, 0.0);
    for (int i=0;i<input.size();i++) {
        sum += input[i];
        output[i] = sum;
    }
    return output;
}

std::vector<std::vector<double>> gen_b_mat(std::vector<double> pre_processed){
    int n = pre_processed.size();
    std::vector<std::vector<double>> mat(n-1, std::vector<double>(2, 0.0));
    for (int i=0;i<n-1;i++) {
        mat[i][0] = -(pre_processed[i] + pre_processed[i+1]) / 2.0;
        mat[i][1] = 1.0;
    }
    return mat;
}

std::vector<std::vector<double>> gen_y_mat(std::vector<double> origin){
    int n = origin.size();
    std::vector<std::vector<double>> output(n-1, std::vector<double>(1, 0.0));
    for (int i=0;i<n-1;i++) {
        output[i][0] = origin[i+1];
    }
    return output;
}
std::vector<double> estau(std::vector<std::vector<double>> mat_b, std::vector<std::vector<double>> mat_y){
    std::vector<std::vector<double>> mat_b_transpose = t(mat_b);
    std::vector<std::vector<double>> c = mulMat(mat_b_transpose, mat_b);
    std::vector<std::vector<double>> c1 = gauss(c);
    std::vector<std::vector<double>> d = mulMat(c1, mat_b_transpose);
    std::vector<std::vector<double>> au = mulMat(d, mat_y);
    std::vector<double> au_vec;
    for (int i=0;i<au.size();i++) {
        au_vec.push_back(au[i][0]);
    }
    return au_vec;
}

std::vector<std::vector<double>> gauss(std::vector<std::vector<double>> A)
{
    int n = A.size();



    double max, temp;
    std::vector<std::vector<double>> t(n, std::vector<double>(n, 0.0));
    std::vector<std::vector<double>> B(n, std::vector<double>(n, 0.0));


    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            t[i][j] = A[i][j];
        }
    }

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            B[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    for (int i = 0; i < n; i++){
        max = t[i][i];
        int k = i;
        for (int j = i + 1; j < n; j++){
            if (fabs(t[j][i]) > fabs(max)){
                max = t[j][i];
                k = j;
            }
        }
        if (k != i){
            for (int j = 0; j < n; j++){
                temp = t[i][j];
                t[i][j] = t[k][j];
                t[k][j] = temp;
                temp = B[i][j];
                B[i][j] = B[k][j];
                B[k][j] = temp;
            }
        }
        if (t[i][i] == 0){
            printf("There is no inverse matrix!\n");
            return {};
        }
        temp = t[i][i];
        for (int j = 0; j < n; j++){
            t[i][j] = t[i][j] / temp;
            B[i][j] = B[i][j] / temp;
        }
        for (int j = 0; j < n; j++){
            if (j != i){
                temp = t[j][i];
                for (k = 0; k < n; k++){
                    t[j][k] = t[j][k] - t[i][k] * temp;
                    B[j][k] = B[j][k] - B[i][k] * temp;
                }
            }
        }
    }
    return B;
}

double predict_x1(double a, double u, double x0_0, int k){
    double res = (x0_0 - u / a) * exp(-a * k) + u / a;
    return res;
}