//
// Created by ruiy on 18-3-16.
//

#ifndef SDK_GCC_GET_DATA_H
#define SDK_GCC_GET_DATA_H
#include <vector>
#include <map>
#include "type_def.h"

std::map<int, std::vector<double>> get_esc_data(char ** data,
                                                char *start_date,
                                                char *end_date /*不包含该天*/,
                                                std::map<int, Vm> vm_info,
                                                int data_num);

#endif //SDK_GCC_GET_DATA_H
