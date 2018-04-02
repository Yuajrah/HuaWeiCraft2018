//
// Created by root on 18-3-31.
//

#include "FFD.h"
#include <cstring>
#include "Bin.h"
#include "BasicInfo.h"

//箱子的结果数组
std::vector<Bin> result_bin;
//权重系数数组
std::vector<double> paramA;
//虚拟机数组
std::vector<Vm> vm(16);
std::map<int, int> predict_data_tmp;
std::vector<std::map<int,int>>result_record;
int total_vm_num = 0;

//get Ai
std::vector<double> getA (std::map<int, Vm> &vm_info , std::map<int, int> &predict_data)
{
    std::vector<double> A(2,0);
    std::map<int, Vm>::iterator ite_vm_info;
    std::map<int, int>::iterator ite_vm_num;
    int count = 0;
    for(int i=1; i<=15; i++)
    {
        ite_vm_info = vm_info.find(i);
        ite_vm_num = predict_data.find(i);
        A[0] += (ite_vm_info->second.core * ite_vm_num->second);
        A[1] += (ite_vm_info->second.mem * ite_vm_num->second);
        count += ite_vm_num->second;
        total_vm_num += ite_vm_num->second;
    }
    A[0] = A[0] * 1.0 / count;
    A[1] = A[1] * 1.0 / count;

    return A;
}


void get_vm_size_dot(std::map<int, Vm> &vm_info, std::vector<Bin> &bin, std::vector<double> &A) {
    double memr = 0, corer = 0;
    std::map<int, Vm>::iterator ite_vm_info;
    for (int i = 0; i < bin.size(); i++) {
        memr += bin[i].cores;
        corer += bin[i].mems;
    }
    for (int i = 1; i <= 15; i++) {
        ite_vm_info = vm_info.find(i);
        ite_vm_info->second.size = A[0] * (corer * ite_vm_info->second.core) + A[1] * (memr * ite_vm_info->second.mem);
    }
}


void get_vm_size_L2(std::map<int, Vm> &vm_info, std::vector<Bin> &bin, std::vector<double> &A){
    double memr = 0, corer = 0;
    std::map<int, Vm>::iterator ite_vm_info;
    for (int i = 0; i < bin.size(); i++) {
        memr += bin[i].cores;
        corer += bin[i].mems;
    }
    for (int i = 1; i <= 15; i++) {
        ite_vm_info = vm_info.find(i);
        ite_vm_info->second.size = A[0] * (corer - ite_vm_info->second.core) * (corer - ite_vm_info->second.core) + A[1] * (memr - ite_vm_info->second.mem) * (memr - ite_vm_info->second.mem);
    }
}

//获取最大size的虚拟机序号
int get_max_vm(std::map<int, Vm> &vm_info, std::map<int, int> &predict_data, int orderType)
{

    if(orderType == 1){
        get_vm_size_dot(vm_info, result_bin, paramA);
    }else if(orderType == 2){
        get_vm_size_L2(vm_info, result_bin, paramA);
    }
    double maxsize = 0;
    int index = -1;
    std::map<int, Vm>::iterator ite_info;
    std::map<int, int>::iterator ite_num;
    for(int i=1; i<=15; i++){
        ite_num = predict_data.find(i);
        //剩余虚拟机数量大于0
        if(ite_num->second > 0){
            ite_info = vm_info.find(i);
            if(ite_info->second.size > maxsize){
                maxsize = ite_info->second.size;
                index = i;
            }
        }
    }
    return index;
}

void insert_record(int maxorder, int bin_index){
    if(result_record[bin_index].find(maxorder) == result_record[bin_index].end())
    {
        result_record[bin_index][maxorder] = 1;
    }
    else
    {
        ++result_record[bin_index][maxorder];
    }
}


std::vector<std::map<int,int>> FFD_Dot (std::map<int, Vm> vm_info, Server server, std::map<int, int> predict_data,  char *opt_object, int order_type)
{
    std::map<int, int> predict_data_tmp = predict_data;
    //首先确定优化目标
    int target;
    const char * target_string  = "CPU\n";
    if (strcmp(target_string, opt_object) == 0){
        target = 0;//CPU
    }else{
        target = 1;//MEM
    }

    result_bin.clear();
    result_record.clear();
    //获取权重系数
    paramA = getA(vm_info, predict_data);

    int max_order;
    bool is_allocated = false; // 用于判断现有箱子是否可以满足分配需求
    std::map<int,int> new_record;
    std::map<int, Vm>::iterator ite_info;
    std::map<int, int>::iterator ite_num;

    Bin temp = Bin(server.core,server.mem);
    result_bin.push_back(temp);
    result_record.push_back(new_record);

    max_order = get_max_vm(vm_info, predict_data, order_type);
    ite_info = vm_info.find((max_order));
    ite_num = predict_data.find(max_order);

    result_bin[0].put(ite_info->second);
    total_vm_num -= 1;
    ite_num->second -= 1;

    insert_record(max_order,0);

    while(total_vm_num > 0){
        max_order = get_max_vm(vm_info, predict_data, order_type);
        ite_info = vm_info.find((max_order));
        ite_num = predict_data.find(max_order);
        for(int a=0; a<result_bin.size(); a++){
            is_allocated = result_bin[a].put(ite_info->second);
            if(is_allocated){
                total_vm_num -= 1;
                ite_num->second -= 1;
                insert_record(max_order, a);
                break;
            }else{
                if(a == result_bin.size()-1){
                    result_bin.push_back(Bin(BasicInfo::server_info.core, BasicInfo::server_info.mem)); // 打开一个新箱子
                    result_bin.back().put(ite_info->second);
                    std::map<int,int> new_record;
                    result_record.push_back(new_record);
                    total_vm_num -= 1;
                    ite_num->second -= 1;
                    insert_record(max_order, a+1);
                    break;
                }
            }

        }
    }
    get_scores_f(predict_data_tmp, server, result_bin.size(), target, vm_info);
    return result_record;
}



void get_scores_f(std::map<int, int>predict_data, Server server, int number, int target, std::map<int, Vm> vm_info)
{
    int total_allocate;
    if (target == 0)
    {
        total_allocate = server.core * number;
    }
    else{
        total_allocate = server.mem *number;
    }
    int total_need = 0;
    for (int i = 1; i<=15; i++)
    {
        std::map<int ,int >::iterator iter;
        iter = predict_data.find(i);
        if (iter == predict_data.end())
        {
            continue;
        }
        else
        {
            int target_need = 0;
            std::map<int, Vm>::iterator current_flavor_info;
            current_flavor_info =  vm_info.find(i);;
            if (target == 0)
            {
                target_need = current_flavor_info->second.core;
            }
            else
            {
                target_need = current_flavor_info->second.mem;
            }
            total_need += iter->second * target_need;
        }
    }
    double percent = (total_need+0.0)/total_allocate;
    printf("allocate score = %f\n", percent);
}

