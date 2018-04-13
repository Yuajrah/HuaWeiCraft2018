//
// Created by ruiy on 18-4-13.
//

#ifndef SDK_CLION_SOLVER_H
#define SDK_CLION_SOLVER_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <locale.h>
#include <vector>

#include "SVR_Q.h"
class SVR_Q;

#define INF HUGE_VAL
#define TAU 1e-12


class Solver {
public:
    Solver() {};
    virtual ~Solver() {};

    struct SolutionInfo {
        double obj;
        double rho;
        double upper_bound_p;
        double upper_bound_n;
        double r;
    };

    void Solve(int l, SVR_Q& Q, const std::vector<double> &p_, const std::vector<char> &y_,
               std::vector<double> &alpha_, double Cp, double Cn, double eps,
               SolutionInfo &si, int shrinking);
protected:
    int active_size;
    std::vector<char> y;
    std::vector<double> G;
    enum { LOWER_BOUND, UPPER_BOUND, FREE };
    char *alpha_status;	// LOWER_BOUND, UPPER_BOUND, FREE
    std::vector<double> alpha;
    SVR_Q *Q;
    const double *QD;
    double eps;
    double Cp,Cn;
    std::vector<double> p;
    std::vector<int> active_set;
    std::vector<double> G_bar;
    int l;
    bool unshrink;

    double get_C(int i)
    {
        return (y[i] > 0)? Cp : Cn;
    }
    void update_alpha_status(int i)
    {
        if(alpha[i] >= get_C(i))
            alpha_status[i] = UPPER_BOUND;
        else if(alpha[i] <= 0)
            alpha_status[i] = LOWER_BOUND;
        else alpha_status[i] = FREE;
    }
    bool is_upper_bound(int i) { return alpha_status[i] == UPPER_BOUND; }
    bool is_lower_bound(int i) { return alpha_status[i] == LOWER_BOUND; }
    bool is_free(int i) { return alpha_status[i] == FREE; }
    void swap_index(int i, int j);
    void reconstruct_gradient();
    virtual int select_working_set(int &i, int &j);
    virtual double calculate_rho();

    virtual void do_shrinking();

    bool be_shrunk(int i, double Gmax1, double Gmax2);
    bool be_shrunk(int i, double Gmax1, double Gmax2, double Gmax3, double Gmax4);

private:
    SolutionInfo si;
};


#endif //SDK_CLION_SOLVER_H
