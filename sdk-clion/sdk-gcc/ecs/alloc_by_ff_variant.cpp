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

//    for (auto &server: BasicInfo::server_infos) {
//        bins_max_num[server.first] = 2;
//    }

    std::vector<Bin> bins;
    for (auto &t: bins_max_num) {
        for (int i=0;i<t.second;i++) {
            bins.push_back(Bin(t.first, BasicInfo::server_infos[t.first].core, BasicInfo::server_infos[t.first].mem));
        }
    }

    bool flag = mrp_bfd_bin_centric(bins, objects);
    return bins;
}
