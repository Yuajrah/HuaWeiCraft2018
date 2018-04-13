//
// Created by ruiy on 18-4-13.
//

#include "SVR.h"


SVR::SVR(svm_problem prob, svm_parameter param): prob(prob), param(param){}

void SVR::train() {

    model.param = param;
    model.free_sv = 0;
    model.nr_class = 2;
    model.sv_coef = std::vector<std::vector<double>>(1);

    if(param.probability)
    {
        model.probA = std::vector<double>(1, svr_probability());
    }

    std::pair<std::vector<double>, double> alpha_rho = train_one(0, 0);
    model.rho = std::vector<double>(1, alpha_rho.second);

    int nSV = 0;
    for(int i=0;i<prob.l;i++)
        if(fabs(alpha_rho.first[i]) > 0) nSV++;

    model.l = nSV;
    model.SV = std::vector<std::vector<svm_node>>(nSV);
    model.sv_coef[0] = std::vector<double>(nSV, 0.0);
    model.sv_indices = std::vector<int>(nSV, 0);

    int j = 0;
    for(int i=0;i<prob.l;i++)
        if(fabs(alpha_rho.first[i]) > 0)
        {
            model.SV[j] = prob.x[i];
            model.sv_coef[0][j] = alpha_rho.first[i];
            model.sv_indices[j] = i+1;
            j++;
        }

}

// Return parameter of a Laplace distribution
double SVR::svr_probability()
{
    std::vector<double> ymv(prob.l);
    double mae = 0;


    for(int i=0;i<prob.l;i++)
    {
        ymv[i]=prob.y[i]-ymv[i];
        mae += fabs(ymv[i]);
    }

    mae /= prob.l;
    double std=sqrt(2*mae*mae);
    int count=0;

    mae=0;
    for(int i=0;i<prob.l;i++)
        if (fabs(ymv[i]) > 5*std)
            count=count+1;
        else
            mae+=fabs(ymv[i]);

    mae /= (prob.l-count);
    return mae;
}


std::pair<std::vector<double>, double> SVR::train_one(double Cp, double Cn)
{
    std::vector<double> alpha(prob.l, 0.0);

    Solver::SolutionInfo si;
    solve_nu_svr(alpha, si);

    // output SVs

    int nSV = 0;
    int nBSV = 0;
    for(int i=0;i<prob.l;i++)
    {
        if(fabs(alpha[i]) > 0)
        {
            ++nSV;
            if(prob.y[i] > 0)
            {
                if(fabs(alpha[i]) >= si.upper_bound_p)
                    ++nBSV;
            }
            else
            {
                if(fabs(alpha[i]) >= si.upper_bound_n)
                    ++nBSV;
            }
        }
    }

    return {alpha, si.rho};
}

void SVR::solve_nu_svr(std::vector<double> &alpha, Solver::SolutionInfo &si) {

    std::vector<double> t_alpha(2 * prob.l);
    std::vector<double> linear_term(2 * prob.l);
    std::vector<char> y(2 * prob.l);

    double sum = param.C * param.nu * prob.l / 2;

    for(int i=0;i<prob.l;i++) {
        t_alpha[i] = t_alpha[i+prob.l] = std::min(param.C, sum);
        sum -= t_alpha[i];

        linear_term[i] = - prob.y[i];
        y[i] = 1;

        linear_term[i+prob.l] = prob.y[i];
        y[i+prob.l] = -1;
    }

    Solver s;
    SVR_Q svr_q(prob, param);
    s.Solve(2 * prob.l, svr_q, linear_term, y, t_alpha, param.C, param.C, param.eps, si, param.shrinking);

    for(int i=0;i<prob.l;i++)
        alpha[i] = t_alpha[i] - t_alpha[i+prob.l];
}


double SVR::predict(const std::vector<svm_node> x)
{
    svm_model model = this->model;
    double pred_result = -model.rho[0];

    for(int i=0;i<model.l;i++)
        pred_result += model.sv_coef[0][i] * SVR_Q::dot(x, model.SV[i]);

    return pred_result;
}
