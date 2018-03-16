#include "date_utils.h"
#include "predict.h"
#include "AR.h"
#include "test.cpp"
#include <cstdio>
#include <map>

/*
 *   ecsDataPath = "../../../data/exercise/date_2015_01_to_2015_05.txt"
 *   inputFilePath = "../../../data/exercise/input_file.txt"
 *   resultFilePath = "../../../data/exercise/output_file.txt"
 *   项目可执行文件的参数： "../../../data/exercise/date_2015_01_to_2015_05.txt" "../../../data/exercise/input_file.txt" resultFilePath = "../../../data/exercise/output_file.txt"
 * */
//你要完成的功能总入口
// info 是inputFile的数据，data是历史数据
struct Server {
	int core;// 核数
	int storage; // 内存大小
    int disk; // 硬盘大小
};
struct Vm {
    int core; // 所需核数
    int storage; // 所需内存大小
};
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

    char *opt_object = info[4+type_num];
    char *forecast_start_date = info[6+type_num]; // 实际预测开始日期为该天，实际历史数据结束日期为该天的前一天
    char *forecast_end_date = info[7+type_num]; // 实际预测结束日期为该日期前一天


    /*
     * 处理历史数据
     */
    std::map<int, std::vector<double>> history_data;
    char history_start_date[11];
    sscanf(data[0], "%*s %*s %s", &history_start_date);

    // 初始化，给每种vm分配数据存储空间（连续天）
    int history_periods_len = get_days(history_start_date, forecast_start_date);  // 历史数据长度
    printf("%d\n", history_periods_len);
    for (auto &info: vm_info) {
        history_data[info.first].assign(history_periods_len, (double)0);
    }


    for (int i=0;i<data_num;i++) {
        int type;char date[11];
        sscanf(data[i], "%*s flavor%d %s",&type, &date);
        int day_index = get_days(history_start_date, date);
        std::map<int, std::vector<double>>::iterator iter = history_data.find(type);
        if (iter != history_data.end()) {
            iter->second[day_index]++;
        }
    }

/*    for (int i=0;i<history_data.size();i++) {
        printf("\n");
        for (int j=0;j<history_data[i].size();j++) {
            printf("%f ", history_data[i][j]);
        }
    }*/

	// 需要输出的内容
	char * result_file = (char *)"17\n\n0 8 0 20";

	// 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
	write_result(result_file, filename);
}
