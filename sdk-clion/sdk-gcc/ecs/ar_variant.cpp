//
// Created by ruiy on 18-3-29.
//

#include "ar_variant.h"
#include "BasicInfo.h"
#include <functional>
#include <algorithm>

/**
 * 基于ar改动, 第一版
 */

std::map<int, int> predict_by_ar_1th (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day) {
    std::map<int, int> predict_data;
    for (auto &t: vm_info) {
        std::vector<double> after_ma_data = ma(train_data[t.first], 6);
        AR ar_model(after_ma_data);
        ar_model.fit("none");
        // ar_model.fit("aic");
        ar_model.predict(need_predict_day);
        // ar_model.print_model_info();
        auto predict_res = ar_model.get_res();
        predict_data[t.first] = std::max(round(accumulate(predict_res.begin(), predict_res.end(), -0.5)), 0.0) * 1.1;
    }
    return predict_data;
}

/**
* 基于ar改动, 第二版
*/
std::map<int, int> predict_by_ar_2th (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day) {

    std::map<int, int> predict_data;
    for (auto &t: vm_info) {

        int tmp_sum = 0;
        for (int jj=0;jj<need_predict_day;jj++) {
            ARIMAModel* arima = new ARIMAModel(ma(train_data[t.first], 5));

            int period = 1;
            int modelCnt = 5;
            std::vector<int> tmpPredict(modelCnt);
            std::vector<std::vector<int>> list;
            int cnt=0;

            for (int k = 0; k < modelCnt; ++k)			//控制通过多少组参数进行计算最终的结果
            {
                std::vector<int> bestModel = arima->getARIMAModel(period, list, (k == 0) ? false : true);
                //std::cout<<bestModel.size()<<std::endl;

                if (bestModel.size() == 0)
                {
                    tmpPredict[k] = (int)train_data[t.first][train_data[t.first].size() - period];
                    cnt++;
                    break;
                }
                else
                {
                    //std::cout<<bestModel[0]<<bestModel[1]<<std::endl;

                    int predictDiff = arima->predictValue(bestModel[0], bestModel[1], period);
                    //std::cout<<"fuck"<<std::endl;
                    tmpPredict[k] = arima->aftDeal(predictDiff, period);
                    cnt++;
                }
                std::cout<<bestModel[0]<<" "<<bestModel[1]<<std::endl;
                list.push_back(bestModel);
            }
            double sumPredict = 0.0;

            for (int k = 0; k < cnt; ++k)
            {
                sumPredict += ((double)tmpPredict[k])/(double)cnt;
//        printf("%f ", sumPredict);
            }

            int predict = (int)std::round(sumPredict);
            tmp_sum += predict;
            printf("predict = %d ", predict);
            train_data[t.first].push_back(predict);
        }

        predict_data[t.first] = tmp_sum;
    }
    return predict_data;
}


/**
* 基于ar改动, 第三版
*/
std::map<int, int> predict_by_ar_3th (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day) {

    int diff_day = 10;
    std::map<int, int> predict_data;
    for (auto &t: vm_info) {
        std::vector<double> after_ma_data = ma(train_data[t.first], 6);
        std::vector<double> after_diff_data = do_diff(after_ma_data, diff_day);
        AR ar_model(after_diff_data);
        ar_model.fit("none");
        // ar_model.fit("aic");
        ar_model.predict(need_predict_day, diff_day);
        // ar_model.print_model_info();
        std::vector<double> ar_res = ar_model.get_res();
        std::vector<double> predict_res = reset_diff(after_ma_data, diff_day, ar_res);
        predict_data[t.first] = round(accumulate(predict_res.begin(), predict_res.end(), 0.0));
    }
    return predict_data;
}

/**
     * 基于ar改动, 第四版
     */
std::map<int, int> predict_by_ar_4th(std::map<int, Vm> vm_info, std::map<int, std::vector<double>> fit_train_data, std::map<int, int> fit_test_data, std::map<int, std::vector<double>> train_data, int need_predict_day){

    std::map<int, int> predict_data;
    for (auto &t: vm_info) {
        int best_sum = INT32_MAX;
        int best_window_size = 0;
        for (int window_size = 1;window_size<20;window_size++) {
            std::vector<double> after_ma_data = ma(fit_train_data[t.first], window_size);
            AR ar_model(after_ma_data);
            ar_model.fit("aic");
            // ar_model.fit("aic");
            ar_model.predict(need_predict_day);
            // ar_model.print_model_info();
            std::vector<double> tmp_predict_res = ar_model.get_res();
            int tmp_predict_sum = round(accumulate(tmp_predict_res.begin(), tmp_predict_res.end(), 0.0));
            if (abs(best_sum - fit_test_data[t.first]) >  abs(tmp_predict_sum - fit_test_data[t.first])) {
                best_sum = tmp_predict_sum;
                best_window_size = window_size;
            }
        }

        std::vector<double> after_ma_data = ma(train_data[t.first], best_window_size);
        AR ar_model(after_ma_data);
        ar_model.fit("aic");
        ar_model.predict(need_predict_day);
        std::vector<double> tmp_predict_res = ar_model.get_res();
        predict_data[t.first] = round(accumulate(tmp_predict_res.begin(), tmp_predict_res.end(), 0.0));
    }
    return predict_data;
}

/**
 * 基于AR改动, 第五版
 */

std::map<int, int> predict_by_ar_5th(std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day){
    std::map<int, int> predict_data;
    for (auto &t: vm_info) {
        std::vector<double> after_ma_data = ma(train_data[t.first], 6);
        std::vector<int> exist_p;
        int mean_cnt = 20;
        double s = 0;
        for (int i=0;i<mean_cnt;i++) {
            AR ar_model(after_ma_data);
            ar_model.fit("aic", -1, exist_p);

            exist_p.push_back(ar_model.get_p());

            ar_model.predict(need_predict_day);
            auto predict_res = ar_model.get_res();
            s += accumulate(predict_res.begin(), predict_res.end(), 0.0);
        }
        predict_data[t.first] = round(s / double(mean_cnt));

        // ar_model.fit("aic");
        // ar_model.print_model_info();
    }
    return predict_data;
}

std::map<int, int> predict_by_ar_6th(std::map<int, std::vector<double>> train_data, int need_predict_day){
    std::map<int, int> predict_data;
    for (auto &t: BasicInfo::vm_info) {
        predict_data[t.first] = std::max(0.0, round(std::accumulate(train_data[t.first].end()-need_predict_day, train_data[t.first].end(), 0.0)));
    }
    return predict_data;
}