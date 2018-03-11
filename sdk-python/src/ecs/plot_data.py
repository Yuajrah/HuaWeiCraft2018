# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import pandas as pd
import os

def mkdir(filename_path):
    file_dir = os.path.split(filename_path)[0]
    if not os.path.isdir(file_dir):
        os.makedirs(file_dir)

def plot_set_data(set_info, save_filename=None):        
    dataframe = pd.DataFrame(set_info).fillna(0)
    dataframe.index = pd.to_datetime(dataframe.index)
    dataframe.plot(figsize=(16, 16), linestyle='--', marker='o', grid=True)
    if save_filename != None:
        mkdir(save_filename)
        plt.savefig(save_filename)
    pass

def plot_set_cumsum_data(set_info, save_filename=None):
    dataframe = pd.DataFrame(set_info).fillna(0)
    dataframe.index = pd.to_datetime(dataframe.index)
    dataframe = dataframe.cumsum()
    dataframe.plot(figsize=(16, 16), linestyle='-', marker='o', grid=True)
    if save_filename != None:
        mkdir(save_filename)
        plt.savefig(save_filename)
        pass