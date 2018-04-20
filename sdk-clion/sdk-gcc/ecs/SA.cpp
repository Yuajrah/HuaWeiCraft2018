//
// Created by ruiy on 18-4-20.
//

#include "SA.h"
#include "math_utils.h"
#include "Random.h"
#include "ff_utils.h"
#include <algorithm>

SA::SA(std::vector<Bin> initial_bins, double alpha, double beta)
        :initial_bins(initial_bins), alpha(alpha), beta(beta){}

double SA::calc_cost(const std::vector<Bin> &bins) {
    return 1 - calc_alloc_score(bins);
}

/**
 * 根据论文里面的说法, 得到bins的一堆领域
 * @param bins
 * @return
 */
std::vector<std::vector<Bin>> SA::one_way_transfer(const std::vector<Bin> &bins) {
    std::vector<std::vector<Bin>> neighborhood_states;
    for (int i=0;i<bins.size();i++) {
        for (int j=0;j<bins[i].objects.size();j++) { // 遍历bins中所有物体, 将物体放入其他箱子中去

            // 如果物体有N个, 箱子有B个, 那么对应有N*(B-1)个领域状态
            for (int k=0;k<bins.size();k++) {
                if (k == i) { // 如果即将要放入的箱子还是原来的, 则continue;
                   continue;
                }
                std::vector<Bin> neighborhood_state(bins);
                neighborhood_state[k].put_force(bins[i].objects[j]);
                neighborhood_state[i].remove(j);
                neighborhood_states.push_back(neighborhood_state);
            }
        }
    }
    return neighborhood_states;
}

double SA::calc_t0() {
    double initial_c = calc_cost(initial_bins);
    std::vector<std::vector<Bin>> neighborhood_states;

    int increase_cnt = 0;
    double increase_sum = 0;
    int decrease_cnt = 0;
    for (const std::vector<Bin> &neighborhood_state: neighborhood_states) {
        double delta_c= calc_cost(neighborhood_state) - initial_c;
        if (delta_c > 0) {
            increase_cnt++;
            increase_sum += delta_c;
        } else {
            decrease_cnt++;
        }
    }

    double acceptance_ratio = 0.90;
    double t0 = - increase_sum / std::log((acceptance_ratio * (increase_cnt + decrease_cnt) - decrease_cnt ) / increase_cnt);
}

void SA::start() {
    double t0 = calc_t0();
    double t = t0;
    std::vector<Bin> S(initial_bins);
    best_solution.assign(S.begin(), S.end());
    while (true) {

        if (BasicInfo::is_stop()) break;

        std::vector<std::vector<Bin>> neighborhood_states = one_way_transfer(S);
        int rnd = Random::random_int(0, neighborhood_states.size()-1);
        double delta_c = calc_cost(neighborhood_states[rnd]) - calc_cost(S);
        if (delta_c < 0) {
            S.assign(neighborhood_states[rnd].begin(), neighborhood_states[rnd].end());
            if (is_feasible(S)) {
                best_solution.assign(S.begin(), S.end());
            }
        } else {
            if (Random::random_double(0, 1) <= std::exp(-delta_c / t)) {
                S.assign(neighborhood_states[rnd].begin(), neighborhood_states[rnd].end());
            }
        }
    }
}
