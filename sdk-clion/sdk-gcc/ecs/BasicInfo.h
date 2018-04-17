//
// Created by ruiy on 18-4-1.
//

#ifndef SDK_CLION_BASICINFO_H
#define SDK_CLION_BASICINFO_H

#include "type_def.h"
#include <map>

class BasicInfo {
public:
    static std::map<int, Vm> vm_info; // 需要预测的vm基本信息
    static std::map<char*, Server> server_infos; // 不同类型server的基本信息
    static time_t t_start;

    static bool is_stop(){
        time_t t_end = time(NULL);
        if (t_end - t_start > 54) {return true;}
        return false;
    }
    static int split_hour; // 以hour_split小时为统计单元, hour_split最好为24*数据天数的因子, 否则最后不足的数据将直接取往前一次的数据代替

    static int need_predict_day; // 需要预测的天数
    static int need_predict_cnt; // 需要预测的次数, 当粒度为天时, 同上(need_predict_day)

    static int extra_need_predict_day; // 需要额外预测的天数
    static int extra_need_predict_cnt; // 需要额外预测的次数, 当粒度为天时, 同上(extra_need_predict_day)


    /**
     * no usage
     */
    static Server server_info;
    static char* opt_object;
    static bool is_cpu(){return *opt_object == 'C';}
    static bool is_mem(){return *opt_object == 'M';}
};


#endif //SDK_CLION_BASICINFO_H
