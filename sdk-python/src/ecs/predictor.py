# coding=utf-8
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from get_sets import get_sets
from plot_data import plot_set_data, plot_set_cumsum_data, plot_set_single_vm_data, plot_set_single_vm_cumsum_data

def predict_vm(ecs_lines, input_lines):
    # Do your work from here#
    
    # 存放训练集，测试集，验证集的起止数据
    set_dates =  ["2015-01-01", "2015-05-30"]
    
    # 所需要预测的虚拟机类型
    target_types = ["flavor1", "flavor2", "flavor3", "flavor4", "flavor5",
                  "flavor6", "flavor7", "flavor8", "flavor9", "flavor10",
                  "flavor11", "flavor12", "flavor13", "flavor14", "flavor15"]
    
    # 该函数用于处理原始数据，分割得到训练集，测试集，验证集
    data = get_sets(ecs_lines, set_dates, target_types)
    print data['flavor1']

    # 绘制集合信息
    plot_set_data(data, "../../../imgs/train_info.png")
    plot_set_cumsum_data(data, "../../../imgs/train_cumsum_info.png")
    
    # 绘制集合中单个vm的信息
    for type in target_types:
        plot_set_single_vm_data(type, data, "../../../imgs/single/train_" + type)    
        plot_set_single_vm_cumsum_data(type, data, "../../../imgs/single_cumsum/train_" + type)    
        
    result = []
    if ecs_lines is None:
        print 'ecs information is none'
        return result
    if input_lines is None:
        print 'input file information is none'
        return result

    return result