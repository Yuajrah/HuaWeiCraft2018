#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <locale.h>
#include "svm.h"
#include "Solver.h"

#define INF HUGE_VAL
#define TAU 1e-12



static void solve_nu_svr(
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

//
// decision_function
//
struct decision_function
{
	std::vector<double> alpha;
	double rho;
};

static decision_function svm_train_one(svm_problem prob, svm_parameter param, double Cp, double Cn)
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


	decision_function f;
	f.alpha = alpha;
	f.rho = si.rho;
	return f;
}


// Return parameter of a Laplace distribution
static double svm_svr_probability(
		const svm_problem prob, const svm_parameter param)
{
	int nr_fold = 5;
	std::vector<double> ymv(prob.l);
	double mae = 0;

	svm_parameter newparam(param);
	newparam.probability = 0;
	svm_cross_validation(prob, newparam, nr_fold, ymv);

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
// Interface functions
//
svm_model svm_train(const svm_problem &prob, const svm_parameter &param)
{
	svm_model model;
	model.param = param;
	model.free_sv = 0;
	model.nr_class = 2;
    model.sv_coef = std::vector<std::vector<double>>(1);

    if(param.probability && param.svm_type == NU_SVR)
    {
        model.probA = std::vector<double>(1, svm_svr_probability(prob, param));
    }

    decision_function f = svm_train_one(prob, param,0,0);
    model.rho = std::vector<double>(1, f.rho);

    int nSV = 0;
    for(int i=0;i<prob.l;i++)
        if(fabs(f.alpha[i]) > 0) nSV++;

    model.l = nSV;
	model.SV = std::vector<std::vector<svm_node>>(nSV);
    model.sv_coef[0] = std::vector<double>(nSV, 0.0);
    model.sv_indices = std::vector<int>(nSV, 0);

    int j = 0;
    for(int i=0;i<prob.l;i++)
        if(fabs(f.alpha[i]) > 0)
        {
            model.SV[j] = prob.x[i];
            model.sv_coef[0][j] = f.alpha[i];
            model.sv_indices[j] = i+1;
            j++;
        }

	return model;
}

// Stratified cross validation
void svm_cross_validation(svm_problem prob, svm_parameter param, int nr_fold, std::vector<double> target)
{
	int i;
	int l = prob.l;

	std::vector<int> perm(l);

	nr_fold = std::max(nr_fold, l);
	std::vector<int> fold_start(nr_fold+1);

	// stratified cv may not give leave-one-out rate
	// Each class to l folds -> some folds may have zero elements

	for(i=0;i<l;i++) perm[i]=i;
	for(i=0;i<l;i++) {
		int j = i+rand()%(l-i);
		std::swap(perm[i],perm[j]);
	}
	for(i=0;i<=nr_fold;i++)
		fold_start[i]=i*l/nr_fold;

	for(i=0;i<nr_fold;i++)
	{
		int begin = fold_start[i];
		int end = fold_start[i+1];
		svm_problem subprob;

		subprob.l = l-(end-begin);
		subprob.x = std::vector<std::vector<svm_node>>(subprob.l);
        subprob.y = std::vector<double>(subprob.l);

		int k=0;
		for(int j=0;j<begin;j++)
		{
			subprob.x[k] = prob.x[perm[j]];
			subprob.y[k] = prob.y[perm[j]];
			k++;
		}
		for(int j=end;j<l;j++)
		{
			subprob.x[k] = prob.x[perm[j]];
			subprob.y[k] = prob.y[perm[j]];
			k++;
		}
		svm_model submodel = svm_train(subprob, param);

		for(int j=begin;j<end;j++)
			target[perm[j]] = svm_predict(submodel,prob.x[perm[j]]);
	}
}


double svm_predict_values(const svm_model &model, const std::vector<svm_node> x, std::vector<double> &dec_values)
{
	std::vector<double> sv_coef = model.sv_coef[0];
	double sum = 0;

	for(int i=0;i<model.l;i++)
		sum += sv_coef[i] * SVR_Q::k_function(x, model.SV[i],model.param);

	sum -= model.rho[0];
	dec_values[0] = sum;

	return sum;

}

double svm_predict(const svm_model &model, const std::vector<svm_node> x)
{
    std::vector<double> dec_values;

    dec_values.assign(1, 0.0);

	double pred_result = svm_predict_values(model, x, dec_values);

	return pred_result;
}
