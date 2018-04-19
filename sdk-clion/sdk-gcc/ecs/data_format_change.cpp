//
// Created by caocongcong on 18-3-19.
//
#include "data_format_change.h"
#include "Random.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include "BasicInfo.h"

/**
 * 格式化预测结果, 以方便写入最终文件
 * @param data
 * @return
 */
std::string format_predict_res(std::map<int, int>data)
{

    std::map<int,int>::iterator it;

    it = data.begin();
    int sum = 0;
    std::string tmp_string;
    while(it != data.end())
    {
        sum += it->second;
        tmp_string += "flavor";
        tmp_string += std::to_string(it->first);
        tmp_string += " ";
        tmp_string += std::to_string(it->second);
        tmp_string += "\n";
        it ++;
    }
    tmp_string += "\n";
    std::string head = std::to_string(sum);
    head += "\n";
    std::string result = head+tmp_string;
    return result;
}

/**
 * 格式化分配结果, 以方便写入最终文件
 * @param result_code
 * @return
 */
std::string format_allocate_res(std::vector<std::map<int,int>> result_code)
{
    std::string result;
    result = std::to_string(result_code.size());
    result += "\n";
    for (size_t i =0; i < result_code.size(); i++) {
        result += std::to_string(i+1);
        for (std::map<int, int>::iterator j=result_code[i].begin(); j!=result_code[i].end(); j++)
        {
            result += " ";
            result += "flavor";
            result += std::to_string(j->first);
            result += " ";
            result += std::to_string(j->second);
        }
        result += "\n";
    }
    return result;
}

/**
 * 格式化分配结果, 以方便写入最终文件
 * @param bins 分配结果, 为Bin类型的vector, 每个元素里面有objects
 * @return
 */
std::string format_allocate_res(std::vector<Bin> bins)
{
    std::string result;
    result = std::to_string(bins.size());
    result += "\n";
    int cnt = 0;

    for (auto &bin: bins) {

        cnt++;

        result += std::to_string(cnt);
        std::map<int, int> bin_objects; // 对一个箱子中的各个虚拟机计数

        for (auto object: bin.objects) {
            bin_objects[object.type]++;
        }

        for (auto &bin_object: bin_objects)
        {
            result += " ";
            result += "flavor";
            result += std::to_string(bin_object.first);
            result += " ";
            result += std::to_string(bin_object.second);
        }

        result += "\n";
    }
    return result;
}

/**
 *
 * 将预测结果, 转化成物体的序列, 然后再用于装箱问题
 * predict_data: {vm1: 3个, vm2: 1个 ... }
 * =>
 * objects: {vm1, vm1, vm1, vm2 ... }
 *
 * @param predict_data
 * @param vm_info
 * @return 物体的序列
 *
 */
std::vector<Vm> serialize(std::map<int, int> predict_data){
    std::vector<Vm> objects;
    int cnt = 0;
    for (auto &t: predict_data) {
        for (int i=0;i<t.second;i++) {
            Vm vm = BasicInfo::vm_info[t.first];
            vm.no = cnt++;
            objects.push_back(vm);
        }
    }
    return objects;
}

/**
 * 随机产生一个objects的排列
 * @param objects
 * @return
 */
std::vector<Vm> random_permutation(std::vector<Vm> objects) {
    /**
     * 注意, 只有每次使用同一个引擎的实例, 才能在下一次调用该shuffle的时候生成不同的排列
     */
    std::shuffle(std::begin(objects), std::end(objects), Random::generator);
    return objects;
}


///**
// * 把背包的结果转化为bin的格式
// * @param packing_result
// * @return
// */
//std::vector<Bin> vector_res_to_bins_res(std::vector<std::map<int,int>> packing_result){
//    std::vector<Bin> bins;
//    int cnt = 0;
//    for (auto &server: packing_result) {
//        Bin bin(BasicInfo::server_info.core, BasicInfo::server_info.mem);
//        for (auto &vm: server) {
//            Vm t_vm = BasicInfo::vm_info[vm.first];
//            for (int i=0;i<vm.second;i++) {
//                t_vm.no = cnt++;
//                t_vm.type = vm.first;
//                bin.put(t_vm);
//            }
//        }
//        bins.push_back(bin);
//    }
//    return bins;
//}

/**
 * 对数据进行进一步处理
 * @param allocate_result
 * @param order_vm_info
 * @param predict_data
 */
void after_process(std::vector<Bin> &allocate_result, std::vector<std::pair<int, Vm>> &order_vm_info, std::map<int, int> &predict_data){

    for (int i=0;i<allocate_result.size();i++) {
        if (allocate_result[i].cores > 0 && allocate_result[i].mems > 0) {
            for (auto &vm_pair: order_vm_info) {
                while (allocate_result[i].cores >= vm_pair.second.core && allocate_result[i].mems >= vm_pair.second.mem) {
                    allocate_result[i].put(vm_pair.second);
                    predict_data[vm_pair.first]++;
                }
            }
        }
    }
}