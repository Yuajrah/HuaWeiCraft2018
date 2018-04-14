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

    SvmParam param;

    SVR(std::vector<std::vector<double>> X, std::vector<double> Y, SvmParam param);

    int model_l;
    std::vector<std::vector<double>> sv;
    std::vector<double> sv_coef;
    double rho;
    std::vector<int> sv_indices;

    double obj;
    double r;

    int lives_size;
    std::vector<char> y;
    std::vector<double> g;
    std::vector<char> status;
    std::vector<double> a;
    double eps;
    std::vector<double> p;
    std::vector<int> lives;
    std::vector<double> g_bar;
    bool not_constricted;
    std::vector<char> sign;
    std::vector<int> index;
    mutable int next_buffer;
    std::vector<std::vector<float>> buffer;
    std::vector<double> qd;

public:
    void update_a_status(int i);

    void calc_gradient(int l);
    int calc_ws(int &i, int &j);
    double calc_rho();
    std::vector<double> calc_a();
    std::vector<float> calc_q(int i, int len);

    void constriction(int l);
    bool constricted(int i, double g_max1, double g_max2, double g_max3, double g_max4);

    void train();
    double predict(const std::vector<double> x);

    inline bool is_upper_bound(int i) { return status[i] == STATUS_UPPER_BOUND; }
    inline bool is_lower_bound(int i) { return status[i] == STATUS_LOWER_BOUND; }
    inline bool is_free(int i) { return status[i] == STATUS_FREE; }
    inline std::vector<double> calc_qd() const {return qd;}
    inline double kernel_linear(int i, int j) {return dot(X[i],X[j]);}
    static double dot(const std::vector<double> px, const std::vector<double> py);

};

#endif //SDK_CLION_SVR_H
