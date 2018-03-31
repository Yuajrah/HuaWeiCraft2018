//
// Created by ruiy on 18-3-30.
//

#include "Chromo.h"

Chromo::Chromo(std::vector<Bin> genes): genes(genes) {}


void Chromo::calc_fitness() {
    /**
     * todo 暂时直接用genes代替, 之后再改
     */
    fitness = 1 / genes.size();
}

