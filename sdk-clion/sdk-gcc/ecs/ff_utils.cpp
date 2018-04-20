//
// Created by ruiy on 18-4-18.
//

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
            bins.push_back(Bin(2, BasicInfo::server_infos[2].core, BasicInfo::server_infos[2].mem)); // 打开一个新箱子
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

    std::vector<double> param_alphas = calc_alpha(bins); // 计算加权系数
    calc_bin_size(bins, param_alphas); // 更新bin的size

    std::sort(objects.begin(), objects.end(), [](const Vm &o1, const Vm &o2){
        return o1.size > o2.size;
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
 * mrp(machine reassignment problem) 中的 best fit decreasing, 面向object(即item)的方法 item centric
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
            return o1.size < o2.size;;
        });

        if (!put_bins(objects.back(), bins)) { // 挑选最大size的object放入当前箱子序列, 如果无法放入, 则return false
            return false;
        }
        objects.pop_back();
    }
    return true;
};

/**
 * mrp(machine reassignment problem) 中的 best fit decreasing, 面向bin的方法 bin centric
 * @param bins
 * @param objects
 * @return
 */
bool mrp_bfd_bin_centric(std::vector<Bin> &bins, std::vector<Vm> objects) {
    std::vector<Bin> res;
    while (!bins.empty()) {

        if (objects.empty()) break;

        std::vector<double> param_alphas = calc_alpha(bins); // 计算加权系数

        calc_bin_size(bins, param_alphas); // 根据加权系数, 计算各个箱子的size
        int index = get_min_size_bin(bins);


        while (!objects.empty()) {
            calc_object_size(objects, param_alphas); // 根据加权系数, 计算各个object的size
            std::sort(objects.begin(), objects.end(), [](const Vm &o1, const Vm &o2){ // 将object按照size从小到大排列
                return o1.size > o2.size;
            });

            bool is_allocated = false;
            for (int i=0;i<objects.size();i++) {
                is_allocated = bins[index].put(objects[i]);
                if(is_allocated == true){
                    objects.erase(objects.begin() + i);
                    break;
                }
            }
            if (is_allocated == false) {
                break;
            }
        }

        res.push_back(bins[index]);
        bins.erase(bins.begin() + index);

    }

    bins.assign(res.begin(), res.end());

    return objects.empty();
};

/**
 * 计算加权系数alpha(j), j = 1..d
 * 该加权系数用于将多个维度资源变成一个维度资源
 *
 * j = 1 表示 cpu维度, j = 2 表示 mem维度
 *
 * C(j) = 各个箱子j维度剩余资源之和
 * alpha(j) =  1 / C(j)
 *
 * 由于这里只有两个资源, 那么系数只有 alpha(1) 和 alpha(2)
 *
 * todo
 * 注: 该接口与FFD.cpp中的getA方法重复
 *
 * @param bins
 * @return
 */
std::vector<double> calc_alpha(const std::vector<Bin> &bins){
    std::vector<double> alpha(2, 0.0);
    for (const Bin &bin: bins) {
        alpha[0] += bin.cores;
        alpha[1] += bin.mems;
    }
    alpha[0] = 1.0 / alpha[0];
    alpha[1] = 1.0 / alpha[1];

    return alpha;
}

/**
 * 计算加权系数beta(j), j = 1..d
 * 该加权系数用于将多个维度资源变成一个维度资源
 *
 * j = 1 表示 cpu维度, j = 2 表示 mem维度
 *
 * R(j) = 各个物体j维度剩余资源之和
 * beta(j) =  1 / R(j)
 *
 * 由于这里只有两个资源, 那么系数只有 beta(1) 和 beta(2)
 *
 *
 * @param objects
 * @return
 */
std::vector<double> calc_beta(const std::vector<Vm> &objects){
    std::vector<double> beta(2, 0.0);
    for (const Vm &vm: objects) {
        beta[0] += vm.core;
        beta[1] += vm.mem;
    }
    beta[0] = 1.0 / beta[0];
    beta[1] = 1.0 / beta[1];

    return beta;
}

