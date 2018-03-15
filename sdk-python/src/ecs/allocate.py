# -*- coding: utf-8 -*-
import heapq

#使用frist_fit方法进行分配
def frist_fit(server_data, flavor_data, target_resource, need_allocate):
    #need data使用优先队列保存服务器信息
    #[
    #(1, ['flavor15', 7]), (2, ['flavor14', 5])
    #]
    #heapq默认使用最小堆，通过16减进行转化
    #此处原则是优先满足需要资源比较多的，即ID越靠后其在优先队列中的排序越靠前
    need_data = []
    change_base = 16
    for flavor_type, need_number in need_allocate.iteritems():
        order = change_base - int(flavor_type[6:])
        heapq.heappush(need_data,(order, [flavor_type, need_number]))
    
    #分配的结果
    result_record = []
    allocate_result = []
    #数据格式 
    #{
    #   (oder, {服务器编号：id, CPU:剩余， MEM: 剩余})
    #}
    server_number = 1
    #首先放进一个空的物理服务器
    heapq.heappush(allocate_result,(server_data[target_resource], {"id" : server_number, "CPU":server_data["CPU"], 
                                                                    "MEM":server_data["MEM"]}))
    #循环进行处理.直到所有的虚拟服务器都被满足
    while (len(need_data) > 0):
        data_record = heapq.heappop(need_data)
        flavor_type = data_record[1][0]
        CPU_need = flavor_data[flavor_type]["CPU"]
        MEM_need = flavor_data[flavor_type]["MEM"]
        ##进行分配服务器的过程
        ##首先遍历已有服务器，看看是否能放下，如果都不能放下就申请新的服务器
        
        #是否需要申请新服务器的标志
        need_allocate_new_server = False
        tmp_allocate_result = []
        while (len(allocate_result) > 0):
            pass
        
        allocate_result = merge_allocate([allocate_result, tmp_allocate_result])
        if need_allocate_new_server:
           server_number += 1
           #初始分配的时候将服务器的CPU和MEM减去对应的值
           tmp_server = {}
           tmp_server["CPU"] = server_data["CPU"] - CPU_need
           tmp_server["MEM"] = server_data["MEM"] - MEM_need
           heapq.heappush(allocate_result,(tmp_server[target_resource], {"id" : server_number, "CPU":tmp_server["CPU"], 
                                                                    "MEM":tmp_server["MEM"]}))
        #分配完成后将需要分配的虚拟机数量减一
        data_record[1][1] -= 1
        #如果需要分配的数量不为0，就继续加入优先队列
        if (data_record[1][1] > 0):
            heapq.heappush(need_data,(data_record[0],data_record[1]))
            
def merge_allocate(data1, data2, target_resource):
        while (len(data2) > 0):
            tmp_data = heapq.heappop(data2)
            heapq.heappush(data1,tmp_data)
        return data1
