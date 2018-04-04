//
// Created by ruiy on 18-3-16.
//

#ifndef SDK_GCC_TYPE_DEF_H
#define SDK_GCC_TYPE_DEF_H

struct Server {
    int core;// 核数
    int storage; // 内存大小
    int disk; // 硬盘大小
};

struct Vm {
    int core; // 所需核数
    int storage; // 所需内存大小
};

struct Allocat_server
{
    int id; //标号
    int core;
    int storage;
    int target; //需要比较的东西，1为核，2为内存
};

typedef double Double;

bool operator< (const Allocat_server &server1, const Allocat_server &server2);

#endif //SDK_GCC_TYPE_DEF_H
