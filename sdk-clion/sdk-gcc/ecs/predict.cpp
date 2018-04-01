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
#include "ar_variant.h"
#include "ff.h"
#include "Random.h"
#include "ml_predict.h"

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

void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
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
    std::map<int, Vm> vm_info;

    Server server_info;
    sscanf(info[0],"%d %d %d",&server_info.core, &server_info.mem, &server_info.disk); // 获取server的基本信息

    int type_num;
    sscanf(info[2],"%d",&type_num); // 获取共有type_num种类型的vm

	for (int i=3;i<3+type_num;i++) {
        int type, core, mem;
		sscanf(info[i],"flavor%d %d %d",&type,&core,&mem);
        vm_info[type] = {type, core,  mem/ 1024}; // 获取各种vm的基本信息（包括 类型,核心数和内存大小）
	}

    char *opt_object = info[4+type_num]; // 获取优化目标

    char forecast_start_date[10]; // 预测起始日期
    sscanf(info[6+type_num], "%s", forecast_start_date);
    char forecast_end_date[10]; // 预测结束日期（不包含）
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

    char date_start[11];
    sscanf(data[0], "%*s %*s %s", &date_start); // 获取esc文本数据的开始日期

    int need_predict_day = get_days(forecast_start_date, forecast_end_date); // 要预测的天数

    int debug = 0;

    std::map<int, std::vector<double>> train_data; // 用于最终训练模型的训练数据

    std::map<int, std::vector<double>> fit_train_data; // 拟合阶段所用的训练集合
    std::map<int, int> fit_test_data;  // 拟合阶段的测试集合

    std::map<int, int> actual_data;
    // 项目可执行文件的参数： "../../../../data/exercise/date_2015_01_to_2015_05.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
    // 项目可执行文件的参数： "../../../../data/exercise/data_2015_12_to_2016_01.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
    if (debug == 0) { // 上传所用
        train_data = get_esc_data(data, date_start, forecast_start_date, vm_info, data_num);
        actual_data = get_sum_data(data, forecast_start_date, forecast_end_date, vm_info, data_num);
    } else if (debug == 1) {
        train_data = get_esc_data(data, date_start, "2015-05-24", vm_info, data_num);
        actual_data = get_sum_data(data, "2015-05-24", "2015-05-31", vm_info, data_num);
    } else if (debug == 2) { // 16年的数据集
        train_data = get_esc_data(data, date_start, "2016-01-21", vm_info, data_num);
        actual_data = get_sum_data(data, "2016-01-21", "2016-01-28", vm_info, data_num);
    } else if (debug == 3) {
        train_data = get_esc_data(data, date_start, forecast_start_date, vm_info, data_num); // 用于最终训练模型的训练数据

        char *test_start_date = add_days(forecast_start_date, -need_predict_day); // 选取最后×天, x天为所需要预测的天数
        fit_train_data = get_esc_data(data, date_start, test_start_date, vm_info, data_num);
        fit_test_data = get_sum_data(data, test_start_date, forecast_start_date, vm_info, data_num);
    } else if (debug == 4) {
        train_data = get_esc_data(data, date_start, "2015-05-24", vm_info, data_num); // 用于最终训练模型的训练数据

        char *test_start_date = add_days("2015-05-24", -7); // 选取最后×天, x天为所需要预测的天数
        fit_train_data = get_esc_data(data, date_start, test_start_date, vm_info, data_num);
        fit_test_data = get_sum_data(data, test_start_date, forecast_start_date, vm_info, data_num);
        actual_data = get_sum_data(data, "2015-05-24", "2015-05-31", vm_info, data_num);
    }


    /*************************************************************************
    *****  预测  **************************************************************
    **************************************************************************/

//    std::map<int, int> predict_data = predict_by_ar_1th (vm_info, train_data, need_predict_day);
//
//    print_predict_score(actual_data, predict_data);
//    std::string result1 = format_predict_res(predict_data);

    /*
     * 使用knn进行预测
     */
    std::map<int, int> predict_data = predict_by_knn(vm_info, train_data, need_predict_day);

    print_predict_score(actual_data, predict_data);
    std::string result1 = format_predict_res(predict_data);
    /*************************************************************************
    *****  分配  **************************************************************
    **************************************************************************/

    /**
     * 第一版分配方式
     * ffd
     */
//    std::vector<int> order;
//    order = get_order(vm_info, server_info, opt_object);
//    std::vector<std::map<int,int>> allocate_result = frist_fit(vm_info, server_info, predict_data, opt_object,order );
//    std::string result2 = format_allocate_res(allocate_result);

    /**
     * 第二版分配方式
     * 背包
     */
    std::vector<std::map<int,int>> allocate_result = packing(vm_info, server_info, predict_data, opt_object);
    std::string result2 = format_allocate_res(allocate_result);

    /**
     * 第三版分配方式
     * 纯ff
     */

//    std::vector<Vm> objects = serialize(predict_data, vm_info);
//    random_permutation(objects);
//    std::vector<Bin> allocate_result = ff(objects, server_info);
//    std::string result2 = format_allocate_res(allocate_result);


    std::string result = result1+result2;
    // 需要输出的内容
    char * result_file = (char *)"17\n\n0 8 0 20";
    // 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
    write_result(result.c_str(), filename);
    //0分答案
    //write_result(result_file, filename);
}
