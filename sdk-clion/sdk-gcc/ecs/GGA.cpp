//
// Created by ruiy on 18-3-30.
//

#include  "GGA.h"
#include "data_format_change.h"
#include "ff.h"
#include "Random.h"

GGA::GGA(std::vector<Vm> objects, int pop_size, double p_cross, double p_mutation, int mutation_num):
        objects(objects),
        pop_size(pop_size),
        p_cross(p_cross),
        p_mutation(p_mutation),
        mutation_num(mutation_num) {}


void GGA::initial() {
    for (int i=0;i<pop_size;i++) {
        /**
         * 生成物体的随机排列, 然后使用ff算法去装箱, 得到装箱结果, 利用装箱结果构造染色体(这波操作可能有点秀... :P)
         */
        Chromo chromo(ff({}, random_permutation(objects)));
        populations.push_back(chromo);
    }
}

void GGA::calc_fitness() {
    for (auto chromo: populations) chromo.calc_fitness();
}

void GGA::calc_p() {
    double sum = 0.0;
    for (Chromo chromo: populations) {
        sum += chromo.get_fitness();
    }
    for (Chromo chromo: populations) {
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
            }
        }

    }

    populations.assign(children.begin(), children.end()); // 用子代替换当前这代
}

void GGA::cross() {
    // 交叉, 开始
    for (int i = 0;i < pop_size; i+=2) {
        if (Random::random_double(0, 1) < p_cross) {
            populations[i] * populations[i + 1];
        }
    }
}

void GGA::mutation() {
    for (Chromo &chromo: populations) {
        if (Random::random_double(0, 1) < p_mutation) {
            chromo.mutation(mutation_num);
        }
    }
}

