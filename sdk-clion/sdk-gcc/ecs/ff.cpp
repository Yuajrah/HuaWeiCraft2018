//
// Created by ruiy on 18-3-30.
//

#include "ff.h"
#include "BasicInfo.h"
#include "ff_utils.h"

/**
 *
 * 将若干物体(objects) 放入 配置为server_info的箱子中去
 * 其中objects的类型为vm, 每个元素代表一个vm, 格式为 [ vm1, vm4, vm2, vm1, vm1, vm3] 并无顺序要求
 *
 * @param bins 初始状态的箱子, 如果初始状态没有箱子, 则传入参数{}即可
 * @param objects 需要放入的物体
 * @return 返回装箱结果
 */
std::vector<Bin> ff(std::vector<Bin> bins, std::vector<Vm> objects) {
    for (auto &object: objects) {
        bool is_allocated = false; // 用于判断现有箱子是否可以满足分配需求
        for (auto &bin: bins) {
            is_allocated = bin.put(object);
            if (is_allocated) break;
        }
        if (is_allocated == false) { // 若现有箱子无法满足分配需求, 则新开一个箱子
            bins.push_back(Bin(BasicInfo::server_info.core, BasicInfo::server_info.mem)); // 打开一个新箱子
            bins.back().put(object);
        }
    }
    return bins;
}

/**
 * mrp(machine reassignment problem) 中的 first fit
 * 分配物体到指定箱子, 如果成功, 则返回true;如果装不下, 则返回false
 *
 * 注:
 * 1. 这里如果装不下, 并不会开辟新箱子
 * 2. 这里 bins 为 引用 传参
 *
 * @param bins 初始状态的箱子, 如果初始状态没有箱子, 则传入参数{}即可
 * @param objects 需要放入的物体
 * @return true表示装下; false表示未装下;
 */
bool mrp_ff(std::vector<Bin> &bins, std::vector<Vm> objects){
    for (auto &object: objects) {
        bool is_allocated = false; // 用于判断现有箱子是否可以满足分配需求
        for (auto &bin: bins) {
            is_allocated = bin.put(object);
            if (is_allocated) break;
        }
        if (is_allocated == false) return false;
    }
    return true;
}

/**
 * mrp(machine reassignment problem) 中的 first fit decreasing
 * 分配物体到指定箱子, 如果成功, 则返回true;如果装不下, 则返回false
 *
 * 注:
 * 1. 这里如果装不下, 并不会开辟新箱子
 * 2. 这里 bins 为 引用 传参
 * 3. 这里根据objects的size属性作从大到小排序, 然后调用mrp_ff
 *
 * @param bins 初始状态的箱子, 如果初始状态没有箱子, 则传入参数{}即可
 * @param objects 需要放入的物体
 * @return true表示装下; false表示未装下;
 */
bool mrp_ffd(std::vector<Bin> &bins, std::vector<Vm> objects){
    std::sort(objects.begin(), objects.end(), [](const Vm &o1, const Vm &o2){
        return o1.size >= o2.size;
    });
    return mrp_ff(bins, objects);
}

/**
 * mrp(machine reassignment problem) 中的 best fit
 * 分配物体到指定箱子序列, 如果成功, 则返回true;如果装不下, 则返回false
 *
 * 注:
 * 1. 这里如果装不下, 并不会开辟新箱子
 * 2. 这里 bins 为 引用 传参
 *
 * @param bins 初始状态的箱子, 如果初始状态没有箱子, 则传入参数{}即可
 * @param objects 需要放入的物体
 * @param params 计算size需要的参数, 之所以让外界传入, 是因为放在里面会导致重复的计算
 * @return true表示装下; false表示未装下;
 */
bool mrp_bf(std::vector<Bin> &bins, std::vector<Vm> objects){
    for (auto &object: objects) {
        std::vector<double> param_alphas = calc_alpha(bins); // 计算加权系数
        calc_bin_size(bins, param_alphas); // 更新bin的size
        std::sort(bins.begin(), bins.end(), [](const Bin &o1, const Bin &o2){ // 按size从小到大排列
            return o1.size < o2.size;
        });

        if (!put_bins(object, bins)) {
            return false;
        }
    }
    return true;
}

/**
 * mrp(machine reassignment problem) 中的 best fit decreasing, 目标对象为object(即item)时
 * @param bins
 * @param objects
 * @return
 */
bool mrp_bfd_item_centric(std::vector<Bin> &bins, std::vector<Vm> objects) {
    while (!objects.empty()) {
        std::vector<double> param_alphas = calc_alpha(bins); // 计算加权系数
        calc_object_size(objects, param_alphas); // 根据加权系数, 计算各个object的size
        std::sort(objects.begin(), objects.end(), [](const Vm &o1, const Vm &o2){ // 将object按照size从小到大排列
            return o1.size < o2.size;
        });

        calc_bin_size(bins, param_alphas); // 根据加权系数, 计算各个箱子的size
        std::sort(bins.begin(), bins.end(), [](const Bin &o1, const Bin &o2){ // 将object按照size从小到大排列
            return o1.size < o2.size;
        });

        if (!put_bins(objects.back(), bins)) { // 挑选最大size的object放入当前箱子序列, 如果无法放入, 则return false
            return false;
        }
        objects.pop_back();
    }
    return true;
};

/**
 * mrp(machine reassignment problem) 中的 best fit decreasing, 目标对象为bin时
 * @param bins
 * @param objects
 * @return
 */
bool mrp_bfd_bin_centric(std::vector<Bin> &bins, std::vector<Vm> objects) {
    std::vector<Bin> res;
    while (!bins.empty()) {
        std::vector<double> param_alphas = calc_alpha(bins); // 计算加权系数

        calc_bin_size(bins, param_alphas); // 根据加权系数, 计算各个箱子的size

        int index = get_min_size_bin(bins);

        calc_object_size(objects, param_alphas); // 根据加权系数, 计算各个object的size
        std::sort(objects.begin(), objects.end(), [](const Vm &o1, const Vm &o2){ // 将object按照size从小到大排列
            return o1.size > o2.size;
        });


        if (objects.empty()) return true;

        std::vector<Vm> t_objects(objects);
        int j=0;
        for (int i=0;i<objects.size();i++) {
            if(bins[index].put(objects[i])){
                t_objects.erase(t_objects.begin() + j);
                continue;
            }
            j++;
        }
        objects = t_objects;

        res.push_back(bins[index]);
        bins.erase(bins.begin() + index);

    }

    return objects.empty();
};