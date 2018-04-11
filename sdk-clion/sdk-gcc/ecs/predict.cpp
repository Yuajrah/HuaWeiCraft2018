#include "date_utils.h"
#include "predict.h"
#include "AR.h"
#include "math_utils.h"
#include "test.h"
#include <cstdio>
#include "get_data.h"
#include "type_def.h"
#include "ar.h"
#include "ma.h"
#include <map>
#include "frist_fit.h"
#include "packing.h"
#include "lib_io.h"
#include "data_format_change.h"
#include "ARIMAModel.h"
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
#include "SA.h"
#include "noise.h"

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

    /**
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
     */


    char date_start[20];
    sscanf(data[0], "%*s %*s %s", &date_start); // 获取esc文本数据的开始日期
    strcat(date_start, " 00:00:00");

    printf("date_start = %s\n", date_start);

    int need_predict_day = get_days(forecast_start_date, forecast_end_date); // 要预测的天数

    BasicInfo::need_predict_day = need_predict_day;
    BasicInfo::need_predict_cnt = BasicInfo::need_predict_day * 24 / BasicInfo::split_hour;

    int debug = 5;


    std::map<int, std::vector<double>> train_data; // 用于最终训练模型的训练数据

    std::map<int, std::vector<double>> fit_train_data; // 拟合阶段所用的训练集合
    std::map<int, int> fit_test_data;  // 拟合阶段的测试集合
    std::map<int, std::vector<double>> fit_test_data_everyday; // 拟合阶段的测试集合, 包含每天的数据

    std::map<int, int> actual_data;
    // 项目可执行文件的参数： "../../../../data/exercise/date_2015_01_to_2015_05.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
    // 项目可执行文件的参数： "../../../../data/exercise/data_2015_12_to_2016_01.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
    if (debug == 0) { // 上传所用
        train_data = get_esc_data(data, date_start, forecast_start_date, data_num);
        actual_data = get_sum_data(data, forecast_start_date, forecast_end_date, data_num);
    } else if (debug == 1) {
        train_data = get_esc_data(data, date_start, "2015-05-24", data_num);
        actual_data = get_sum_data(data, "2015-05-24", "2015-05-31", data_num);
    } else if (debug == 2) { // 16年的数据集
        train_data = get_esc_data(data, date_start, "2016-01-21 00:00:00", data_num);
        actual_data = get_sum_data(data, "2016-01-21 00:00:00", "2016-01-28 00:00:00", data_num);
    } else if (debug == 3) {
        train_data = get_esc_data(data, date_start, forecast_start_date, data_num); // 用于最终训练模型的训练数据

        char *test_start_date = add_days(forecast_start_date, -need_predict_day); // 选取最后×天, x天为所需要预测的天数
        fit_train_data = get_esc_data(data, date_start, test_start_date, data_num);
        fit_test_data = get_sum_data(data, test_start_date, forecast_start_date, data_num);
    } else if (debug == 4) {
        train_data = get_esc_data(data, date_start, "2015-05-24", data_num); // 用于最终训练模型的训练数据

        char *test_start_date = add_days("2015-05-24", -7); // 选取最后×天, x天为所需要预测的天数
        fit_train_data = get_esc_data(data, date_start, test_start_date, data_num);
        fit_test_data = get_sum_data(data, test_start_date, forecast_start_date, data_num);
        actual_data = get_sum_data(data, "2015-05-24", "2015-05-31", data_num);
    } else if (debug == 5) {
        // 拟合阶段所用的训练集合
        train_data = get_esc_data(data, date_start, forecast_start_date, data_num); // 用于最终训练模型的训练数据

        char *test_start_date = add_days(forecast_start_date, -need_predict_day * 2); // 选取最后×天, x天为所需要预测的天数
        fit_train_data = get_esc_data(data, date_start, test_start_date, data_num);
        fit_test_data = get_sum_data(data, test_start_date, forecast_start_date, data_num);
        fit_test_data_everyday = get_esc_data(data, test_start_date, forecast_start_date, data_num);

    } else if (debug == 6) {
        // 拟合阶段所用的训练集合
        train_data = get_esc_data(data, date_start, "2016-01-21 00:00:00", data_num); // 用于最终训练模型的训练数据

        char *test_start_date = add_days("2016-01-21", -need_predict_day); // 选取最后×天, x天为所需要预测的天数
        fit_train_data = get_esc_data(data, date_start, test_start_date, data_num);
        fit_test_data_everyday = get_esc_data(data, test_start_date, "2016-01-21 00:00:00", data_num);
        actual_data = get_sum_data(data, "2016-01-21 00:00:00", "2016-01-28 00:00:00", data_num);
    }


