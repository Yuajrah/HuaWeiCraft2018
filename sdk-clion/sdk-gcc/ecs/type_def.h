//
// Created by ruiy on 18-3-16.
//

#ifndef SDK_GCC_TYPE_DEF_H
#define SDK_GCC_TYPE_DEF_H

#define INF HUGE_VAL
#define TAU 1e-12

#include <vector>

struct Server {
    char* type;
    int core;// 核数
    int mem; // 内存大小
    int disk; // 硬盘大小
};

struct Vm {
    int type; // 标明类型, 比如1代表flavor1, 为了方便处理
    int core; // 所需核数
    int mem; // 所需内存大小
    int no; // 当改类型用做物体处理时, 作为编号, 其他情况不作处理
    double size;//排序时虚拟机的size
};

struct Allocat_server
{
    int id; //标号
    int core;
    int mem;
    int target; //需要比较的东西，1为核，2为内存
};

bool operator< (const Allocat_server &server1, const Allocat_server &server2);

struct Node_index
{
    double value;
    int id;
    double target;
};

bool operator< (const Node_index &index1, const Node_index &index);

/**
 * svm 参数
 */
struct SvmParam
{
    double eps;
    double C;
    double nu;
};

enum { STATUS_LOWER_BOUND, STATUS_UPPER_BOUND, STATUS_FREE };

#endif //SDK_GCC_TYPE_DEF_H
