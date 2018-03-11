# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import pandas as pd
import os
from collections import OrderedDict

# 如果filename_path不存在，则创建目录
def mkdir(filename_path):
    file_dir = os.path.split(filename_path)[0]
    if not os.path.isdir(file_dir):
        os.makedirs(file_dir)

# 绘制集合中所有虚拟机的信息
def plot_set_data(set_info, save_filename=None):      
    dataframe = pd.DataFrame(set_info).fillna(0)
    dataframe.index = pd.to_datetime(dataframe.index)
    dataframe.plot(figsize=(16, 16), linestyle='--', marker='o', grid=True)
    if save_filename != None:
        mkdir(save_filename)
        plt.savefig(save_filename)
    pass

# 绘制集合中所有虚拟机的累计信息
def plot_set_cumsum_data(set_info, save_filename=None):
    dataframe = pd.DataFrame(set_info).fillna(0)
    dataframe.index = pd.to_datetime(dataframe.index)
    dataframe = dataframe.cumsum()
    dataframe.plot(figsize=(16, 16), linestyle='-', marker='o', grid=True)
    if save_filename != None:
        mkdir(save_filename)
        plt.savefig(save_filename)
        pass
    
# 绘制集合中，某个vm的信息
def plot_set_single_vm_data(vm_type, set_info, save_filename=None):
    if not set_info.has_key(vm_type):
        print "没有vm类型：" + vm_type
        return False
    plot_set_data({vm_type: set_info[vm_type]}, save_filename)