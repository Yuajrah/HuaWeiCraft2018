//
// Created by ruiy on 18-3-16.
//

#include "get_data.h"
#include "date_utils.h"

std::map<int, std::vector<Double>> get_esc_data(
        char **data,
        char *start_date,
        char *end_date /*不包含该天*/,
        std::map<int, Vm> vm_info,
        int data_num){
    /*
  * 处理历史数据
  */
    std::map<int, std::vector<Double>> res;

    // 初始化，给每种vm分配数据存储空间（连续天）
    int periods_len = get_days(start_date, end_date);  // 历史数据长度
    /*printf("%d\n", periods_len);
    printf("period: %s - %s", start_date, end_date);*/
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
