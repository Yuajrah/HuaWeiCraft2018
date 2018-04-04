//
// Created by root on 18-3-31.
//

#include "FFD.h"
#include "math_utils.h"

FFD::FFD(std::map<int, Vm> vm_info, int type, std::map<int, int> predict_data) :vminfo(vm_info),order_type(type),predict_data(predict_data){}

//get Ai
void FFD::getA()
{
    int count = 0;
    double A1 = 0, A2 = 0;
    for(auto &t: predict_data)
    {
        A1 += (vminfo[t.first].core * t.second);
        A2 += (vminfo[t.first].mem * t.second);
        count += t.second;
        total_vm_num += t.second;
    }
    A1 = A1 * 1.0 / count;
    A2 = A2 * 1.0 / count;
    paramA.push_back(A1);
    paramA.push_back(A2);

}


void FFD::get_vm_size_dot()  {
    double memr = 0, corer = 0;
    for (int i = 0; i < result_Bin.size(); i++) {
        corer += result_Bin[i].cores;
        memr += result_Bin[i].mems;
    }
    for (auto &t: vminfo) {
        t.second.size = paramA[0] * (corer * t.second.core) + paramA[1] * (memr * t.second.mem);
    }
}


void FFD::get_vm_size_L2() {
    double memr = 0, corer = 0;
    for (int i = 0; i < result_Bin.size(); i++) {
        corer += result_Bin[i].cores;
        memr += result_Bin[i].mems;
    }
    for (auto &t: vminfo) {
       t.second.size = paramA[0] * (corer - t.second.core) * (corer - t.second.core) + paramA[1] * (memr - t.second.mem) * (memr - t.second.mem);
    }
}

//获取最大size的虚拟机序号
int FFD::get_max_vm()
{
    int index = -1;

    if(order_type == 1){
        double maxsize = 0;
        get_vm_size_dot();
        for (auto &t: predict_data) {
            if(t.second >0 && vminfo[t.first].size > maxsize){
                maxsize = vminfo[t.first].size;
                index = t.first;
            }
        }
    }else if(order_type == 2){
        get_vm_size_L2();
        double minsize = -1;
        for (auto &t: predict_data) {
            if(t.second >0 && minsize == -1){
                minsize = vminfo[t.first].size;
                index = t.first;
                continue;
            }
            if(t.second >0 && vminfo[t.first].size < minsize){
                minsize = vminfo[t.first].size;
                index = t.first;
            }
        }
    }
    return index;
}



std::vector<Bin> FFD::FFD_Dot()

{
    std::map<int, int> predict_data_tmp = predict_data;
    //首先确定优化目标
    int target;
    const char * target_string  = "CPU\n";
    if (strcmp(target_string, BasicInfo::opt_object) == 0){
        target = 0;//CPU
    }else{
        target = 1;//MEM
    }

    result_Bin.clear();
    //获取权重系数
    getA();

    int max_order;
    std::map<int, Vm>::iterator ite_info;
    std::map<int, int>::iterator ite_num;

    Bin temp = Bin(BasicInfo::server_info.core,BasicInfo::server_info.mem);
    result_Bin.push_back(temp);


    max_order = get_max_vm();
    ite_info = vminfo.find((max_order));
    ite_num = predict_data.find(max_order);

    result_Bin[0].put(ite_info->second);
    total_vm_num -= 1;
    ite_num->second -= 1;

    //insert_record(max_order,0);

    while(total_vm_num > 0){
        bool is_allocated = false; // 用于判断现有箱子是否可以满足分配需求
        max_order = get_max_vm();
        ite_info = vminfo.find((max_order));
        ite_num = predict_data.find(max_order);
        for(int a=0; a<result_Bin.size(); a++){
            is_allocated = result_Bin[a].put(ite_info->second);
            if(is_allocated){
                total_vm_num -= 1;
                ite_num->second -= 1;
                break;
            }

        }

        if(is_allocated==false){
            result_Bin.push_back(Bin(BasicInfo::server_info.core, BasicInfo::server_info.mem)); // 打开一个新箱子
            result_Bin.back().put(ite_info->second);
            total_vm_num -= 1;
            ite_num->second -= 1;
        }
    }
    return result_Bin;
}





