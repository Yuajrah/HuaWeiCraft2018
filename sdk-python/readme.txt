1. SDK_Python代码说明：
   我们已经提供了保姆式的服务，你只需要做：
   完成predictor.py文件中的XXX方法。
   SDK已经实现了读取文件、按要求格式写文件以及打印开始和结束时间的功能。为了便于调试，SDK将读入的信息全部在屏幕输出，可根据自身的需要酌情删除此打印信息。
   注意：读取文件功能是指，将图的信息文件和路径信息文件按行读取到内存，其在内存中的存储格式仍是字符串格式。因为这些信息以什么格式存储涉及到算法设计，这样做是为了不禁锢你的思路。

2. Python 版本要求为： python 2.7.5

3. 运行时只需要执行： python ecs.py /xxx/TrainData.txt /xxx/input.txt /xxx/output.txt
	(说明：TrainData.txt是历史数据文件，input.txt是其他参数输入文件，output.txt是输出文件)

4. 打包上传文件需要包括文件路径：
   src/ecs/ecs.py
   src/ecs/predictor.py
   如有你自己定义的py文件也可添加， 保证能引用得到。可直接修改SDK_Python为自己名字，打成tar.gz包上传即可，如zhangsan.tar.gz。
   
5 预测方法
    - AR模型
    - 移动平均后套用AR模型
    - 均值
    - 如果数据量较小，就直接均值，否则AR模型(待实现)

6 部署方案
    - firstfit模型
        将已经分配的物理服务器按照小端在前的优先队列，从占用资源多的虚拟服务器开始分配，如果已经分配的物理服务器都不满足就开辟新的物理服务器 
  