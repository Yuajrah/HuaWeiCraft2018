#include "date_utils.h"
#include "predict.h"
#include "AR.h"
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
    //北京1987-2014人口: 35
    std::vector<double> test_data = {871.5,897.1,904.3,919.2,935.0,950.0,965.0,981.0,1028.0,1047.0,1061.0,1075.0,
                   1086.0,1094.0,1102.0,1112.0,1125.0,1251.1,1259.4,1240.0,1245.6,1257.2,1363.6,
                   1385.1,1423.2,1456.4,1492.7,1538.0,1601.0,1676.0,1771.0,1860.0,1961.9,2018.6,2069.3
    };
    // std::vector<double> test_data = {2,3,4,3,7};

    AR ar_model = AR(test_data);
    ar_model.fit();
    std::vector<double> res = ar_model.predict(4);

    printf("最佳滞后阶：best_p = %d", ar_model.best_p);
    printf("\n\nauto_cov size：size = %d\n", ar_model.auto_cov.size());
    for (auto a: ar_model.auto_cov) {
        printf("%f ", a);
    }
    printf("\n\nauto_cor size：size = %d\n", ar_model.auto_cor.size());
    for (auto a: ar_model.auto_cor) {
        printf("%f ", a);
    }
    printf("\n\nbias_cor size：size = %d\n", ar_model.bias_cor.size());
    for (auto a: ar_model.bias_cor) {
        printf("%f ", a);
    }
    printf("\n\naic size：size = %d\n", ar_model.aic.size());
    for (auto a: ar_model.aic) {
        printf("%f ", a);
    }

	// 需要输出的内容
	char * result_file = (char *)"17\n\n0 8 0 20";

	// 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
	write_result(result_file, filename);
}
