//
// Created by ruiy on 18-4-13.
//

#include "SVR.h"


SVR::SVR(svm_problem prob, svm_parameter param): prob(prob), param(param){}

void SVR::train() {
    svm_model model;
    model.param = param;
    model.free_sv = 0;
    model.nr_class = 2;
    model.sv_coef = std::vector<std::vector<double>>(1);

    if(param.probability)
    {
        model.probA = std::vector<double>(1, svr_probability(prob, param));
    }

    std::pair<std::vector<double>, double> alpha_rho = train_one(prob, param,0,0);
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

    this->model = model;
}

// Return parameter of a Laplace distribution
double SVR::svr_probability(
        const svm_problem prob, const svm_parameter param)
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

//
// decision_function
//
//struct decision_function
//{
//	std::vector<double> alpha;
//	double rho;
//};

std::pair<std::vector<double>, double> SVR::train_one(svm_problem prob, svm_parameter param, double Cp, double Cn)
{
//	double *alpha = Malloc(double,prob.l);
    std::vector<double> alpha(prob.l, 0.0);
    Solver::SolutionInfo si;

    solve_nu_svr(prob, param, alpha, si);


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

void SVR::solve_nu_svr(
        const svm_problem &prob,
        const svm_parameter &param,
        std::vector<double> &alpha,
        Solver::SolutionInfo &si)
{
    int l = prob.l;
    double C = param.C;
    std::vector<double> alpha2(2*l);
    std::vector<double> linear_term(2*l);
    std::vector<char> y(2*l);

    double sum = C * param.nu * l / 2;
    for(int i=0;i<l;i++)
    {
        alpha2[i] = alpha2[i+l] = std::min(sum,C);
        sum -= alpha2[i];

        linear_term[i] = - prob.y[i];
        y[i] = 1;

        linear_term[i+l] = prob.y[i];
        y[i+l] = -1;
    }

    Solver s;
    SVR_Q svr_q(prob, param);
    s.Solve(2*l, svr_q, linear_term, y, alpha2, C, C, param.eps, si, param.shrinking);

    for(int i=0;i<l;i++)
        alpha[i] = alpha2[i] - alpha2[i+l];
}


double SVR::predict(const std::vector<svm_node> x)
{
    svm_model model = this->model;
    double pred_result = -model.rho[0];

    for(int i=0;i<model.l;i++)
        pred_result += model.sv_coef[0][i] * SVR_Q::dot(x, model.SV[i]);

    return pred_result;
}
