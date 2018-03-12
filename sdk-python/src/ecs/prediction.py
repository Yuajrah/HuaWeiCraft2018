# -*- coding: utf-8 -*-
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from statsmodels.tsa.ar_model import AR
from sklearn.metrics import mean_squared_error

#输出预测的得分，输出参数为两个list
def get_score(predict, actual):
    predict_sum = sum([x**2 for x in predict])/(len(predict))
    actual_sum = sum([x**2 for x in actual])/(len(actual)+0.0)
    score = 1-mean_squared_error(actual, predict)/(predict_sum + actual_sum)
    print "predict scrore: %f" % (score)
    
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
    actual = []
    predict = []
    for type in target_types:
        tmp_predict = max(round(sum(ar(train_dataframe[type], predict_dates))), 0)
        dataframe.loc[type].predict = tmp_predict
        predict.append(tmp_predict)
        tmp_actual = actual_data.setdefault(type, 0)
        dataframe.loc[type].actual = tmp_actual
        actual.append(tmp_actual)
        
        
    print dataframe
    get_score(predict, actual)
    print "train dates:  %s - %s" % (train_dataframe.index[0].strftime('%Y-%m-%d'), train_dataframe.index[-1].strftime('%Y-%m-%d'))
    print "predict dates:%s - %s" % (predict_dates[0], predict_dates[1])
    


def mv_and_ar(train_series, watch_windows, predict_dates, target_types, actual_data):
    #show_data(train_series, target_types)
    data_ma = train_series.rolling(window=watch_windows,center=False).mean()
    data_ma = data_ma.dropna(axis=0, how='any')
    print_ar_res(data_ma, predict_dates, actual_data,  target_types )