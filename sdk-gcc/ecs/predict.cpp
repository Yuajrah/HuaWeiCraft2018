#include "date_utils.h"
#include "predict.h"
#include "AR.h"
#include "math_utils.h"
#include "test.h"
#include <cstdio>
#include "get_data.h"
#include "type_def.h"
#include <map>

/*
 *   ecsDataPath = "../../../data/exercise/date_2015_01_to_2015_05.txt"
 *   inputFilePath = "../../../data/exercise/input_file.txt"
 *   resultFilePath = "../../../data/exercise/output_file.txt"
 *   项目可执行文件的参数： "../../../data/exercise/date_2015_01_to_2015_05.txt" "../../../data/exercise/input_file.txt" resultFilePath = "../../../data/exercise/output_file.txt"
 *   项目可执行文件的参数： "../../../data/exercise/data_2015_12_to_2016_01.txt" "../../../data/exercise/input_file.txt" resultFilePath = "../../../data/exercise/output_file.txt"
 *
 * */
//你要完成的功能总入口
// info 是inputFile的数据，data是历史数据

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

    Server server;
    sscanf(info[0],"%d %d %d",&server.core, &server.storage, &server.disk); // 获取server的基本信息

    int type_num;
    sscanf(info[2],"%d",&type_num); // 获取共有type_num种类型的vm

	for (int i=3;i<3+type_num;i++) {
        int type, cores, storage;
		sscanf(info[i],"flavor%d %d %d",&type,&cores,&storage);
        vm_info[type] = {cores, storage}; // 获取各种vm的基本信息（包括 核心数和内存大小）
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

    std::map<int, std::vector<Double>> train_data = get_esc_data(data, date_start, "2016-01-21", vm_info, data_num);
    std::map<int, int> actual_data = get_sum_data(data, "2016-01-21", "2016-01-28", vm_info, data_num);


    std::map<int, int> predict_data;
    for (auto &t: vm_info) {
        AR ar_model(train_data[t.first]);
        ar_model.fit("aic");
        // ar_model.fit("aic");
        ar_model.predict(7);
        ar_model.print_model_info();
        predict_data[t.first] = ar_model.get_sum();
    }

    print_predict_score(actual_data, predict_data);
	// 需要输出的内容
	char * result_file = (char *)"17\n\n0 8 0 20";

	// 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
	write_result(result_file, filename);
}
