# -*- coding: utf-8 -*-
# 参考链接：
# https://www.cnblogs.com/foley/p/5582358.html
# https://www.cnblogs.com/bradleon/p/6832867.html

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from statsmodels.graphics.tsaplots import plot_acf, plot_pacf
from statsmodels.tsa.stattools import adfuller
from statsmodels.stats.diagnostic import acorr_ljungbox

# 移动平均图
def draw_trend(timeSeries, size):
    f = plt.figure(facecolor='white')
    # 对size个数据进行移动平均
    rol_mean = timeSeries.rolling(window=size).mean()
    # 对size个数据进行加权移动平均
    rol_weighted_mean = pd.ewma(timeSeries, span=size)

    timeSeries.plot(color='blue', label='Original')
    rol_mean.plot(color='red', label='Rolling Mean')
    rol_weighted_mean.plot(color='black', label='Weighted Rolling Mean')
    plt.legend(loc='best')
    plt.title('Rolling Mean')
    plt.show()

def draw_ts(timeSeries):
    f = plt.figure(facecolor='white')
    timeSeries.plot(color='blue')
    plt.show()

'''
　　Unit Root Test
   The null hypothesis of the Augmented Dickey-Fuller is that there is a unit
   root, with the alternative that there is no unit root. That is to say the
   bigger the p-value the more reason we assert that there is a unit root
'''
def test_stationarity(timeseries, watch_windows = 12):
    
    #这里以一年为一个窗口，每一个时间t的值由它前面12个月（包括自己）的均值代替，标准差同理。
    rolmean = pd.rolling_mean(timeseries,window=watch_windows)
    rolstd = pd.rolling_std(timeseries, window=watch_windows)
    
    #plot rolling statistics:
    fig = plt.figure(figsize=(16,16))
    fig.add_subplot()
    orig = plt.plot(timeseries, color = 'blue',label='Original')
    mean = plt.plot(rolmean , color = 'red',label = 'rolling mean')
    std = plt.plot(rolstd, color = 'black', label= 'Rolling standard deviation')
    
    plt.legend(loc = 'best')
    plt.title('Rolling Mean & Standard Deviation')
    plt.show(block=False)
    
    
    #Dickey-Fuller test:
    
    print 'Results of Dickey-Fuller Test:'
    dftest = adfuller(timeseries,autolag = 'AIC')
    #dftest的输出前一项依次为检测值，p值，滞后数，使用的观测数，各个置信度下的临界值
    dfoutput = pd.Series(dftest[0:4],index = ['Test Statistic','p-value','#Lags Used','Number of Observations Used'])
    for key,value in dftest[4].items():
        dfoutput['Critical value (%s)' %key] = value
    
    print dfoutput

# 自相关和偏相关图，默认阶数为31阶
def draw_acf_pacf(ts, lags=31):
    f = plt.figure(facecolor='white', figsize=(16, 16))
    ax1 = f.add_subplot(211)
    plot_acf(ts, lags=31, ax=ax1)
    ax2 = f.add_subplot(212)
    plot_pacf(ts, lags=31, ax=ax2)
    plt.show()

#进行adf测试，如果满足平稳就返回true
def adf_test(data, p_value):
    result = adfuller(data)
    if (result[1] >= p_value):
        print('ADF statistc: %f' %result[0])
        print('p-value: %f' %result[1])
        print('Critical Values:')
        for key, value in result[4].items():
            print('\t%s:%.3f' %(key,value))
        return False
    return  True

#进行白噪声检测
def white_noise_test(data, p_value):
    result = acorr_ljungbox(data)
    if result[1][0] > p_value:
        print('state value: %f'  %result[0][0])
        print('p value: %f'  %result[1][0])
        return False
    else:
        return True

