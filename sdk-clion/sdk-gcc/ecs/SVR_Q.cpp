//
// Created by ruiy on 18-4-13.
//

#include "SVR_Q.h"

SVR_Q::SVR_Q(const svm_problem& prob, const svm_parameter& param) {

    x = prob.x;
    l = prob.l;

    cache = new Cache(l,(long int)(param.cache_size*(1<<20)));

    QD = new double[2*l];
    sign = new char[2*l];
    index = new int[2*l];

    for(int k=0;k<l;k++) {
        sign[k] = 1;
        sign[k+l] = -1;
        index[k] = k;
        index[k+l] = k;
        QD[k] = kernel_linear(k,k);
        QD[k+l] = QD[k];
    }
    buffer[0] = new float[2*l];
    buffer[1] = new float[2*l];
    next_buffer = 0;
}


void SVR_Q::swap_index(int i, int j) const
{
    std::swap(sign[i],sign[j]);
    std::swap(index[i],index[j]);
    std::swap(QD[i],QD[j]);
}

float*  SVR_Q::get_Q(int i, int len)
{
    float *data;
    int j, real_i = index[i];
    if(cache->get_data(real_i,&data,l) < l)
    {
        for(j=0;j<l;j++)
            data[j] = (float)kernel_linear(real_i,j);
    }

    // reorder and copy
    float *buf = buffer[next_buffer];
    next_buffer = 1 - next_buffer;
    char si = sign[i];
    for(j=0;j<len;j++)
        buf[j] = (float) si * (float) sign[j] * data[index[j]];
    return buf;
}


double * SVR_Q::get_QD() const
{
    return QD;
}

double SVR_Q::kernel_linear(int i, int j)
{
    return dot(x[i],x[j]);
}

double SVR_Q::dot(const std::vector<double> px, const std::vector<double> py)
{
    double sum = 0;
    int i = 0;
    while(i < px.size()) {
        sum += px[i] * py[i];
        i++;
    }
    return sum;
}

SVR_Q::~SVR_Q()
{
    delete cache;
    delete[] sign;
    delete[] index;
    delete[] buffer[0];
    delete[] buffer[1];
    delete[] QD;
}
