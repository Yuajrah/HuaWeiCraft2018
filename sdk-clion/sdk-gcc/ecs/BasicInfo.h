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
};


#endif //SDK_CLION_BASICINFO_H
