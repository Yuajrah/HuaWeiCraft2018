#include "frist_fit.h"
#include <cstring>

std::vector<std::vector<int>> frist_fit(std::map<int, Vm> vm_info, Server server, std::map<int, int> predict_data,  char *opt_object)
{
    //首先确定优化目标
    int target;
    const char * target_string  = "CPU";
    if (strcmp(target_string, opt_object))
    {
        target = 0;
    }
    else
    {
        target = 1;
    }
    // 保存最终结果的，vector中的ID对应编号为多少的分配结果
    std::vector<std::vector<int>> result_record;
    // 初始化服务器节点
    int server_number = 0;
    std::priority_queue<Allocat_server> allocate_result;

    //首先初始化一个节点
    Allocat_server new_server = allocate_one(server_number, server.core, server.storage, target);
    allocate_result.push(new_server);
    std::vector<int> new_record;
    result_record.push_back(new_record);

    //对于预测文档的数据逐个进行分配
    while (!predict_data.empty())
    {
        //首先选择id最大的虚拟服务器作为当前需要处理的节点
        //frist为id,second为数量
        std::map<int ,int >::iterator current_flavor;
        for(int i = 15; i>0; i++ )
        {
            std::map<int ,int >::iterator iter;
            iter = predict_data.find(i);
            if (iter == predict_data.end())
            {
                continue;
            }
            else
            {
                if (iter->second == 0)
                {
                    predict_data.erase(iter);
                    continue;
                }
                else
                {
                    current_flavor = iter;
                    break;
                }
            }
        }
        //获取当前目标flavor的一些参数
        int core_need;
        int storage_need;
        std::map<int, Vm>::iterator current_flavor_info;
        current_flavor_info =  vm_info.find(current_flavor->first);
        core_need = current_flavor_info->second.core;
        storage_need = current_flavor_info->second.storage;
        
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
            if(current_server_data.core >= core_need && current_server_data.storage >= storage_need)
            {
                current_server_data.core -= core_need;
                current_server_data.storage -= storage_need;
                tmp_allocate_result.push(current_server_data);
                //把当前的处理加到记录里面去
                result_record[current_server_data.id].push_back(current_flavor->first);
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
            Allocat_server new_server = allocate_one(server_number, server.core - core_need, server.storage-storage_need, target);
            allocate_result.push(new_server);
            std::vector<int> new_record;
            result_record.push_back(new_record);
            //保存记录
            result_record[new_server.id].push_back(current_flavor->first);

        }

        //当前处理的虚拟机的数量减一
        current_flavor->second--;
    }
    get_scores(predict_data, server, server_number+1, target, vm_info);
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

Allocat_server allocate_one(int id, int core, int storage, int target)
{
    Allocat_server new_allocate;
    new_allocate.id = id;
    new_allocate.core = core;
    new_allocate.storage = storage;
    new_allocate.target = target;
}

void get_scores(std::map<int, int>predict_data, Server server, int number, int target, std::map<int, Vm> vm_info)
{
    int total_allocate;
    if (target == 0)
    {
        total_allocate = server.core * number;
    }
    else{
        total_allocate = server.storage *number;
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
                    target_need = current_flavor_info->second.storage;
                }
                total_need = iter->second * target_need;
            }
    }
    double percent = (total_need+0.0)/total_allocate;
    printf("allocate score = %f\n", percent);
}