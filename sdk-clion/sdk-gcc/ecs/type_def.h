//
// Created by ruiy on 18-3-16.
//

#ifndef SDK_GCC_TYPE_DEF_H
#define SDK_GCC_TYPE_DEF_H

#include <vector>

struct Server {
    int core;// 核数
    int mem; // 内存大小
    int disk; // 硬盘大小
};

struct Vm {
    int type; // 标明类型, 比如1代表flavor1, 为了方便处理
    int core; // 所需核数
    int mem; // 所需内存大小
    int no; // 当改类型用做物体处理时, 作为编号, 其他情况不作处理
    double size;//排序时虚拟机的size
};

struct Allocat_server
{
    int id; //标号
    int core;
    int mem;
    int target; //需要比较的东西，1为核，2为内存
};

bool operator< (const Allocat_server &server1, const Allocat_server &server2);

struct Node_index
{
    double value;
    int id;
    double target;
};

bool operator< (const Node_index &index1, const Node_index &index);


/**
 * svm type
 */

//
// decision_function
//

struct decision_function
{
    std::vector<double> alpha;
    double rho;
};


struct svm_node
{
    int index;
    double value;
};

struct svm_problem
{
    int l;
    std::vector<double> y;
//    struct svm_node **x;
    std::vector<std::vector<svm_node>> x;
};

enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR };	/* svm_type */
enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED }; /* kernel_type */

struct svm_parameter
{
    int svm_type;
    int kernel_type;
    int degree;	/* for poly */
    double gamma;	/* for poly/rbf/sigmoid */
    double coef0;	/* for poly/sigmoid */

    /* these are for training only */
    double cache_size; /* in MB */
    double eps;	/* stopping criteria */
    double C;	/* for C_SVC, EPSILON_SVR and NU_SVR */
    int nr_weight;		/* for C_SVC */
    int *weight_label;	/* for C_SVC */
    double* weight;		/* for C_SVC */
    double nu;	/* for NU_SVC, ONE_CLASS, and NU_SVR */
    double p;	/* for EPSILON_SVR */
    int shrinking;	/* use the shrinking heuristics */
    int probability; /* do probability estimates */
};

//
// svm_model
//
struct svm_model
{
    svm_parameter param;	/* parameter */
    int nr_class;		/* number of classes, = 2 in regression/one class svm */
    int l;			/* total #SV */
//    struct svm_node **SV;		/* SVs (SV[l]) */
    std::vector<std::vector<svm_node>> SV;		/* SVs (SV[l]) */
    std::vector<std::vector<double>> sv_coef;	/* coefficients for SVs in decision functions (sv_coef[k-1][l]) */
    std::vector<double> rho;		/* constants in decision functions (rho[k*(k-1)/2]) */
    std::vector<double> probA;		/* pariwise probability information */
    std::vector<double> probB;
    std::vector<int> sv_indices;        /* sv_indices[0,...,nSV-1] are values in [1,...,num_traning_data] to indicate SVs in the training set */

    /* for classification only */

    std::vector<int> label;		/* label of each class (label[k]) */
    std::vector<int> nSV;		/* number of SVs for each class (nSV[k]) */
    /* nSV[0] + nSV[1] + ... + nSV[k-1] = l */
    /* XXX */
    int free_sv;		/* 1 if svm_model is created by svm_load_model*/
    /* 0 if svm_model is created by svm_train */
};


#endif //SDK_GCC_TYPE_DEF_H
