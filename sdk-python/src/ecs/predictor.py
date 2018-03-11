# coding=utf-8
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from get_sets import get_sets, get_test_data
from plot_data import plot_set_data, plot_set_cumsum_data

def predict_vm(ecs_lines, input_lines):
    # Do your work from here#
    
    # 存放训练集，测试集，验证集的起止数据
    set_dates =  ["2015-01-01", "2015-03-31"]
    
    # 所需要预测的虚拟机类型
    target_types = ["flavor1", "flavor2", "flavor3", "flavor4", "flavor5",
                  "flavor6", "flavor7", "flavor8", "flavor9", "flavor10",
                  "flavor11", "flavor12", "flavor13", "flavor14", "flavor15"]
    
    # 该函数用于处理原始数据，分割得到训练集，测试集，验证集
    data = get_sets(ecs_lines, set_dates, target_types)
    
    test_dates = ["2015-04-01", "2015-04-07"]
    
    test_data = get_test_data(ecs_lines, test_dates, target_types)
    print(test_data)

    # 绘制集合信息
    plot_set_data(data, "../../../imgs/train_info.png")
    plot_set_cumsum_data(data, "../../../imgs/train_cumsum_info.png");
    
    result = []
    if ecs_lines is None:
        print 'ecs information is none'
        return result
    if input_lines is None:
        print 'input file information is none'
        return result

    return result
