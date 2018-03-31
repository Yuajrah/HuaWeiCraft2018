//
// Created by ruiy on 18-3-30.
//

#include "Chromo.h"
#include "Random.h"
#include <algorithm>

Chromo::Chromo(std::vector<Bin> genes): genes(genes) {}


void Chromo::calc_fitness() {
    /**
     * todo 暂时直接用genes代替, 之后再改
     */
    fitness = 1 / genes.size();
}


void Chromo::operator*(Chromo &b) {
    /**
     * Chromo1: [bin bin bin bin bin ... bin] 取其中的 [begin, end)
     * Chromo2: [bin bin bin bin bin ... bin] 取其中的 [begin, end) 注: 两条染色体长度并不一定相同
     */
    int end = Random::random_int(1, genes.size());
    int begin = Random::random_int(0, end - 1);

    int b_end = Random::random_int(1, b.genes.size());
    int b_begin = Random::random_int(0, b_end - 1);


}

void Chromo::insert(int index, std::vector<Bin> genes) {
//    std::vector<Vm> objects; // 在去除重复包含物体的箱子之前, 统计去除之后会缺失的物体
//    for (Bin &gene: genes) {
//        for (Vm &object: gene.objects) {
//            std::find();
//        }
//    }
//    genes.insert(genes.begin() + index, genes.begin(), genes.end());
}