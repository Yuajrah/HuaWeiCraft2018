# -*- coding: utf-8 -*-
import heapq

#根据需要优化的资源，将所有的服务器分成两类，一种是大于等于服务器的
def spilt_data(server_data, flavor_data, target_resource, need_allocate):
    base = 0.0
    frist_allocate = {}
    second_allocate = {}
    frist_resource = "CPU"
    second_resource = "MEM"
    if target_resource == second_resource:
        frist_resource = "MEM"
        second_resource = "CPU"
        
    base = (server_data[frist_resource]+0.0) / server_data[second_resource]
    for i in range(15):
        current_flavor = "flavor"+str(i+1)
        current_rate = (flavor_data[current_flavor][frist_resource]+0.0)/flavor_data[current_flavor][second_resource]
        if base <= current_rate:
            frist_allocate[current_flavor] = need_allocate[current_flavor]
        else:
            second_allocate[current_flavor] = need_allocate[current_flavor]
    return frist_allocate,second_allocate    
#使用frist_fit方法进行分配
def frist_fit(server_data, flavor_data, target_resource, need_allocate, allocate_result=[], result_record=[], order = {}):
    #need data使用优先队列保存服务器信息
    #[
    #(1, ['flavor15', 7]), (2, ['flavor14', 5])
    #]
    #heapq默认使用最小堆，通过16减进行转化
    #此处原则是优先满足需要资源比较多的，即ID越靠后其在优先队列中的排序越靠前
    need_data = []
    change_base = 16
    if (len(order) == 0):
        for flavor_type, need_number in need_allocate.iteritems():
            tmp_order = change_base - int(flavor_type[6:])
            heapq.heappush(need_data,(tmp_order, [flavor_type, need_number]))
    else:
        for flavor_type, need_number in need_allocate.iteritems():
            tmp_order = order[flavor_type]
            heapq.heappush(need_data,(tmp_order, [flavor_type, need_number]))
    
    #数据格式 
    #{
    #   (oder, {服务器编号：id, CPU:剩余， MEM: 剩余})
    #}
    server_number = 0
    #如果为空首先放进一个空的物理服务器
    if len(allocate_result) == 0:
        heapq.heappush(allocate_result,[server_data[target_resource], {"id" : server_number, "CPU":server_data["CPU"], 
                                                                    "MEM":server_data["MEM"]}])
        result_record.append([])
    #循环进行处理.直到所有的虚拟服务器都被满足
    while (len(need_data) > 0):
        data_record = heapq.heappop(need_data)
        flavor_type = data_record[1][0]
        CPU_need = flavor_data[flavor_type]["CPU"]
        MEM_need = flavor_data[flavor_type]["MEM"]
        ##进行分配服务器的过程
        ##首先遍历已有服务器，看看是否能放下，如果都不能放下就申请新的服务器
        
        #是否需要申请新服务器的标志
        need_allocate_new_server = True
        tmp_allocate_result = []
        
        #循环遍历服务器
        while (len(allocate_result) > 0):
            current_server_data = heapq.heappop(allocate_result)
            if current_server_data[1]["CPU"] >= CPU_need and current_server_data[1]["MEM"] >= MEM_need:
                current_server_data[1]["CPU"] -= CPU_need
                current_server_data[1]["MEM"] -= MEM_need
                #元祖不可改变
                current_server_data[0] = current_server_data[1][target_resource]
                result_record[current_server_data[1]["id"]].append(flavor_type)
                heapq.heappush(tmp_allocate_result, current_server_data)
                need_allocate_new_server = False
                break
            else:
                heapq.heappush(tmp_allocate_result, current_server_data)
                
        
        allocate_result = merge_allocate(allocate_result, tmp_allocate_result)
        if need_allocate_new_server:
           server_number += 1
           #初始分配的时候将服务器的CPU和MEM减去对应的值
           tmp_server = {}
           tmp_server["CPU"] = server_data["CPU"] - CPU_need
           tmp_server["MEM"] = server_data["MEM"] - MEM_need
           heapq.heappush(allocate_result,[tmp_server[target_resource], {"id" : server_number, "CPU":tmp_server["CPU"], 
                                                                    "MEM":tmp_server["MEM"]}])
           result_record.append([])
           result_record[server_number].append(flavor_type)
        #分配完成后将需要分配的虚拟机数量减一
        data_record[1][1] -= 1
        #如果需要分配的数量不为0，就继续加入优先队列
        if (data_record[1][1] > 0):
            heapq.heappush(need_data,(data_record[0],data_record[1]))
    print "预测需要的服务器数量："
    print len(result_record)
    allocate_server_number = len(result_record)
    get_scores(allocate_server_number, server_data, flavor_data, need_allocate, target_resource)
    return allocate_result,result_record
 
#合并两个data           
def merge_allocate(data1, data2):
        while (len(data2) > 0):
            tmp_data = heapq.heappop(data2)
            heapq.heappush(data1,tmp_data)
        return data1

def weight_frist_fit(server_data, flavor_data, target_resource, need_allocate, allocate_result=[], result_record=[], order={}):
    frist_allocate, second_allocate = spilt_data(server_data, flavor_data, target_resource, need_allocate)
    print "第一次分配资源利用率"
    allocate_result,result_record = frist_fit(server_data, flavor_data, target_resource, frist_allocate, allocate_result= [], result_record= [])
    print "无效数据："
    allocate_result,result_record = frist_fit(server_data, flavor_data, target_resource, second_allocate, allocate_result, result_record)
    print "最后的资源利用率"
    get_scores(len(result_record), server_data, flavor_data, need_allocate, target_resource)

def ordered_frist_fit(server_data, flavor_data, target_resource, need_allocate):
    need_order = {}
    if target_resource == "CPU":
        for i in range(1,16):
            current_flavor = "flavor"+str(i)
            if i%3==0:
                need_order[current_flavor] = i-2
            elif i%3 == 1:
                need_order[current_flavor] = i+2
            else:
                need_order[current_flavor] = [i]
    else:
        for i in range(1,16):
            current_flavor = "flavor"+str(i)
            need_order[current_flavor] = i
    frist_fit(server_data, flavor_data, target_resource, need_allocate, allocate_result=[], result_record=[], order= need_order)
            
        
        
#得到关于该种资源的利用率
#参数     allocate_server_number : 最后分配需要的服务器个数
#        server_data: 服务器信息
#        flavor_data: 虚拟服务器信息
#        need_allocate: 预测结果中需要分配的信息
#        target_resource: 需要优化的资源
def get_scores(allocate_server_number, server_data, flavor_data, need_allocate, target_resource):
    all_allocate_resource = allocate_server_number * server_data[target_resource]
    need_allocate_resource = 0.0
    for flavor_type, need_number in need_allocate.iteritems():
        need_allocate_resource += flavor_data[flavor_type][target_resource] * need_number
    print target_resource+"资源利用率："
    print need_allocate_resource/all_allocate_resource

#观察每个服务器的资源使用情况
def show_data(allocate_server_number, server_data, flavor_data, need_allocate, target_resource):
    pass
    