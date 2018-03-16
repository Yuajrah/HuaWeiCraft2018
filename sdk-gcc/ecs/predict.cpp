#include "date_utils.h"
#include "predict.h"
#include "AR.h"
#include "test.cpp"
#include <cstdio>

/*
 *   ecsDataPath = "../../../data/exercise/date_2015_01_to_2015_05.txt"
 *   inputFilePath = "../../../data/exercise/input_file.txt"
 *   resultFilePath = "../../../data/exercise/output_file.txt"
 *   项目可执行文件的参数： "../../../data/exercise/date_2015_01_to_2015_05.txt" "../../../data/exercise/input_file.txt" resultFilePath = "../../../data/exercise/output_file.txt"
 * */
//你要完成的功能总入口
// info 是inputFile的数据，data是历史数据
/*struct Server {
	cores, // 核数
	storage, // 内存大小
	disk // 硬盘大小
};*/
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
/*	Server server;
    *//*int days = get_days("2013-03-15", "2013-03-16");
    printf("%d\n", days);*//*
    sscanf(info[0],"%d %d %d",&server.cores, &server.storage, &server.disk_size);
    sscanf(info[2],"%d",&type_num);
	for (int i=3;i<3+type_num;i++) {
		sscanf(info[i],"flavor%d %d %d",&type,&cores,&day);
	}*/
    test_ar();

	// 需要输出的内容
	char * result_file = (char *)"17\n\n0 8 0 20";

	// 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
	write_result(result_file, filename);
}
