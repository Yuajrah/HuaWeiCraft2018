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
    static bool is_cpu(){return *opt_object == 'C';}
    static bool is_mem(){return *opt_object == 'M';}
};


#endif //SDK_CLION_BASICINFO_H
