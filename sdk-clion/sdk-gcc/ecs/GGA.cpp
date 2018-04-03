//
// Created by ruiy on 18-3-30.
//

#include <algorithm>
#include  "GGA.h"
#include "data_format_change.h"
#include "ff.h"
#include "Random.h"

GGA::GGA(std::vector<Vm> objects, int pop_size, int cross_num, double p_mutation, int mutation_num, int inversion_num, int iter_num):
        objects(objects),
        pop_size(pop_size),
        cross_num(cross_num),
        p_mutation(p_mutation),
        mutation_num(mutation_num),
        inversion_num(inversion_num),
        iter_num(iter_num){}


void GGA::initial(std::vector<Bin> bins, int num) {
    if (num == -1) {
        num = 0;
    }
    for (int i=0;i<num;i++) {
        populations.push_back(bins);
    }
    for (int i=0;i<pop_size - num;i++) {
        /**
         * 生成物体的随机排列, 然后使用ff算法去装箱, 得到装箱结果, 利用装箱结果构造染色体(这波操作可能有点秀... :P)
         */
        Chromo chromo(ff({}, random_permutation(objects)));
        populations.push_back(chromo);
    }
}

void GGA::calc_fitness() {
    for (auto &chromo: populations) chromo.calc_fitness();
}

void GGA::calc_p() {
    double sum = 0.0;
    for (Chromo &chromo: populations) {
        sum += chromo.get_fitness();
    }

    for (Chromo &chromo: populations) {
        chromo.set_p(chromo.get_fitness() / sum);
    }
}

std::vector<Chromo> GGA::random_select(int num) {
    std::vector<Chromo> chromos;
    chromos.push_back(populations[Random::random_int(0, populations.size() - 1)]);
    return chromos;
}

void GGA::tournament_select() {
    std::vector<Chromo> children;

    for (int i=0;i<pop_size;i++) {
        std::vector<Chromo> chromos = random_select(2);
        Chromo best_chromo;
        double best_fitness = 0.0;
        for (auto chromo: chromos) {
            if (best_fitness < chromo.get_fitness()) {
                best_chromo = chromo;
                best_fitness = chromo.get_fitness();
            }
        }
        children.push_back(best_chromo);
    }
    populations.assign(children.begin(), children.end()); // 用子代替换当前这代
}


void GGA::rolette_select() {
    std::vector<Chromo> children;


    for (int i=0;i<pop_size;i++) {

        /**
         * 轮盘选择：产生一个[0,1)区间内的随机数，若该随机数小于或等于个体的累积概率，选择个体进入子代种群
         */
        double s = 0;
        double r = Random::random_double(0, 1);

        for (Chromo chromo: populations) {
            s += chromo.get_p();
            if (r <= s) {
                children.push_back(chromo);
                break;
            }
        }

    }

    populations.assign(children.begin(), children.end()); // 用子代替换当前这代
}

void GGA::cross() {
    // 交叉, 开始
    // 用cross_num计算变异率
    double p_cross = (cross_num + 0.0) / pop_size;
    for (int i = 0;i < pop_size; i+=2) {
        if (Random::random_double(0, 1) < p_cross) {
            std::pair<Chromo, Chromo> cross_res = populations[i] * populations[i + 1];
            populations[i] = cross_res.first;
            populations[i+1] = cross_res.second;
        }
    }
}

void GGA::cross_replace() {
    // 先按fitness 降序排序
    std::sort(populations.begin(), populations.end(), [](const Chromo &chromo1, const Chromo &chromo2){
        return chromo1.get_fitness() > chromo2.get_fitness();
    });

    // 用前cross_num个染色体交叉得到对应的结果去代替最后cross_num个染色体
    // 所以这里cross_num < pop_size / 2
    cross_num = std::min(pop_size/2, cross_num);
    for (int i=0;i<cross_num;i+=2) {
        std::pair<Chromo, Chromo> cross_res = populations[i] * populations[i + 1];
        populations[pop_size-i-1] = cross_res.first;
        populations[pop_size-i-2] = cross_res.second;
    }
}

void GGA::mutation() {
    for (Chromo &chromo: populations) {
        if (Random::random_double(0, 1) < p_mutation) {
            chromo.mutation(mutation_num);
        }
    }
}

void GGA::start() {
    int iter_cnt = 0;
    int min_num = INT32_MAX;
    while (iter_cnt < iter_num) {
        calc_fitness();
        calc_p();
        rolette_select();
        cross_replace();
        mutation();
        inversion();

        if (get_best_chrome().get_bin_num() < min_num){
            min_num = get_best_chrome().get_bin_num();
            best_solution = get_best_chrome();
        }
        iter_cnt++;
    }
}

Chromo GGA::get_best_chrome() {
    int least_num = INT32_MAX;
    Chromo least_bin_chrome;
    for (Chromo &chromo: populations) {
        if (chromo.get_bin_num() < least_num) {
            least_num = chromo.get_bin_num();
            least_bin_chrome = chromo;
        }
    }
    return least_bin_chrome;
}

Chromo GGA::get_best_solution() {
    return best_solution;
}

void GGA::inversion() {
    std::vector<int> inversion_no;
    for (int i=0;i<inversion_num;i++) {
        int rnd = Random::random_int(0, pop_size-1);
        if (std::find(inversion_no.begin(), inversion_no.end(), rnd) == inversion_no.end()) {
            populations[rnd].inversion();
            inversion_no.push_back(rnd);
        }
    }
}