//
// Created by ruiy on 18-4-13.
//

#ifndef SDK_CLION_SVR_H
#define SDK_CLION_SVR_H

#include <cmath>
#include "Solver.h"

class SVR {
public:
    svm_problem prob;
    svm_parameter param;

    svm_model model;

    SVR(svm_problem prob, svm_parameter param);
    void train();
    double svr_probability();
    std::pair<std::vector<double>, double> train_one(double Cp, double Cn);
    void solve_nu_svr(std::vector<double> &alpha,Solver::SolutionInfo &si);

    double predict(const std::vector<svm_node> x);
};


#endif //SDK_CLION_SVR_H
