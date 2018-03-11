# -*- coding: utf-8 -*-
import re
from collections import OrderedDict

# 该函数用于处理原始数据，分割得到训练集，测试集，验证集
def get_sets(ecs_lines, set_dates, target_types):
    # 为每一个所需要预测的虚拟机，分配一个数据结构，存放信息，即，训练集中某一天，有多少数目的该虚拟机
    # 格式如
    # {
    #     flavor1: {2015.01.01: 3, 2015.01.02: 4, 2015.01.03: 5},
    #     flavor2: {2015.01.01: 5, 2015.02.03: 5, 2015.02.06: 6}
    # }
    data = OrderedDict()
    # 同上，不过是存放测试集的信息
    
    for esc_line in ecs_lines:
        # 每行按空串分割三次，分割结果：[id, type, date, time]
        # 如['564bfd2c-8b99', 'flavor8', '2015-03-31', '22:15:00\n']
        line_split_res = re.split(r"\s+", esc_line, 3)
        type = line_split_res[1] 
        date = line_split_res[2]
        # 如果该行虚拟机类型在所需要预测的虚拟机类型中，则作相应的计数
        if type in target_types:
            # train_info.setdefault(type, dict()).setdefault(date, 0)
            # train_info[type][date] = train_info[type][date] + 1
            # 上面这两行代码 等同于 下面这行代码
            # 判断属于训练集、测试集还是验证集
            if set_dates[0] <= date <= set_dates[1]:
                data[type][date] = data.setdefault(type, OrderedDict()).setdefault(date, 0) + 1
            elif data > set_dates[1]:
                break
    
    return data        
     
