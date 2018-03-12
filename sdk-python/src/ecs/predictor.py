# coding=utf-8
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from get_sets import get_sets, get_sets_dataframe, get_test_data
from plot_data import plot_set_data, plot_set_cumsum_data, plot_set_single_vm_data, plot_set_single_vm_cumsum_data
from test_stationarity import draw_trend, test_stationarity, draw_acf_pacf
import statsmodels as sm
from prediction import ar, print_ar_res

def predict_vm(ecs_lines, input_lines):
    # Do your work from here#
    
    train_dates = ["2015-01-01", "2015-05-24"]
    predict_dates = ["2015-05-25", "2015-05-31"]
    # 所需要预测的虚拟机类型
    target_types = ["flavor1", "flavor2", "flavor3", "flavor4", "flavor5",
                  "flavor6", "flavor7", "flavor8", "flavor9", "flavor10",
                  "flavor11", "flavor12", "flavor13", "flavor14", "flavor15"]
    
    # 该函数用于处理原始数据，分割得到训练集，测试集，验证集
    train_data = get_sets(ecs_lines, train_dates, target_types)
    actual_data = get_test_data(ecs_lines, predict_dates, target_types)
    
    train_dataframe = get_sets_dataframe(ecs_lines, train_dates, target_types)
    actual_dataframe = get_sets_dataframe(ecs_lines, predict_dates, target_types)
        
     # 绘制集合信息
#    plot_set_data(data, "../../../imgs/train_info.png")
#    plot_set_cumsum_data(data, "../../../imgs/train_cumsum_info.png")
    
    # 绘制集合中单个vm的信息
    for type in target_types:
        plot_set_single_vm_data(type, train_data, "../../../imgs/single/train_" + type)    
        plot_set_single_vm_cumsum_data(type, train_data, "../../../imgs/single_cumsum/train_" + type)  
    
    # ar部分的预测结果
    # print_ar_res(train_dataframe, predict_dates, actual_data, target_types)


    result = []
    if ecs_lines is None:
        print 'ecs information is none'
        return result
    if input_lines is None:
        print 'input file information is none'
        return result

    return result