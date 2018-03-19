//
// Created by ruiy on 18-3-16.
//

#ifndef SDK_GCC_GET_DATA_H
#define SDK_GCC_GET_DATA_H
#include <vector>
#include <map>
#include "type_def.h"

std::map<int, std::vector<Double>> get_esc_data(char ** data,
                                                char *start_date,
                                                char *end_date /*不包含该天*/,
                                                std::map<int, Vm> vm_info,
                                                int data_num);

std::map<int, int> get_sum_data(
        char **data, // 直接将data数组传入即可
        char *start_date, // 所需要获取集合的开始的日期
        char *end_date , // 所需要获取集合的结束的日期（不包含该日期）
        std::map<int, Vm> vm_info, // 所有vm的基本信息
        int data_num // data的大小，即esc文本有多少行
);
#endif //SDK_GCC_GET_DATA_H
