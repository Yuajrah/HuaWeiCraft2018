//
// Created by ruiy on 18-3-30.
//

#include "Chromo.h"
#include "Random.h"
#include "ff.h"
#include "BasicInfo.h"
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

    // todo 这么传入参数会不会有问题
    insert(begin, {b.genes.begin() + b_begin, b.genes.begin() + b_end});
    b.insert(b_begin, {genes.begin() + begin, genes.begin() + end});

}

void Chromo::insert(int index, std::vector<Bin> insert_genes) {

    std::vector<Vm> reinsert_objects; // 在去除重复包含物体的箱子之前, 统计去除之后会缺失的物体

    // 统计重复包含的物体的编号
    std::vector<int> repeat_contain_no;
    for (Bin &insert_gene: insert_genes) {
        for (Vm &insert_object: insert_gene.objects) {
            repeat_contain_no.push_back(insert_object.no);
        }
    }

    std::vector<Bin> t_genes(genes);
    for (int i=0;i<t_genes.size();i++) {
        bool is_repeat = false;
        for (Vm &object: t_genes[i].objects) {
            // 如果包含了重复物体, 则跳出循环, 开始统计会缺失的物体
            if(repeat_contain_no.end() != std::find(repeat_contain_no.begin(), repeat_contain_no.end(), object.no)){
                is_repeat = true;
                break;
            };
        }

        if (is_repeat == true) {
            for (Vm &object: t_genes[i].objects) {
                if(repeat_contain_no.end() == std::find(repeat_contain_no.begin(), repeat_contain_no.end(), object.no)){
                    reinsert_objects.push_back(object);
                };
            }
            genes.erase(genes.begin() + i);
            if (i < index) {
                index--;
            }
        }

    }
    genes.insert(genes.begin() + index, insert_genes.begin(), insert_genes.end());
    // todo 先用ff替代, 之后再用ffd替换
    genes = ff(genes, reinsert_objects);
}