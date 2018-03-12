# -*- coding: utf-8 -*-
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from statsmodels.tsa.ar_model import AR
from sklearn.metrics import mean_squared_error

def ar(train_series, predict_dates):
    # train autoregression
    model = AR(train_series)
    model_fit = model.fit()
    # print('Lag: %s' % model_fit.k_ar)
    # print('Coefficients: %s' % model_fit.params)
    # make predictions
    predictions = model_fit.predict(start=predict_dates[0], end=predict_dates[1], dynamic=True)
    return predictions

# 显示 ar 算法的预测结果
# 参数：训练集，预测时间，目标
def print_ar_res(train_dataframe, predict_dates, actual_data, target_types):
    dataframe = pd.DataFrame(index=target_types, columns=['actual', 'predict'])
    for type in target_types:
        dataframe.loc[type].predict = max(round(sum(ar(train_dataframe[type], predict_dates))), 0)
        dataframe.loc[type].actual = actual_data.setdefault(type, 0)
        
    print dataframe
    print "train dates:  %s - %s" % (train_dataframe.index[0].strftime('%Y-%m-%d'), train_dataframe.index[-1].strftime('%Y-%m-%d'))
    print "predict dates:%s - %s" % (predict_dates[0], predict_dates[1])
    
    
    