#include "date_utils.h"
#include "predict.h"
#include "AR.h"
#include "math_utils.h"
#include <cstdio>
#include "get_data.h"
#include "type_def.h"
#include "ar.h"
#include "ma.h"
#include <map>
#include "packing.h"
#include "lib_io.h"
#include "data_format_change.h"
#include <numeric>
#include <cfloat>
#include <algorithm>
#include <chrono>
#include <cstring>
#include "ar_variant.h"
#include "ff.h"
#include "Random.h"
#include "ml_predict.h"
#include "BasicInfo.h"
#include "FFD.h"
#include "GGA.h"
#include "math_utils.h"


/*
 *   ecsDataPath = "../../../data/exercise/date_2015_01_to_2015_05.txt"
 *   inputFilePath = "../../../data/exercise/input_file.txt"
 *   resultFilePath = "../../../data/exercise/output_file.txt"
 *   项目可执行文件的参数： "../../../../data/exercise/date_2015_01_to_2015_05.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
 *   项目可执行文件的参数： "../../../../data/exercise/data_2015_12_to_2016_01.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
 *
 * */
//你要完成的功能总入口
// info 是inputFile的数据，data是历史数据

//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine Random::generator;
Server BasicInfo::server_info;
std::map<int, Vm> BasicInfo::vm_info;
time_t BasicInfo::t_start;
char* BasicInfo::opt_object;
int BasicInfo::need_predict_day;
int BasicInfo::split_hour;
int BasicInfo::need_predict_cnt;

