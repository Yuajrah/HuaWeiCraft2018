//
// Created by caocongcong on 18-3-19.
//
#include "data_format_change.h"

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
std::vector<Vm> serialize(std::map<int, int> predict_data, std::map<int, Vm> vm_info){
    std::vector<Vm> objects;
    for (auto &t: predict_data) {
        for (int i=0;i<t.second;i++) {
            objects.push_back(vm_info[t.first]);
        }
    }
    return objects;
}