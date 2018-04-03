//
// Created by ruiy on 18-4-3.
//

#include <cfloat>
#include "SA.h"
#include "Random.h"
#include "Bin.h"
#include "ff.h"
#include "BasicInfo.h"

SA::SA(std::vector<Vm> objects, double t0, double t_min, double r):
        objects(objects),
        t0(t0),
        t_min(t_min),
        r(r)
{}

void SA::start() {
    double t = t0;
    double min_f = DBL_MAX;
    while (t > t_min) {

        if (BasicInfo::is_stop()) break;

        // 随机交换两物体顺序
        int rnd1 = Random::random_int(0, objects.size()-1);
        int rnd2 = -1;
        do{rnd2 = Random::random_int(0, objects.size()-1);}while(rnd2 == rnd1);

        // 生成一个新的解决方案
        std::vector<Vm> new_objects(objects);
        std::swap(new_objects[rnd1], new_objects[rnd2]);

        // ff方法放置物体
        std::vector<Bin> bins = ff({}, new_objects);

        // 计算目标函数
        double f;
        if (BasicInfo::is_cpu()) {
            double last_bin_cpu_usage_rate = 1 - bins.rbegin()->cores / static_cast<double>(BasicInfo::server_info.core);
            f = bins.size() - 1 + last_bin_cpu_usage_rate;
        } else {
            double last_bin_mem_usage_rate = 1 - bins.rbegin()->mems / static_cast<double>(BasicInfo::server_info.mem);
            f = bins.size() - 1 + last_bin_mem_usage_rate;
        }

        // 如果更优, 则替换, 否则按概率替换
        if (f < min_f) {
            min_f = f;
            best_solution = bins;
            objects = new_objects;
        } else {
            if (exp(min_f - f) / t > Random::random_double(0, 1)) {
                min_f = f;
                best_solution = bins;
                objects = new_objects;
            }
        }

        // 温度降低
        t = r * t;

    }
}
