//
// Created by ruiy on 18-3-18.
//

#include "ma.h"
#include <cstdio>

std::vector<Double> ma(std::vector<Double> data, int window_size, int move_size){
    std::vector<Double> res;
    for (int i=0;i<data.size()-window_size+1;i=i+move_size) {
        Double sum = 0.0;
        for (int j=i;j<i+window_size;j++) {
            sum += data[j];
        }
        res.push_back(sum / (window_size + 0.0));
    }
    return res;
}