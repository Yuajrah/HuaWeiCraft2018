//
// Created by ruiy on 18-3-18.
//

#include "ma.h"
#include <cstdio>

std::vector<double> ma(std::vector<double> data, int window_size, int move_size){
    std::vector<double> res;
    for (int i=0;i<data.size()-window_size+move_size;i=i+move_size) {
        double sum = 0.0;
        for (int j=i;j<i+window_size;j++) {
            sum += data[j];
        }
        res.push_back(sum / (window_size + 0.0));
    }
    return res;
}