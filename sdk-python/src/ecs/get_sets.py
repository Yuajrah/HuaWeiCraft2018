# -*- coding: utf-8 -*-
import re

def get_sets(ecs_lines, set_dates, target_types):
    # 为每一个所需要预测的虚拟机，分配一个数据结构，存放信息，即，训练集中某一天，有多少数目的该虚拟机
    # 格式如
    # {
    #     flavor1: {2015.01.01: 3, 2015.01.02: 4, 2015.01.03: 5},
    #     flavor2: {2015.01.01: 5, 2015.02.03: 5, 2015.02.06: 6}
    # }
    train_info = dict()
    # 同上，不过是存放测试集的信息
    test_info = dict()
    # 同上，不过是存放验证集的信息
    validate_info = dict()
    
    for esc_line in ecs_lines:
        # 每行按空串分割三次，分割结果：[id, type, date, time]
        # 如['564bfd2c-8b99', 'flavor8', '2015-03-31', '22:15:00\n']
        line_split_res = re.split(r"\s+", esc_line, 3)
        type = line_split_res[1]; 
        date = line_split_res[2];
        # 如果该行虚拟机类型在所需要预测的虚拟机类型中，则作相应的计数
        if type in target_types:
            # train_info.setdefault(type, dict()).setdefault(date, 0)
            # train_info[type][date] = train_info[type][date] + 1
            # 上面这两行代码 等同于 下面这行代码
            # 判断属于训练集、测试集还是验证集
            if set_dates['train'][0] <= date <= set_dates['train'][1]:
                train_info[type][date] = train_info.setdefault(type, dict()).setdefault(date, 0) + 1
            elif set_dates['test'][0] <= date <= set_dates['test'][1]:
                test_info[type][date] = test_info.setdefault(type, dict()).setdefault(date, 0) + 1
            elif set_dates['validate'][0] <= date <= set_dates['validate'][1] <= date <= validate_end_date:
                validate_info[type][date] = validate_info.setdefault(type, dict()).setdefault(date, 0) + 1
    
    return [train_info, test_info, validate_info]        
     
