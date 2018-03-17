#include "date_utils.h"
#include "predict.h"
#include "AR.h"
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
 * */
//你要完成的功能总入口
// info 是inputFile的数据，data是历史数据

void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{

    /*
     * 先处理input_file中的数据
     */
    std::map<int, Vm> vm_info; // key为type的编号,value为对应的信息
    Server server;
    int type_num; // vm的类型

    sscanf(info[0],"%d %d %d",&server.core, &server.storage, &server.disk);
    sscanf(info[2],"%d",&type_num);
	for (int i=3;i<3+type_num;i++) {
        int type, cores, storage;
		sscanf(info[i],"flavor%d %d %d",&type,&cores,&storage);
        vm_info[type] = {cores, storage};
	}

/*    for (auto info: vm_info) {
        printf("flavor%d %d %d\n", info.first, info.second.core, info.second.storage);
    }*/

    char *opt_object = info[4+type_num]; // 所要优化的目标，CPU或者MEM

    char forecast_start_date[10]; // 实际预测开始日期为该天，实际历史数据结束日期为该天的前一天
    sscanf(info[6+type_num], "%s", forecast_start_date);
    char forecast_end_date[10]; // 实际预测结束日期为该日期前一天
    sscanf(info[7+type_num], "%s", forecast_end_date);

    char date_start[11];
    sscanf(data[0], "%*s %*s %s", &date_start);
    std::map<int, std::vector<Double>> train_data = get_esc_data(data, date_start, forecast_start_date, vm_info, data_num);

    AR ar_model(train_data[13]);
    ar_model.fit("none_and_least_square");
    ar_model.predict(get_days(forecast_start_date, forecast_end_date));
    ar_model.print_model_info();

/*    for (int i=0;i<train_data.size();i++) {
        printf("\n");
        for (int j=0;j<train_data[i].size();j++) {
            printf("%f ", train_data[i][j]);
        }
    }*/

	// 需要输出的内容
	char * result_file = (char *)"17\n\n0 8 0 20";

	// 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
	write_result(result_file, filename);
}
