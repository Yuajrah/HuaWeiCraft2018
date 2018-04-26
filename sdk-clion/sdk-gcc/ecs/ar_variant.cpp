//
// Created by ruiy on 18-3-29.
//

#include "ar_variant.h"
#include "BasicInfo.h"
#include <functional>
#include <algorithm>

#include "type_def.h"
#include "ml_predict.h"
#include "noise.h"


/*************************************************************************
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
 **************************************************************************/

std::map<int, int> predict_by_ar_1th (std::map<int, std::vector<double>> train_data) {
    std::map<int, int> predict_data;
    for (auto &t: BasicInfo::vm_info) {
//        std::vector<double> after_ma_data = ma(train_data[t.first], 6);
        std::vector<double> after_ma_data = remove_noise_by_box(train_data[t.first]);
        //after_ma_data = split_high(after_ma_data,4);
        AR ar_model(after_ma_data);
//        ar_model.fit("none", BasicInfo::need_predict_cnt + BasicInfo::extra_need_predict_cnt - 1);
        ar_model.fit("none");
        // ar_model.fit("aic");
        ar_model.predict(BasicInfo::need_predict_cnt);
         ar_model.print_model_info();
        auto predict_res = ar_model.get_res();
        predict_data[t.first] = std::max(round(accumulate(predict_res.begin(), predict_res.end(), 0.0)), 0.0);
        if(predict_data[t.first] < 0){
            predict_data[t.first] = 0;
        }
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
            ar_model.fit("aic", false, -1, exist_p);

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



/**
 * 基于测试集合的残差做svm模型的训练, 然后预测残差, 再加到ar预测的结果上去
 * debug 改成5上传
 * @param vm_info
 * @param fit_train_data
 * @param fit_test_data
 * @param train_data
 * @param need_predict_day
 * @return
 */
//std::map<int, int> predict_by_ar_7th(std::map<int, std::vector<double>> fit_train_data, std::map<int, std::vector<double>> fit_test_data_every, std::map<int, std::vector<double>> train_data){
//
//    std::map<int, double> predict_residual_sum;
//    for (auto &t: BasicInfo::vm_info) {
//
//        // 训练集训练模型
//        std::vector<double> after_ma_data = fit_train_data[t.first];
//        AR ar_model(after_ma_data);
//        ar_model.fit("none");
//        // ar_model.fit("aic");
//        ar_model.predict(fit_test_data_every[t.first].size());
//        // ar_model.print_model_info();
//        auto predict_res = ar_model.get_res();
//
//        for (int i=0;i<predict_res.size();i++) {
//            predict_res[i] = fit_test_data_every[t.first][i] - predict_res[i];
//        }
//
//        int split_windows = 4;
//        bool mv_flag = false;
//        std::map<std::vector<double>, double> train_data_need = timeseries_to_supervised(predict_res, split_windows, mv_flag);
//        std::vector<std::vector<double>> train_x = get_vector_train(train_data_need);
//        std::vector<double> train_y = get_vector_target(train_data_need);
//
//        /* 2. 初始化问题*/
//        svm_problem prob = init_svm_problem(train_x, train_y);     // 打包训练样本
//        svm_parameter param = init_svm_parameter();   // 初始化训练参数
//
//        /* 3. 训练模型 */
//        svm_model* model = svm_train(&prob, &param);
//
//        /* 4. 获取所需要的特征 */
//        std::vector<double> first_predict_data = get_frist_predict_data(predict_res, split_windows, mv_flag);
//
//        /* 5. 开始预测残差 */
//        std::vector<double> predict_residual_data;
//        for(int i=0; i < BasicInfo::need_predict_cnt; i++)
//        {
//            svm_node* node = feature_to_svm_node(first_predict_data);
//            double tmp_predict = svm_predict(model, node);
//
//            /* 6. 构造新的预测所需特征 */
//            first_predict_data.erase(first_predict_data.begin());
//            first_predict_data.push_back(tmp_predict);
//
//            /* 7. 存储预测结果 */
//            predict_residual_data.push_back(tmp_predict);
//        }
//
//        // predict_residual_sum[t.first] = std::max(round(accumulate(predict_residual_data.begin(), predict_residual_data.end(), 0.0)), 0.0);
//
//        predict_residual_sum[t.first] = accumulate(predict_residual_data.begin(), predict_residual_data.end(), 0.0);
//    }
//
//
//    std::map<int, int> predict_ecs_sum;
//    for (auto &t: BasicInfo::vm_info) {
//        std::vector<double> after_ma_data = ma(train_data[t.first], 6);
//        AR ar_model(after_ma_data);
//        ar_model.fit("none");
//        // ar_model.fit("aic");
//        ar_model.predict(BasicInfo::need_predict_cnt);
//        // ar_model.print_model_info();
//        auto predict_res = ar_model.get_res();
//        predict_ecs_sum[t.first] = round(std::max(accumulate(predict_res.begin(), predict_res.end(), 0.0) + predict_residual_sum[t.first], 0.0));
//    }
//    return predict_ecs_sum;
//}
