import pandas as pd
import numpy as np
def get_data(data, train_begin_date, test_begin_date, train_end_date, test_end_date):
    target_ecs = ["flavor1","flavor2","flavor3","flavor4","flavor5","flavor6","flavor7","flavor8","flavor9","flavor10","flavor11","flavor12","flavor13","flavor14","flavor15"]
    #首先对数据进行聚合,统计每一种服务器们每天的个数
    groupby_data = data.groupby(["date","ecs"]).size().reset_index(name="count")
    #生成一个新的时间序列
    begin_date = groupby_data.iloc[0]['date']
    end_date = groupby_data.iloc[-1]['date']
    full_data = pd.DataFrame(index=pd.date_range(begin_date,end_date))
    for ecs_type in target_ecs:
        tmp_data = groupby_data[groupby_data["ecs"]==ecs_type]
        tmp_data = tmp_data.set_index("date")
        tmp_data=tmp_data.drop(['ecs'], axis=1)
        tmp_data.rename(columns={'count':ecs_type}, inplace = True)
        full_data = full_data.join(tmp_data).fillna(0)
    train_data = full_data.iloc[full_data.index >= train_begin_date]
    train_data = train_data.iloc[train_data.index <= train_end_date]
    test_data = full_data.iloc[full_data.index >= test_begin_date]
    test_data = test_data.iloc[test_data.index <= test_end_date]    
    test_target = {}
    for ecs in target_ecs:
        tmp_data = sum(test_data[ecs])
        test_target[ecs] = tmp_data
    return  train_data, test_data, test_target