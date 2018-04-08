# -*- coding: utf-8 -*-
import re
import pandas as pd
from collections import OrderedDict

# 返回dataframe，这个dataframe的数据的断天是填充的
# 参数和get_sets相同
def get_sets_dataframe(ecs_lines, set_dates, target_types):
    data = get_sets(ecs_lines, set_dates, target_types)
    dataframe = pd.DataFrame(data).fillna(0) # 进行一次内键合并，NAN填充0，但是此时仍然有断天
    retframe = pd.DataFrame(index=pd.date_range(set_dates[0], set_dates[1])) # 生成连续的天的索引
    return retframe.join(dataframe).fillna(0) # 填充，并将NaA置位0
    
# 该函数用于处理原始数据，分割得到训练集
def get_sets(ecs_lines, set_dates, target_types):
    # 为每一个所需要预测的虚拟机，分配一个数据结构，存放信息，即，训练集中某一天，有多少数目的该虚拟机
    # 格式如
    # {
    #     flavor1: {2015.01.01: 3, 2015.01.02: 4, 2015.01.03: 5},
    #     flavor2: {2015.01.01: 5, 2015.02.03: 5, 2015.02.06: 6}
    # }
    data = OrderedDict()
    
    for esc_line in ecs_lines:
        # 每行按空串分割三次，分割结果：[id, type, date, time]
        # 如['564bfd2c-8b99', 'flavor8', '2015-03-31', '22:15:00\n']
        line_split_res = re.split(r"\s+", esc_line, 3)
        type = line_split_res[1] 
        date = line_split_res[2]
        # 如果该行虚拟机类型在所需要预测的虚拟机类型中，则作相应的计数
        if type in target_types:
            # 判断是都在目标日期之内
            if set_dates[0] <= date <= set_dates[1]:
                data[type][date] = data.setdefault(type, OrderedDict()).setdefault(date, 0) + 1
            elif date > set_dates[1]:
                break
    return data


#该函数用来获测试数据
def get_test_data(ecs_lines, set_dates, target_types):         
    """
    返回的数据类型为字典，统计在目标时段内对应虚拟机的总个数
    {
         flavor1: 1
         flavor2: 2
         ....
    }
    """
    data = {}
     
    for esc_line in ecs_lines:
        #首先进行分割
        line_split_res = re.split(r"\s+", esc_line, 3)
        type = line_split_res[1] 
        date = line_split_res[2]
        
        #如果虚拟机是目标虚拟机
        if type in target_types:
            #是否在目标日期之内
            if set_dates[0] <= date <= set_dates[1]:
                data[type] = data.setdefault(type, 0) + 1
            elif date > set_dates[1]:
                break
    return data

#读取输入文件信息，依次返回读取内容，数据格式为：
    #物理服务器信息：server_data  {“CPU”:vlaue, MEM:value}
    #需要观察的虚拟服务器类型: flavor_type [flavor1, flavor2....] 
    #虚拟服务器信息：flavor_data
    #{
    #      flavor1:{CPU: value, MEM: value}
    #      flavor2:{CPU: value, MEM: vlaue}
    #      ........ 
    #}
    #目标优化资源:target_resource CPU/MEM
    #预测的起始日期： predict_dates [2016-01-01, 2016-01-07]
def get_flavor_data(input_lines):
    server_data = {}
    flavor_type = []
    flavor_data = {}
     
    predict_dates = []
    line_number = 0
    #首先读取物理服务器信息
    server_line = input_lines[line_number]
    server_line_split = server_line.split() 
    server_data["CPU"] = int(server_line_split[0])
    server_data["MEM"] = int (server_line_split[1])
    #跳过空行
    line_number += 2
    
    #开始读取虚拟服务器信息 
    flavor_number = int(input_lines[line_number])
    line_number += 1
    change__to_Gb = 1024
    for i in range(line_number, line_number+flavor_number):
        flavor_line = input_lines[i]
        flavor_line_split = flavor_line.split()
        flavor_type.append(flavor_line_split[0])
        tmp_flavor = {}
        tmp_flavor["CPU"] = int(flavor_line_split[1])
        tmp_flavor["MEM"] = int(flavor_line_split[2])/change__to_Gb
        flavor_data[flavor_line_split[0]] = tmp_flavor
        line_number += 1
    
    #开始读取需要优化的资源
    line_number += 1
    target_resource = input_lines[line_number][0:3]
    
    #开始读取预测日期
    line_number += 2
    for i in range(line_number, line_number+2):
        date = input_lines[i]
        date_split = date.split()
        predict_dates.append(date_split[0])
    return server_data, flavor_type, flavor_data, target_resource, predict_dates
    
    
    