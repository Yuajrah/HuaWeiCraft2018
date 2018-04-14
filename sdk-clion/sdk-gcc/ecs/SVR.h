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

class SVR {
public:
    std::vector<std::vector<double>> X;
    std::vector<double> Y;

    svm_parameter param;

    svm_model model;

    SVR(std::vector<std::vector<double>> X, std::vector<double> Y, svm_parameter param);
    ~SVR();
    void train();
    std::pair<std::vector<double>, double> train_one();
    double predict(const std::vector<double> x);
protected:
    int active_size;
    std::vector<char> y;
    std::vector<double> G;
    std::vector<char> alpha_status;	// LOWER_BOUND, UPPER_BOUND, FREE
    std::vector<double> alpha;
//    SVR_Q Q;
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

private:

    // svm_parameter
    int l;
    std::vector<char> sign;
    std::vector<int> index;
    mutable int next_buffer;
    std::vector<std::vector<float>> buffer;
    double *QD;


public:
//    SVR_Q(std::vector<std::vector<double>> X, std::vector<double> Y, const svm_parameter& param);
//    void swap_index(int i, int j);

    std::vector<float> get_Q(int i, int len);

    double *get_QD() const;

    double kernel_linear(int i, int j);

    static double dot(const std::vector<double> px, const std::vector<double> py);
};

#endif //SDK_CLION_SVR_H
