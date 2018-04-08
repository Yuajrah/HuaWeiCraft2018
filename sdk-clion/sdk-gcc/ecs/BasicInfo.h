//
// Created by ruiy on 18-4-1.
//

#ifndef SDK_CLION_BASICINFO_H
#define SDK_CLION_BASICINFO_H

#include "type_def.h"
#include <map>

class BasicInfo {
public:
    static std::map<int, Vm> vm_info;
    static Server server_info;
    static char* opt_object;
    static time_t t_start;
    static int need_predict_day;
    static bool is_cpu(){return *opt_object == 'C';}
    static bool is_mem(){return *opt_object == 'M';}
    static bool is_stop(){
        time_t t_end = time(NULL);
        if (t_end - t_start > 54) {return true;}
        return false;
    }
};


#endif //SDK_CLION_BASICINFO_H
