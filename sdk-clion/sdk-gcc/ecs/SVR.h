//
// Created by ruiy on 18-4-13.
//

#ifndef SDK_CLION_SVR_H
#define SDK_CLION_SVR_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <cfloat>
#include <cstring>
#include <cstdarg>
#include <climits>
#include "type_def.h"
#include "SVR_Q.h"

class SVR {
public:
    std::vector<std::vector<double>> X;
    std::vector<double> Y;

    svm_parameter param;

    svm_model model;

    SVR(std::vector<std::vector<double>> X, std::vector<double> Y, svm_parameter param);
    void train();
    std::pair<std::vector<double>, double> train_one();
    void solve_nu_svr(std::vector<double> &alpha, SolverRes &si);
    double predict(const std::vector<double> x);
    void Solve();
protected:
    int active_size;
    std::vector<char> y;
    std::vector<double> G;
    std::vector<char> alpha_status;	// LOWER_BOUND, UPPER_BOUND, FREE
    std::vector<double> alpha;
    SVR_Q *Q;
    const double *QD;
    double eps;
    double Cp,Cn;
    std::vector<double> p;
    std::vector<int> active_set;
    std::vector<double> G_bar;
    bool unshrink;

    inline double get_C(int i) { return (y[i] > 0)? Cp : Cn;}
    void update_alpha_status(int i);
    inline bool is_upper_bound(int i) { return alpha_status[i] == UPPER_BOUND; }
    inline bool is_lower_bound(int i) { return alpha_status[i] == LOWER_BOUND; }
    inline bool is_free(int i) { return alpha_status[i] == FREE; }
    void gradient(int l);
    int select_workset(int &i, int &j);
    double calc_rho();
    void shrink(int l);

    bool shrunk(int i, double Gmax1, double Gmax2, double Gmax3, double Gmax4);

private:
    SolverRes si;
};

#endif //SDK_CLION_SVR_H
