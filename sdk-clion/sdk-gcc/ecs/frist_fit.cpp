#include "frist_fit.h"
#include <cstring>

std::vector<std::map<int,int>> frist_fit(std::map<int, Vm> vm_info, Server server, std::map<int, int> predict_data,  char *opt_object, std::vector<int> order)
{
    std::map<int, int> predict_data_tmp = predict_data;
    //首先确定优化目标
    int target;
    if (BasicInfo::is_cpu())
    {
        target = 0;
    }
    else
    {
        target = 1;
    }
    // 保存最终结果的，vector中的ID对应编号为多少的分配结果
    std::vector<std::map<int,int>>result_record;
    // 初始化服务器节点
    int server_number = 0;
    std::priority_queue<Allocat_server> allocate_result;

    //首先初始化一个节点
    Allocat_server new_server = allocate_one(server_number, server.core, server.mem, target);
    allocate_result.push(new_server);
    std::map<int,int> new_record;
    result_record.push_back(new_record);

    //对于预测文档的数据逐个进行分配
    while (!predict_data.empty())
    {
        //首先选择id最大的虚拟服务器作为当前需要处理的节点
        //frist为id,second为数量
        std::map<int, int>::iterator current_flavor = predict_data.end();
        for (int i = 0; i < order.size(); i++) {
            std::map<int, int>::iterator iter;
            iter = predict_data.find(order[i]);
            if (iter == predict_data.end()) {
                continue;
            } else {
                if (iter->second == 0) {
                    predict_data.erase(iter);
                    continue;
                } else {
                    current_flavor = iter;
                    break;
                }
            }
        }

        if (current_flavor == predict_data.end()) break;

        //获取当前目标flavor的一些参数
        int core_need;
        int mem_need;
        std::map<int, Vm>::iterator current_flavor_info;
        current_flavor_info =  vm_info.find(current_flavor->first);
        core_need = current_flavor_info->second.core;
        mem_need = current_flavor_info->second.mem;
        
        //对当前的的虚拟服务器进行处理
        //首先判断是否需要开辟新的服务器
        bool need_allocate_new_server = true;
        std::priority_queue<Allocat_server> tmp_allocate_result;
        //如果非空，就一直遍历
        while(!allocate_result.empty())
        {
            Allocat_server current_server_data;
            current_server_data = allocate_result.top();
            allocate_result.pop();
            //如果弹出的满足分配条件，就自减然后然后放进tmp，否则就直接压进tmp
            if(current_server_data.core >= core_need && current_server_data.mem >= mem_need)
            {
                current_server_data.core -= core_need;
                current_server_data.mem -= mem_need;
                tmp_allocate_result.push(current_server_data);
                //把当前的处理加到记录里面去
                if( result_record[current_server_data.id].find(current_flavor->first) == result_record[current_server_data.id].end())
                {
                    result_record[current_server_data.id][current_flavor->first] = 1;
                }
                else
                {
                    ++result_record[current_server_data.id][current_flavor->first];
                }
                need_allocate_new_server = false;
                break;
            }
            else{
                tmp_allocate_result.push(current_server_data);
            }

        }
        //首先将tmp_allocate 和allocate进行合拼
        merge_allocate(allocate_result, tmp_allocate_result);

        //判断是否需要开辟新的服务器
        if (need_allocate_new_server)
        {
            //需要，就开辟一个新服务器，同时分配当前的虚拟机
            server_number++;
            Allocat_server new_server = allocate_one(server_number, server.core - core_need, server.mem-mem_need, target);
            allocate_result.push(new_server);
            std::map<int,int> new_record;
            result_record.push_back(new_record);
            //保存记录
            if( result_record[new_server.id].find(current_flavor->first) == result_record[new_server.id].end())
            {
                result_record[new_server.id][current_flavor->first] = 1;
            }
            else
            {
                ++result_record[new_server.id][current_flavor->first];
            }

        }

        //当前处理的虚拟机的数量减一
        current_flavor->second--;
    }
    get_scores(predict_data_tmp, server, server_number+1, target, vm_info);
    /**
     * [
     *      0: {flavor1: xx, flavor2: xx...}
     *      1: {flavor1: xx, flavor2: xx...}
     *      ...
     *
     * ]
     */
    return result_record;
}

void merge_allocate (std::priority_queue<Allocat_server> &data1, std::priority_queue<Allocat_server> &data2)
{
    Allocat_server tmp;
    while(!data2.empty())
    {
        tmp = data2.top();
        data2.pop();
        data1.push(tmp);
    }

}

Allocat_server allocate_one(int id, int core, int mem, int target)
{
    Allocat_server new_allocate;
    new_allocate.id = id;
    new_allocate.core = core;
    new_allocate.mem = mem;
    new_allocate.target = target;
    return new_allocate;
}

void get_scores(std::map<int, int>predict_data, Server server, int number, int target, std::map<int, Vm> vm_info)
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

//确定order的函数
std::vector<int> get_order(std::map<int, Vm> vm_info, Server server, char *opt_object)
{
    std::vector<int> order;
    if (BasicInfo::is_cpu())
    {
        for (int i = 15; i >0; i-- )
        {
            order.push_back(i);
        }
    }
    else
    {
        order.push_back(15);
        order.push_back(14);
        order.push_back(12);
        order.push_back(13);
        order.push_back(11);
        order.push_back(9);
        order.push_back(10);
        order.push_back(8);
        order.push_back(6);
        order.push_back(7);
        order.push_back(5);
        order.push_back(3);
        order.push_back(4);
        order.push_back(2);
        order.push_back(1);
    }


    return order;
}