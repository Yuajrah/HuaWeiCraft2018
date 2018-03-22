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
#include "lib_io.h"
#include "data_format_change.h"
#include "ARIMAModel.h"


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

    int debug = 0;
    std::map<int, std::vector<Double>> train_data;
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
    }




    /**
     * 第一版预测方案
     */
//    std::map<int, int> predict_data;
//    for (auto &t: vm_info) {
//        std::vector<Double> after_ma_data = ma(train_data[t.first], 7);
//        AR ar_model(after_ma_data);
//        ar_model.fit("aic");
//        // ar_model.fit("aic");
//        ar_model.predict(7);
//        // ar_model.print_model_info();
//        predict_data[t.first] = ar_model.get_sum();
//    }

    /**
     * 第二版预测方法
     */

//    std::map<int, int> predict_data;
//    for (auto &t: vm_info) {
//        ARIMAModel* arima = new ARIMAModel(ma(train_data[t.first], 5));
//
//        int period = 1;
//        int modelCnt=50;
//        int cnt=0;
//        std::vector<std::vector<int>> list;
//        std::vector<int> tmpPredict(modelCnt);
//
//        for (int k = 0; k < modelCnt; ++k)			//控制通过多少组参数进行计算最终的结果
//        {
//            std::vector<int> bestModel = arima->getARIMAModel(period, list, (k == 0) ? false : true);
//            //std::cout<<bestModel.size()<<std::endl;
//
//            if (bestModel.size() == 0)
//            {
//                tmpPredict[k] = (int)train_data[t.first][train_data[t.first].size() - period];
//                cnt++;
//                break;
//            }
//            else
//            {
//                //std::cout<<bestModel[0]<<bestModel[1]<<std::endl;
//                for (int jj=0;jj<7;jj++) {
//                    int predictDiff = arima->predictValue(bestModel[0], bestModel[1], period);
//                    //std::cout<<"fuck"<<std::endl;
//                    tmpPredict[k] += arima->aftDeal(predictDiff, period);
//                }
//                cnt++;
//            }
//            std::cout<<bestModel[0]<<" "<<bestModel[1]<<std::endl;
//            list.push_back(bestModel);
//        }
//
//        double sumPredict = 0.0;
//        for (int k = 0; k < cnt; ++k)
//        {
//            sumPredict += ((double)tmpPredict[k])/(double)cnt;
////        printf("%f ", sumPredict);
//        }
//        int predict = (int)std::round(sumPredict);
//
//        predict_data[t.first] = std::max(predict, 0);
//    }


    /**
     * 第三版预测方案
     */

    int diff_day = 1;


    std::map<int, int> predict_data;
    for (auto &t: vm_info) {
        std::vector<Double> after_ma_data = ma(train_data[t.first], 6);
        std::vector<double> after_diff_data = preDealDiff(after_ma_data, 1);
        AR ar_model(after_ma_data);
        ar_model.fit("none");
        // ar_model.fit("aic");
        ar_model.predict(7);
        // ar_model.print_model_info();
        predict_data[t.first] = ar_model.get_sum();
    }
    print_predict_score(actual_data, predict_data);
    std::vector<std::map<int,int>> allocate_result;
    allocate_result = frist_fit(vm_info, server, predict_data, opt_object);


    std::string result1 = change_map_char(predict_data);
    std::string result2 = change_format(allocate_result);
    std::string result = result1+result2;
	// 需要输出的内容
	char * result_file = (char *)"17\n\n0 8 0 20";
	// 直接调用输出文件的方法输出到指定文件中（ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开）
	write_result(result.c_str(), filename);
}
