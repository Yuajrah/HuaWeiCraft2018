//
// Created by ruiy on 18-3-29.
//

#include "ar_variant.h"
#include "BasicInfo.h"
#include <functional>
#include <algorithm>

#include "type_def.h"
#include "ml_predict.h"

std::map<int, int> predict_by_ar_1th (std::map<int, Vm> vm_info, std::map<int, std::vector<double>> train_data, int need_predict_day) {
    std::map<int, int> predict_data;
    for (auto &t: vm_info) {
        std::vector<double> after_ma_data = ma(train_data[t.first], 6);
        //after_ma_data = split_high(after_ma_data,4);
        AR ar_model(after_ma_data);
        ar_model.fit("none");
        // ar_model.fit("aic");
        ar_model.predict(need_predict_day);
        // ar_model.print_model_info();
        auto predict_res = ar_model.get_res();
        predict_data[t.first] = std::max(round(accumulate(predict_res.begin(), predict_res.end(), 0.0)), 0.0);
    }
    return predict_data;
}

