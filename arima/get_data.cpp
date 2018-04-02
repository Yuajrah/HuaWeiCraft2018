//
// Created by ruiy on 18-3-16.
//

#include "get_data.h"
#include "date_utils.h"

/**
 * 获取[start_date, end_date)范围内的数据
 * 得到的数据格式为：
 * {
 *      1: {core: xx, storage: xx}, // 1代表flavor1
 *      2: {core: xx, storage: xx}  // 2代表flavor2
 *      ...
 * }
 */
std::map<int, std::vector<Double>> get_esc_data(
        char **data, // 直接将data数组传入即可
        char *start_date, // 所需要获取集合的开始的日期
        char *end_date , // 所需要获取集合的结束的日期（不包含该日期）
        std::map<int, Vm> vm_info, // 所有vm的基本信息
        int data_num // data的大小，即esc文本有多少行
){
    std::map<int, std::vector<Double>> res;

    // 初始化，给每种vm分配数据存储空间（连续天）
    int periods_len = get_days(start_date, end_date);  // 数据长度

    for (auto &info: vm_info) {
        res[info.first].assign(periods_len, (Double)0);
    }

    for (int i=0;i<data_num;i++) {
        int type;char date[11];
        sscanf(data[i], "%*s flavor%d %s",&type, &date);
        int day_index = get_days(start_date, date);
        if (day_index >= 0 && get_days(date, end_date) > 0 ) { // 如果 start_date <= date && date < end_date，则是需要统计period内的数据
            std::map<int, std::vector<Double>>::iterator iter = res.find(type);
            if (iter != res.end()) {
                iter->second[day_index]++;
            }
        }
    }
    return res;
};

/**
 * 获取[start_date, end_date)范围内的各个vm的总个数
 * 得到的数据格式为：
 * {
 *      1: 3, // 1代表flavor1
 *      2: 2  // 2代表flavor2
 *      ...
 * }
 */
std::map<int, int> get_sum_data(
        char **data, // 直接将data数组传入即可
        char *start_date, // 所需要获取集合的开始的日期
        char *end_date , // 所需要获取集合的结束的日期（不包含该日期）
        std::map<int, Vm> vm_info, // 所有vm的基本信息
        int data_num // data的大小，即esc文本有多少行
){
    std::map<int, int> res;

    // 初始化，给每种vm初始化
    for (auto &info: vm_info) res[info.first] = 0;

    for (int i=0;i<data_num;i++) {
        int type;char date[11];
        sscanf(data[i], "%*s flavor%d %s",&type, &date);
        int day_index = get_days(start_date, date);
        if (day_index >= 0 && get_days(date, end_date) > 0 ) { // 如果 start_date <= date && date < end_date，则是需要统计period内的数据
            std::map<int, int>::iterator iter = res.find(type);
            if (iter != res.end()) {
                iter->second++;
            }
        }
    }
    return res;
};