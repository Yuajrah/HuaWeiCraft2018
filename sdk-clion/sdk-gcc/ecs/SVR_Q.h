//
// Created by ruiy on 18-4-13.
//

#ifndef SDK_CLION_SVR_Q_H
#define SDK_CLION_SVR_Q_H

#define INF HUGE_VAL
#define TAU 1e-12

#include "type_def.h"

#include "Cache.h"
class Cache;

class SVR_Q
{
private:
    std::vector<std::vector<svm_node>> x;

    // svm_parameter
    int kernel_type;


    int l;
    Cache *cache;
    char *sign;
    int *index;
    mutable int next_buffer;
    float *buffer[2];
    double *QD;

public:
    SVR_Q(const svm_problem& prob, const svm_parameter& param);
    void swap_index(int i, int j) const;

    float *get_Q(int i, int len);

    double *get_QD() const;

    double kernel_linear(int i, int j);

    static double dot(const std::vector<svm_node> px, const std::vector<svm_node> py);

    ~SVR_Q();
};

#endif //SDK_CLION_SVR_Q_H
