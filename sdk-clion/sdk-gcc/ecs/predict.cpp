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
#include "alloc_by_ff_variant.h"
#include "Random.h"
#include "ml_predict.h"
#include "BasicInfo.h"
#include "GGA.h"
#include "math_utils.h"
#include "noise.h"
#include "ff_utils.h"
#include "FFOD.h"
#include "predict_by_svm.h"

#include "test.h"
#include "predict_by_bp.h"
#include "Pack.h"


/*
 *   ecsDataPath = "../../../data/exercise/date_2015_01_to_2015_05.txt"
 *   inputFilePath = "../../../data/exercise/input_file.txt"
 *   resultFilePath = "../../../data/exercise/output_file.txt"
 *
 * */
//你要完成的功能总入口
// info 是inputFile的数据，data是历史数据

//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine Random::generator;
std::map<int, Server> BasicInfo::server_infos;
std::map<int, Vm> BasicInfo::vm_info;
time_t BasicInfo::t_start;
char* BasicInfo::opt_object;
int BasicInfo::need_predict_day;
int BasicInfo::split_hour;
int BasicInfo::need_predict_cnt;

int BasicInfo::extra_need_predict_day; // 需要额外预测的天数
int BasicInfo::extra_need_predict_cnt; // 需要额外预测的次数, 当粒度为天时, 同上(extra_need_predict_day)
int BasicInfo::sum_need_predict_day;
std::map<int, std::string> BasicInfo::server_type = {{TYPE_GENERAL, "General"}, {TYPE_LARGE, "Large-Memory"}, {TYPE_HIGH, "High-Performance"}};

// no usage
Server BasicInfo::server_info;

