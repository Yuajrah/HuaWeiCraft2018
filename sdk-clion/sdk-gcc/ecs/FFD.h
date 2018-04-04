//
// Created by root on 18-3-31.
//

#include "type_def.h"
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <cstring>
#include "Bin.h"
#include "BasicInfo.h"
class FFD{

public:
    //进行分配函数
    std::vector<Bin> FFD_Dot ();

    FFD(std::map<int, Vm> vm_info, int type, std::map<int, int> predict_data);


private:
    //权重系数数组
    std::vector<double> paramA;
    int total_vm_num = 0;
    std::vector<Bin> result_Bin;
    std::map<int, Vm> vminfo;
    std::map<int, int> predict_data;
    int order_type;

    void getA ();
    void get_vm_size_dot();
    void get_vm_size_L2();
    //获取最大size的虚拟机序号
    int get_max_vm();

};