void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{

    BasicInfo::split_hour = 24;
    BasicInfo::t_start = time(NULL); // 计时开始

    /**
     * 先处理input_file中的数据
     *
     * type_num为vm的类型数目
     *
     * vm_info 格式为map，key为vm类型的编号（比如flavor1编号即为1），value为Vm类，其包含核心数以及内存大小
     *
     * server为Server类的对象，保存服务器的核心数和内存大小
     *
     * opt_object，优化的目标，CPU或者MEM
     *
     * forecast_start_date，实际预测开始日期为该天，实际历史数据结束日期为该天的前一天
     * forecast_end_date， 实际预测结束日期为该日期前一天
     *
     */

    sscanf(info[0],"%d %d %d",&BasicInfo::server_info.core, &BasicInfo::server_info.mem, &BasicInfo::server_info.disk); // 获取server的基本信息

    int type_num;
    sscanf(info[2],"%d",&type_num); // 获取共有type_num种类型的vm

	for (int i=3;i<3+type_num;i++) {
        int type, core, mem;
		sscanf(info[i],"flavor%d %d %d",&type,&core,&mem);
        BasicInfo::vm_info[type] = {type, core,  mem/ 1024}; // 获取各种vm的基本信息（包括 类型,核心数和内存大小）
	}

    BasicInfo::opt_object = info[4+type_num]; // 获取优化目标

    char forecast_start_date[20]; // 预测起始日期
    sscanf(info[6+type_num], "%s", forecast_start_date);
    char forecast_end_date[20]; // 预测结束日期（不包含）
    sscanf(info[7+type_num], "%s", forecast_end_date);

    /*************************************************************************
     *
     * data_start，esc文本数据的开始日期
     * ar_model的使用：
     *      1. 传入序列, 构造对象
     *      2. 拟合, 定阶
     *      3. 预测
     *      [4]. 打印信息
     *
     *  比如： train_data[8], 表示获取flavor8的序列
     *      AR ar_model(train_data[8]);
     *      ar_model.fit("none_and_least_square");
     *      ar_model.predict(get_days(forecast_start_date, forecast_end_date));
     *      ar_model.print_model_info();
     *
     **************************************************************************/


    char date_start[20];
    sscanf(data[0], "%*s %*s %s", &date_start); // 获取esc文本数据的开始日期
    strcat(date_start, " 00:00:00");

    printf("date_start = %s\n", date_start);

    int need_predict_day = get_days(forecast_start_date, forecast_end_date); // 要预测的天数

    BasicInfo::need_predict_day = need_predict_day;
    BasicInfo::need_predict_cnt = BasicInfo::need_predict_day * 24 / BasicInfo::split_hour;




    std::map<int, std::vector<double>> train_data; // 用于最终训练模型的训练数据

    std::map<int, std::vector<double>> fit_train_data; // 拟合阶段所用的训练集合
    std::map<int, int> fit_test_data;  // 拟合阶段的测试集合
    std::map<int, std::vector<double>> fit_test_data_everyday; // 拟合阶段的测试集合, 包含每天的数据

    std::map<int, int> actual_data;


    /**
     * debug = 0, 上传代码时所用的获取数据的方法, 上传时所用
     * debug = 1, 获取 date_2015_01_to_2015_05.txt 的数据, 注意要将二进制文件的执行参数改为该文本, 本地测试用
     * debug = 2, 获取 data_2015_12_to_2016_01.txt 的数据, 注意要将二进制文件的执行参数改为该文本, 本地测试用
     */
    int debug = 2;

    if (debug == 0) { // 上传所用
        train_data = get_esc_data(data, date_start, forecast_start_date, data_num);
        actual_data = get_sum_data(data, forecast_start_date, forecast_end_date, data_num);
    } else if (debug == 1) {
        train_data = get_esc_data(data, date_start, "2015-05-24", data_num);
        actual_data = get_sum_data(data, "2015-05-24", "2015-05-31", data_num);
    } else if (debug == 2) { // 16年的数据集
        train_data = get_esc_data(data, date_start, "2016-01-21 00:00:00", data_num);
        actual_data = get_sum_data(data, "2016-01-21 00:00:00", "2016-01-28 00:00:00", data_num);
    }

    /*************************************************************************
    *****  预测  **************************************************************
    **************************************************************************/

    /**
     * ar 方法预测
     */
    std::map<int, int> predict_data = predict_by_ar_1th (BasicInfo::vm_info, train_data, need_predict_day);
    print_predict_score(actual_data, predict_data);


    /**
     * svm 方法预测
     */

//    std::map<int, int> predict_data = predict_by_svm(train_data);
//    print_predict_score(actual_data, predict_data);


    /*************************************************************************
    *****  分配  **************************************************************
    **************************************************************************/


    /**
     * 先用背包分配, 然后再作为遗传的初始化种群, 跑遗传
     *
     */


    std::vector<std::map<int,int>> packing_result = packing(BasicInfo::vm_info, BasicInfo::server_info, predict_data, BasicInfo::opt_object);

    // 背包结果的格式,和遗传的接口不一样, 所以这里进行了一些处理
    std::vector<Bin> bins;
    int cnt = 0;
    for (auto &server: packing_result) {
        Bin bin(BasicInfo::server_info.core, BasicInfo::server_info.mem);
        for (auto &vm: server) {
            Vm t_vm = BasicInfo::vm_info[vm.first];
            for (int i=0;i<vm.second;i++) {
                t_vm.no = cnt++;
                t_vm.type = vm.first;
                bin.put(t_vm);
            }
        }
        bins.push_back(bin);
    }


    std::vector<Vm> objects = serialize(predict_data);
    int pop_size = 100;
    int cross_num = 40;
    double p_mutation = 0.15;
    int mutation_num = 5;
    int inversion_num = 10;
    int iter_num = 8000;
    GGA gga(objects, pop_size, cross_num, p_mutation, mutation_num, inversion_num, iter_num);
    gga.initial(bins, 100);
    gga.start();
    std::vector<Bin> allocate_result = gga.get_best_chrome().get_bin();

    /**
     * 对vm按照mem规格从大到小排序
     */
    std::vector<std::pair<int, Vm>> order_vm_info(BasicInfo::vm_info.begin(), BasicInfo::vm_info.end());
    std::sort(order_vm_info.begin(), order_vm_info.end(), [](const std::pair<int, Vm>& a, const std::pair<int, Vm>& b) {
        return a.second.mem > b.second.mem;
    });

    /**
     * 善后处理, 找到还可以填充vm的箱子, 然后增加预测结果、填装箱子
     */
    after_process(allocate_result, order_vm_info, predict_data);

    /**
     * 将分配结果, 格式化为字符串
     */
    std::string result2 = format_allocate_res(allocate_result);


    /**
     * 将预测结果, 格式化为字符串
     */
    std::string result1 = format_predict_res(predict_data);
    std::string result = result1+result2;

    // 需要输出的内容
    char * result_file = (char *)"17\n\n0 8 0 20";
    // 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
    write_result(result.c_str(), filename);
    //0分答案
    //write_result(result_file, filename);
}
