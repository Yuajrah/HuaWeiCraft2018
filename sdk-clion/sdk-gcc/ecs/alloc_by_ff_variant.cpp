//
// Created by ruiy on 18-4-19.
//

#include <map>
#include <cmath>
#include "alloc_by_ff_variant.h"
#include "type_def.h"
#include "BasicInfo.h"
#include "Bin.h"
#include "ff_utils.h"
#include "math_utils.h"

/**
 * 计算三个箱子可能的上限, 并分配物体到其中
 */
std::vector<Bin> alloc_by_ff_variant_1th (std::vector<Vm> objects) {
    double core_demand_sum = 0.0;
    double mem_demand_sum = 0.0;
    for (Vm &vm: objects) {
        core_demand_sum += vm.core;
        mem_demand_sum += vm.mem;
    }

    std::map<int, int> bins_max_num;
    for (auto &server: BasicInfo::server_infos) {
        bins_max_num[server.first] = std::ceil(std::max(core_demand_sum / server.second.core  , mem_demand_sum / server.second.mem));
    }

    double max_score = 0;
    std::vector<Bin> best_solution;
    std::vector<Bin> bins;
    for (int i=0;i<bins_max_num[0];i++) {
        for (int j=0;j<bins_max_num[1];j++) {
            for (int k=0;k<bins_max_num[2];k++) {
                bins.clear();
                std::vector<Bin> bins1(i, Bin(0, BasicInfo::server_infos[0].core, BasicInfo::server_infos[0].mem));
                std::vector<Bin> bins2(j, Bin(1, BasicInfo::server_infos[1].core, BasicInfo::server_infos[1].mem));
                std::vector<Bin> bins3(k, Bin(2, BasicInfo::server_infos[2].core, BasicInfo::server_infos[2].mem));
                bins.insert(bins.end(), bins1.begin(), bins1.end());
                bins.insert(bins.end(), bins2.begin(), bins2.end());
                bins.insert(bins.end(), bins3.begin(), bins3.end());

                bool flag = mrp_ffd(bins, objects);
                if (flag) {
                    double score = calc_alloc_score(bins);
                    if (max_score < score) {
                        best_solution = bins;
                        max_score  = score;
                    }
                }
            }
        }
    }

//    std::vector<Bin> bins;
//    for (auto &t: bins_max_num) {
//        for (int i=0;i<t.second;i++) {
//            bins.push_back(Bin(t.first, BasicInfo::server_infos[t.first].core, BasicInfo::server_infos[t.first].mem));
//        }
//    }

//    bool flag = mrp_bfd_bin_centric(bins, objects);
    return best_solution;
}