//    if (data_num == 1690 && BasicInfo::vm_info.size() == 5 && BasicInfo::is_cpu()) { // 用例
//
//    } else{
//        char * result_file = (char *)"17\n\n0 8 0 20";
//        // 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
//        write_result(result_file, filename);
//        return;
//    }


    /*************************************************************************
    *****  去噪声 **************************************************************
    **************************************************************************/

//    train_data = remove_noise_1th(train_data);

    /*************************************************************************
    *****  预测  **************************************************************
    **************************************************************************/

//    std::map<int, int> predict_data = predict_by_ar_1th (BasicInfo::vm_info, train_data, need_predict_day);
////
//    print_predict_score(actual_data, predict_data);


    /*
     * 使用knn进行预测
     */
    //std::map<int, int> predict_data = predict_by_knn(BasicInfo::vm_info, train_data, need_predict_day);
//    std::map<int, int> predict_data = predict_by_knn_method2(BasicInfo::vm_info, train_data, need_predict_day);
//    print_predict_score(actual_data, predict_data);
//    std::string result1 = format_predict_res(predict_data);

    /*
    * 使用决策树进行预测
    * 有问题
    */
//    std::map<int, int> predict_data = predict_by_cart(BasicInfo::vm_info, train_data, need_predict_day);
//    print_predict_score(actual_data, predict_data);
    /*
    * 使用随机森林进行预测
    * 有问题
    */
//    std::map<int, int> predict_data = predict_by_randomForest(BasicInfo::vm_info, train_data, need_predict_day);
//    //std::map<int, int> predict_data = predict_by_randomForest_method2(BasicInfo::vm_info, train_data, need_predict_day);
//    print_predict_score(actual_data, predict_data);
//    std::string result1 = format_predict_res(predict_data);


    /**
     * 使用svm进行预测
     */

//    std::map<int, int> predict_data = predict_by_svm(train_data);
//    print_predict_score(actual_data, predict_data);

    /**
     * 用残差做预测
     */

    std::map<int, int> predict_data = predict_by_ar_7th(fit_train_data, fit_test_data_everyday, train_data);
    print_predict_score(actual_data, predict_data);

    /*************************************************************************
    *****  分配  **************************************************************
    **************************************************************************/

    /**
     * 第一版分配方式
     * ffd
     */
//    std::vector<int> order;
//    order = get_order(BasicInfo::vm_info, BasicInfo::server_info, BasicInfo::opt_object);
//    std::vector<std::map<int,int>> allocate_result = frist_fit(BasicInfo::vm_info, BasicInfo::server_info, predict_data, BasicInfo::opt_object,order );
//    std::string result2 = format_allocate_res(allocate_result);
    /**
     * 第二版分配方式
     * 背包
     */


    std::vector<std::map<int,int>> allocate_result = packing(BasicInfo::vm_info, BasicInfo::server_info, predict_data, BasicInfo::opt_object);
    std::string result2 = format_allocate_res(allocate_result);



    /**
     * 第三版分配方式
     * 纯ff
     */

//    std::vector<Vm> objects = serialize(predict_data);
//    random_permutation(objects);
//    std::vector<Bin> allocate_result = ff({}, objects, server_info);
//    std::string result2 = format_allocate_res(allocate_result);
    /**
     * 第四版分配方式
     * ffd+
     */
//    FFD ffd(BasicInfo::vm_info,2,predict_data);
//    std::vector<Bin> allocate_result = ffd.FFD_Dot();
//    std::string result2 = format_allocate_res(allocate_result);
//    get_scores_f(predict_data, BasicInfo::server_info, allocate_result.size());