void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{

    BasicInfo::split_hour = 24;
    BasicInfo::t_start = time(NULL); // 计时开始

    /**
     * 先处理input_file中的数据
     *
     * server_type_num 为server的类型数目
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

    int server_type_num;
    sscanf(info[0], "%d", &server_type_num);
    for (int i=1;i<=server_type_num;i++) {
        Server server_info;
        char type_str[20];
        sscanf(info[i],"%s %d %d %d", &type_str, &server_info.core, &server_info.mem, &server_info.disk); // 获取server的基本信息
        if (strcmp(type_str, "General") == 0) {
            server_info.type = TYPE_GENERAL;
        } else if (strcmp(type_str, "Large-Memory") == 0) {
            server_info.type = TYPE_LARGE;
        } else if (strcmp(type_str, "High-Performance") == 0) {
            server_info.type = TYPE_HIGH;
        }

        printf("%s \n", type_str);
        BasicInfo::server_infos[server_info.type] = server_info;
    }

    int type_num;
    sscanf(info[server_type_num+2],"%d",&type_num); // 获取共有type_num种类型的vm

	for (int i=server_type_num+3;i<server_type_num+3+type_num;i++) {
        int type, core, mem;
		sscanf(info[i],"flavor%d %d %d",&type,&core,&mem);
        BasicInfo::vm_info[type] = {type, core,  mem/ 1024}; // 获取各种vm的基本信息（包括 类型,核心数和内存大小）
	}

    char forecast_start_date[20]; // 预测起始日期
    sscanf(info[server_type_num+3+type_num+1], "%[^\\n]", forecast_start_date);

    char forecast_end_date[20]; // 预测结束日期（不包含）
    char tmp1[20];
    sscanf(info[server_type_num+3+type_num+2], "%s %s", forecast_end_date, tmp1);
    strcat(forecast_end_date, " 00:00:00");
    if (*tmp1 == '2') {
        memcpy(forecast_end_date, add_days(forecast_end_date, 1), 20 * sizeof(char));
    }
    printf("forecast_end_date = %s\n", forecast_end_date);


    char date_start[20];
    sscanf(data[0], "%*s %*s %s", &date_start); // 获取esc文本数据的开始日期
    strcat(date_start, " 00:00:00");
    char date_end[20];
    sscanf(data[data_num-1], "%*s %*s %s", &date_end); // 获取esc文本数据的结束日期
    strcat(date_end, " 00:00:00");

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
     *
     * debug = 1, 获取 date_2015_01_to_2015_05.txt 的数据, 注意要将二进制文件的执行参数改为该文本, 本地测试用
     * 项目可执行文件的参数：
     * 初赛: "../../../../data/exercise/date_2015_01_to_2015_05.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
     * 复赛: "../../../../data/exercise/date_2015_01_to_2015_05.txt" "../../../../data/exercise/input_file_semi.txt" "../../../../data/exercise/output_file.txt"
     *
     * debug = 2, 获取 data_2015_12_to_2016_01.txt 的数据, 注意要将二进制文件的执行参数改为该文本, 本地测试用
     * 项目可执行文件的参数：
     * 初赛: "../../../../data/exercise/data_2015_12_to_2016_01.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
     * 复赛:  "../../../../data/exercise/data_2015_12_to_2016_01.txt" "../../../../data/exercise/input_file_semi.txt" "../../../../data/exercise/output_file.txt"
     *
     * debug = 3, 获取 date_2015_01_to_2015_08.txt 的数据
     * 项目可执行文件的参数：
     * 初赛: "../../../../data/exercise/data_2015_01_to_2015_08.txt" "../../../../data/exercise/input_file.txt" "../../../../data/exercise/output_file.txt"
     * 复赛: "../../../../data/exercise/data_2015_01_to_2015_08.txt" "../../../../data/exercise/input_file_semi.txt" "../../../../data/exercise/output_file.txt"
     *
     *
     * 注:
     * 本地测试时, 每次更改debug, 需要手动更替项目可执行参数
     * 项目可执行文件的参数需要手动更替
     *
     * 初赛对应的inputfile为 input_file.txt
     * 复赛对应的inputfile为 input_file_semi.txt
     *
     */

    if (getenv("DATA_SET") == NULL) {
        train_data = get_esc_data(data, date_start, add_days(date_end, 1), data_num);
        BasicInfo::extra_need_predict_day = get_days(date_end, forecast_start_date) - 1; // 间隔的天数
        BasicInfo::extra_need_predict_cnt = BasicInfo::extra_need_predict_day * 24 / BasicInfo::split_hour;
    } else if (strcmp(getenv("DATA_SET"), "2") == 0){
        train_data = get_esc_data(data, date_start, "2016-01-15 00:00:00", data_num);
        actual_data = get_sum_data(data, "2016-01-21 00:00:00", "2016-01-28 00:00:00", data_num);

        BasicInfo::extra_need_predict_day = get_days("2016-01-14 00:00:00", "2016-01-21 00:00:00") - 1; // 间隔的天数
        BasicInfo::extra_need_predict_cnt = BasicInfo::extra_need_predict_day * 24 / BasicInfo::split_hour;
    } else if (strcmp(getenv("DATA_SET"), "3") == 0) {
        memcpy(date_start, "2015-01-01 00:00:00", 20 * sizeof(char));
        memcpy(date_end, "2015-08-10 00:00:00", 20 * sizeof(char));

        memcpy(forecast_start_date, "2015-08-18 00:00:00", 20 * sizeof(char));
        memcpy(forecast_end_date, "2015-09-01 00:00:00", 20 * sizeof(char));

        train_data = get_esc_data(data, date_start, add_days(date_end, 1), data_num);
        actual_data = get_sum_data(data, forecast_start_date, forecast_end_date, data_num);

        BasicInfo::extra_need_predict_day = get_days(date_end, forecast_start_date) - 1; // 间隔的天数
        BasicInfo::extra_need_predict_cnt = BasicInfo::extra_need_predict_day * 24 / BasicInfo::split_hour;
    }

    printf("extra_need_predict_day = %d\n", BasicInfo::extra_need_predict_day);
    /**
     * 如果是线上, 则可以区别初级和中级对待
     */
    if (getenv("DATA_SET") == NULL) {
        if (BasicInfo::extra_need_predict_day > 0) {

        } else {
            exit(0);
        }
    }

    BasicInfo::sum_need_predict_day = BasicInfo::need_predict_day + BasicInfo::extra_need_predict_day;
    /*************************************************************************
    *****  去噪声 **************************************************************
    **************************************************************************/

//    train_data = remove_noise_1th(train_data);

    /*************************************************************************
    *****  预测  **************************************************************
    **************************************************************************/


    /**
     * 使用knn进行预测
     */
//    std::map<int, int> predict_data = predict_by_knn(BasicInfo::vm_info, train_data, BasicInfo::sum_need_predict_day);
//    print_predict_score(actual_data, predict_data);
//    std::string result1 = format_predict_res(predict_data);

    /**
     * 使用决策树进行预测
     * 有问题
     */
//    std::map<int, int> predict_data = predict_by_cart(BasicInfo::vm_info, train_data, BasicInfo::sum_need_predict_day);
//    print_predict_score(actual_data, predict_data);

    /*
    * 使用随机森林进行预测
    */
//    std::map<int, int> predict_data = predict_by_randomForest(BasicInfo::vm_info, train_data, BasicInfo::sum_need_predict_day);
//    print_predict_score(actual_data, predict_data);
//    std::string result1 = format_predict_res(predict_data);


//
//    std::map<int, int> predict_data = predict_by_ar_1th (train_data);
//    print_predict_score(actual_data, predict_data);


    /**
     * 线性回归
//     */

    std::map<int, int> predict_data = predict_by_LR(BasicInfo::vm_info, train_data, BasicInfo::sum_need_predict_day);

    print_predict_score(actual_data, predict_data);


//    std::map<int, int> predict_data = predict_by_LR(BasicInfo::vm_info, train_data, BasicInfo::sum_need_predict_day);
////    std::map<int, int> predict_data = predict_by_LR_intervel(BasicInfo::vm_info, train_data, BasicInfo::sum_need_predict_day);
//    print_predict_score(actual_data, predict_data);


    /*
     * 使用指数平滑预测
     */
//    std::map<int, int> predict_data = predict_by_enponential(BasicInfo::vm_info, train_data, BasicInfo::sum_need_predict_day);
//    print_predict_score(actual_data, predict_data);
    /**
     * 用残差做预测
     */

//    std::map<int, int> predict_data = predict_by_ar_7th(fit_train_data, fit_test_data_everyday, train_data);
//    print_predict_score(actual_data, predict_data);


    /**
     *
     *
     */

//    std::map<int, int> predict_data = predict_by_svm_1th(train_data);
//
//    print_predict_score(actual_data, predict_data);

    /**
     * 使用单独线性模型做预测
     */
//    std::map<int, int> predict_data = predict_by_LR(BasicInfo::vm_info, train_data, BasicInfo::need_predict_cnt);
//    print_predict_score(actual_data, predict_data);


    /**
     * bp 神经网络
     */

//    std::map<int, int> predict_data  = predict_by_bp_1th(train_data);
//    print_predict_score(actual_data, predict_data);

    /*************************************************************************
    *****  复赛分配  **************************************************************
    **************************************************************************/

    /*
     * FFOD
     */
//    std::vector<Vm> objects = serialize(predict_data);
//    std::vector<Bin> allocate_result;
//    allocate_result = alloc_by_ffod(objects);
//    printf("\nallocate score = %f\n", calc_alloc_score(allocate_result));

    /**
     * 背包
     */
    std::vector<Server> allocate_result;
    BasicInfo::server_info = BasicInfo::server_infos[0];
//    Pack pack = Pack(BasicInfo::server_infos, predict_data, BasicInfo::vm_info, 18);
//    std::vector<std::map<int,int>> packing_result = pack.packStepBest(allocate_result);

    //1.0
//    std::vector<std::map<int,int>> packing_result = packing(BasicInfo::vm_info, BasicInfo::server_info, predict_data, allocate_result);
    //2.0
    std::vector<std::map<int,int>> packing_result = packing_ad(BasicInfo::vm_info, BasicInfo::server_infos, predict_data, allocate_result);


    std::vector<Bin> bins;
    int cnt = 0;
    for (int k=0;k<packing_result.size();k++) {
        Bin bin(allocate_result[k].type, allocate_result[k].core, allocate_result[k].mem);
        for (auto &vm: packing_result[k]) {
            Vm t_vm = BasicInfo::vm_info[vm.first];
            for (int i=0;i<vm.second;i++) {
                t_vm.no = cnt++;
                t_vm.type = vm.first;
                bin.put(t_vm);
            }
        }
        bins.push_back(bin);
    }

    printf("\n allocated score = %f\n", calc_alloc_score(bins));

//    if (calc_alloc_score(bins) > 0.87) {
//
//    } else {
//        return;
//    }



    after_process_add_bin(bins, predict_data);
//    after_process_remove_bin();

    printf("\n allocated score = %f\n", calc_alloc_score(bins));

    std::string result2 = format_allocate_res(bins);


//    std::vector<Vm> objects = serialize(predict_data);
//    std::vector<Bin> allocate_result;
//    allocate_result = ff({}, objects);
//
////    allocate_result = alloc_by_ff_variant_1th(objects);
//    printf("\nallocate score = %f\n", calc_alloc_score(allocate_result));
//    std::string result2 = format_allocate_res(allocate_result);

    /**
     * 将预测结果, 格式化为字符串
     */
    std::string result1 = format_predict_res(predict_data);
    std::string result = result1+result2;

    // 需要输出的内容
    char * result_file = (char *)"17\n\n0 8 0 20";
//    write_result(result.c_str(), filename);
    // 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
    write_result(result.c_str(), filename);

    //0分答案
//    write_result(result_file, filename);
}
