import pandas as pd
import numpy as np
from statsmodels.tsa.ar_model import AR
from sklearn.metrics import mean_squared_error
import math
#该文件中函数为使用统计的方法进行预测

#使用过去周期中的平均值直接对未来进行预测
def mean_value(data, train_len, test_len):
    sum_data = sum(data)
    predict_num = sum_data / train_len *test_len
    return int(predict_num)

    

#使用先移动平均，再进行AR预测
def mv_and_ar(train_series,test_begin_date, test_end_date, window_len = 6):
    data_ma = train_series.rolling(window=window_len,center=False).mean()
    data_ma = data_ma.dropna(axis=0, how='any')
    #print (data_ma)
    model = AR(data_ma)
    model_fit = model.fit()
    #make predictions
    tmp = model_fit.predict(start=test_begin_date.strftime("%Y-%m-%d"), end=test_end_date.strftime("%Y-%m-%d"), dynamic=True)
    predictions = round(max(round(sum(tmp)),0))
    return predictions

#使用特定的ARMA进行预测
def arma(data,test_len):
    pass

def get_scores(predict, actual, target_ecs):
    predict_value = []
    actual_value = []
    for ecs in target_ecs:
        predict_value.append(predict[ecs])
        actual_value.append(actual[ecs])
    sum_predict = 0.0
    sum_actual = 0.0
    sum_minue = 0.0
    n = len(predict_value)
    for i in range(len(predict_value)):
        sum_predict += pow(predict_value[i],2)
        sum_actual += pow(actual_value[i],2)
        sum_minue += pow ((predict_value[i]-actual_value[i]),2)
    sum_actual = sum_actual/n
    sum_actual = math.sqrt(sum_actual)
    sum_predict = sum_predict/n
    sum_predict = math.sqrt(sum_predict)
    sum_minue = sum_minue/n
    sum_minue = math.sqrt(sum_minue)
    score = 1-sum_minue/(sum_actual+sum_predict)
    print("predict scrore: %f" % (score))