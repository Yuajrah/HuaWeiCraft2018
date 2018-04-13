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
    double svr_probability(const svm_problem prob, const svm_parameter param);
    static decision_function train_one(svm_problem prob, svm_parameter param, double Cp, double Cn);
    static void solve_nu_svr(const svm_problem &prob,const svm_parameter &param,std::vector<double> &alpha,Solver::SolutionInfo &si);

    double predict(const std::vector<svm_node> x);
    void cross_validation(svm_problem prob, svm_parameter param, int nr_fold, std::vector<double> target);
    double predict_values(const svm_model &model, const std::vector<svm_node> x, std::vector<double> &dec_values);
};


#endif //SDK_CLION_SVR_H