//    std::vector<std::map<int,int>> allocate_result = FFD_Dot(BasicInfo::vm_info, BasicInfo::server_info, predict_data, BasicInfo::opt_object,2);
//    std::string result2 = format_allocate_res(allocate_result);

    /**
     * 第五版分配方式
     * 遗传算法测试
     */


//    std::vector<std::map<int,int>> packing_result = packing(BasicInfo::vm_info, BasicInfo::server_info, predict_data, BasicInfo::opt_object);
//    std::vector<Bin> bins;
//    int cnt = 0;
//    for (auto &server: packing_result) {
//        Bin bin(BasicInfo::server_info.core, BasicInfo::server_info.mem);
//        for (auto &vm: server) {
//            Vm t_vm = BasicInfo::vm_info[vm.first];
//            for (int i=0;i<vm.second;i++) {
//                t_vm.no = cnt++;
//                t_vm.type = vm.first;
//                bin.put(t_vm);
//            }
//        }
//        bins.push_back(bin);
//    }
//
//
//
//    std::vector<Vm> objects = serialize(predict_data);
//    int pop_size = 100;
//    int cross_num = 40;
//    double p_mutation = 0.15;
//    int mutation_num = 5;
//    int inversion_num = 10;
//    int iter_num = 8000;
//    GGA gga(objects, pop_size, cross_num, p_mutation, mutation_num, inversion_num, iter_num);
//    gga.initial(bins, 100);
////    gga.initial({}, 0);
//    gga.start();
//    std::vector<Bin> allocate_result = gga.get_best_chrome().get_bin();
//
//    std::vector<std::pair<int, Vm>> order_vm_info(BasicInfo::vm_info.begin(), BasicInfo::vm_info.end());
//    std::sort(order_vm_info.begin(), order_vm_info.end(), [](const std::pair<int, Vm>& a, const std::pair<int, Vm>& b) {
//        return a.second.mem > b.second.mem;
//    });
//    after_process(allocate_result, order_vm_info, predict_data);
//    std::string result2 = format_allocate_res(allocate_result);

    /**
     * 第六版分配, 目前坠吼
     * 对最后的分配结果进行进一步的处理, 填充新的服务器
     */
//    std::vector<std::map<int,int>> packing_result = packing(BasicInfo::vm_info, BasicInfo::server_info, predict_data, BasicInfo::opt_object);
//    std::vector<Bin> allocate_result = vector_res_to_bins_res(packing_result);
//
//    std::vector<std::pair<int, Vm>> order_vm_info(BasicInfo::vm_info.begin(), BasicInfo::vm_info.end());
//
//    std::sort(order_vm_info.begin(), order_vm_info.end(), [](const std::pair<int, Vm>& a, const std::pair<int, Vm>& b) {
//        return a.second.mem > b.second.mem;
//    });
//
//    after_process(allocate_result, order_vm_info, predict_data);
//    std::string result2 = format_allocate_res(allocate_result);

    /**
     * 第六版分配
     * 模拟退火
     */

//    std::vector<Vm> objects = serialize(predict_data);
//    double t0 = 100.0;
//    double t_min = 1;
//    double r = 0.9999;
//    SA sa(objects, t0, t_min, r);
//    sa.start();
//    std::vector<Bin> allocate_result = sa.get_best_solution();
//
//    std::vector<std::pair<int, Vm>> order_vm_info(BasicInfo::vm_info.begin(), BasicInfo::vm_info.end());
//    std::sort(order_vm_info.begin(), order_vm_info.end(), [](const std::pair<int, Vm>& a, const std::pair<int, Vm>& b) {
//        return a.second.mem > b.second.mem;
//    });
//    after_process(allocate_result, order_vm_info, predict_data);
//    std::string result2 = format_allocate_res(allocate_result);


    std::string result1 = format_predict_res(predict_data);
    std::string result = result1+result2;

    // 需要输出的内容
    char * result_file = (char *)"17\n\n0 8 0 20";
    // 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
    write_result(result.c_str(), filename);
    //0分答案
    //write_result(result_file, filename);
}
