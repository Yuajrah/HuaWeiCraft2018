//
// Created by ruiy on 18-4-18.
//

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
    alpha[0] = 1 / alpha[0];
    alpha[1] = 1 / alpha[1];

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
    beta[0] = 1 / beta[0];
    beta[1] = 1 / beta[1];

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
 * 如果is_priority为true, 则 size = max{ r[j] * alpha[j] }, 其中j = 1..d, 论文里面只给了alpha的情况, 没有提beta
 *
 * 这里由于维度只有2个, 所以直接加权求和即可
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