/**
 * 计算加权系数
 * 该系数为 alpha[j] / beta[j]
 *
 * 通过bins计算 加权系数 alpha[1..d], 此处 d = 2
 * 通过objects计算 加权系数 beta[1..d], 此处 d = 2
 *
 * @param bins
 * @param objects
 * @return
 */
std::vector<double> calc_alpha_div_beta(const std::vector<Bin> &bins, const std::vector<Vm> &objects){
    std::vector<double> alpha = calc_alpha(bins);
    std::vector<double> beta = calc_beta(objects);

    std::vector<double> res;
    for (int i=0;i<alpha.size();i++) {
        res.push_back(alpha[i] / beta[i]);
    }

    return res;
}

/**
 *
 * 计算objects的'size', 该size是对不同维度资源做加权得到, 即将多维度降到一维度
 * 加权系数有多种, 这个通过外界传入, 之所以由外界传入, 是因为这个系数可能由箱子计算得到, 也可能由物体计算得到, 封装在一起, 反而显得杂乱
 *
 * 这里由于维度只有2个, 所以直接加权求和即可
 *
 * 如果is_priority为true, 则 size = max{ r[j] * alpha[j] }, 其中j = 1..d, 论文里面只给了alpha的情况, 没有提beta
 *
 * @param objects
 * @param params 加权系数, alpha 或者 belta 或者 alpha / belta
 * @param is_priority 默认为false, 如果为true, 则选出最突出的那个维度的资源代表size
 */
void calc_object_size(std::vector<Vm> &objects, const std::vector<double> &params, bool is_priority) {
    for (Vm &vm: objects) {

        if (is_priority == false) {
            vm.size = params[0] * vm.core + params[1] * vm.mem;
        } else {
            vm.size = std::max(params[0] * vm.core, params[1] * vm.mem);
        }

    }

}

/**
 *
 * 计算bin的'size', 该size是对不同维度资源做加权得到, 即将多维度降到一维度
 * 加权系数有多种, 这个通过外界传入, 之所以由外界传入, 是因为这个系数可能由箱子计算得到, 也可能由物体计算得到, 封装在一起, 反而显得杂乱
 *
 * 这里由于维度只有2个, 所以直接加权求和即可
 *
 * 如果is_priority为true, 则 size = max{ r[j] * alpha[j] }, 其中j = 1..d, 论文里面只给了alpha的情况, 没有提beta
 *
 * @param objects
 * @param params 加权系数, alpha 或者 belta 或者 alpha / belta
 * @param is_priority 默认为false
 */
void calc_bin_size(std::vector<Bin> &bins, const std::vector<double> &params, bool is_priority) {
    for (Bin &bin: bins) {
        if (is_priority == false) {
            bin.size = params[0] * bin.cores + params[1] * bin.mems;
        } else {
            bin.size = std::max(params[0] * bin.cores, params[1] * bin.mems);
        }
    }
}


/**
 * 将object放入箱子序列中, 若可以放入, 则放入, 返回true; 若不能放入, 则返回false
 * @param object
 * @param bins
 * @return
 */
bool put_bins(const Vm &object, std::vector<Bin> &bins){
    for (Bin &bin: bins) {
        if (bin.put(object)) return true;
    }
    return false;
}

/**
 * 从bins中返回size最小的那个bin的索引
 * @param bins
 * @return
 */
int get_min_size_bin(const std::vector<Bin> &bins){
    int t_min_size = INT32_MAX;
    int index;
    for (int i=0;i<bins.size();i++) {
        if (bins[i].size < t_min_size) {
            t_min_size = bins[i].size;
            index = i;
        }
    }
    return index;
}

/**
 * 从bins中返回size最大的那个bin的索引
 * @param bins
 * @return
 */
int get_max_size_bin(const std::vector<Bin> &bins){
    int t_max_size = 0;
    int index;
    for (int i=0;i<bins.size();i++) {
        if (bins[i].size > t_max_size) {
            t_max_size = bins[i].size;
            index = i;
        }
    }
    return index;
}

/**
 * 判断一个箱子序列是否为可行解
 * @param bins
 * @return
 */
bool is_feasible(std::vector<Bin> &bins){
    for (Bin &bin: bins) {
        if (!bin.is_feasible()) return false;
    }
    return true;
}