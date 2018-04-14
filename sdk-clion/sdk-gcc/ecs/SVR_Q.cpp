//
// Created by ruiy on 18-4-13.
//

#include "SVR_Q.h"

SVR_Q::SVR_Q(std::vector<std::vector<double>> X, std::vector<double> Y, const svm_parameter& param) {

    this->X = X;
    l = Y.size();

//    buf = new Buf(l,(long int)(param.cache_size*(1<<20)));

    QD = new double[2*l];
    sign = std::vector<char>(2*l);
    index = std::vector<int>(2*l);

    for(int k=0;k<l;k++) {
        sign[k] = 1;
        sign[k+l] = -1;
        index[k] = k;
        index[k+l] = k;
        QD[k] = kernel_linear(k,k);
        QD[k+l] = QD[k];
    }

    buffer.push_back(std::vector<float>(2*l));
    buffer.push_back(std::vector<float>(2*l));

    next_buffer = 0;
}


void SVR_Q::swap_index(int i, int j)
{
    std::swap(sign[i],sign[j]);
    std::swap(index[i],index[j]);
    std::swap(QD[i],QD[j]);
}

std::vector<float> SVR_Q::get_Q(int i, int len)
{
    float *data = new float[l];
    int j, real_i = index[i];

    for(j=0;j<l;j++) data[j] = (float)kernel_linear(real_i,j);

    std::vector<float> buf = buffer[next_buffer];

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
    return dot(X[i],X[j]);
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

SVR_Q::~SVR_Q() {
//    delete[] sign;
//    delete[] index;
    delete[] QD;
}
