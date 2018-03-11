# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import pandas as pd

def plot_set_data(set_info, save_filename=None):        
    dataframe = pd.DataFrame(set_info).fillna(0)
    dataframe.index = pd.to_datetime(dataframe.index)
    dataframe.plot(figsize=(16, 16), linestyle='--', marker='o', grid=True)
    if save_filename != None:
        plt.savefig(save_filename)
    pass

def plot_set_cumsum_data(set_info, save_filename=None):
    dataframe = pd.DataFrame(set_info).fillna(0)
    dataframe.index = pd.to_datetime(dataframe.index)
    dataframe = dataframe.cumsum()
    dataframe.plot(figsize=(16, 16), linestyle='-', marker='o', grid=True)
    if save_filename != None:
        plt.savefig(save_filename)
    pass