//
// Created by ruiy on 18-3-30.
//

#include "Chromo.h"
#include "Random.h"
#include "ff.h"
#include "BasicInfo.h"
#include <algorithm>
#include <iterator>

Chromo::Chromo(std::vector<Bin> genes): genes(genes) {}


void Chromo::calc_fitness() {
    /**
     *
     * todo 暂时直接用genes代替, 之后再改
     *
     */
//    fitness = 1.0 / genes.size();

    double sum = 0.0;
    int k = 2;
    for (Bin &bin: genes) {
        sum += pow((BasicInfo::server_info.mem - bin.mems) * (BasicInfo::server_info.core - bin.cores) / (BasicInfo::server_info.mem * BasicInfo::server_info.core), 2);
//        sum += pow((BasicInfo::server_info.mem - bin.mems)  / (BasicInfo::server_info.mem), 2);
    }
    fitness = sum / genes.size();
}


std::pair<Chromo, Chromo> Chromo::operator*(Chromo b) {
    /**
     * Chromo1: [bin bin bin bin bin ... bin] 取其中的 [begin, end)
     * Chromo2: [bin bin bin bin bin ... bin] 取其中的 [begin, end) 注: 两条染色体长度并不一定相同
     */
    int end = Random::random_int(1, genes.size());
    int begin = Random::random_int(0, end - 1);

    int b_end = Random::random_int(1, b.genes.size());
    int b_begin = Random::random_int(0, b_end - 1);

    // 用a_copy去插入b的子段, 保证this不受破坏
    Chromo a_copy(*this);
    // todo 这么传入参数会不会有问题
    a_copy.insert(begin, {b.genes.begin() + b_begin, b.genes.begin() + b_end});
    b.insert(b_begin, {genes.begin() + begin, genes.begin() + end});
    return {a_copy, b};
}

void Chromo::insert(int index, std::vector<Bin> insert_genes) {

    std::vector<Vm> reinsert_objects; // 在去除重复包含物体的箱子之前, 统计去除之后会缺失的物体
    int actual_index = index; // 由于会删除箱子, 所以实际插入的位置和index不一样

    // 统计重复包含的物体的编号
    std::vector<int> repeat_contain_no;
    for (Bin &insert_gene: insert_genes) {
        for (Vm &insert_object: insert_gene.objects) {
            repeat_contain_no.push_back(insert_object.no);
        }
    }

    std::vector<Bin> t_genes(genes);
    genes.clear();

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

            if (i < index) {
                actual_index--;
            }
        } else {
            genes.push_back(t_genes[i]);
        }

    }

    genes.insert(genes.begin() + actual_index, insert_genes.begin(), insert_genes.end());
    // todo 先用ff替代, 之后再用ffd替换
    genes = ff(genes, reinsert_objects);

}

void Chromo::mutation(int mutation_num) {

    std::vector<Vm> eliminate_objects;

    for (int i=0;i<mutation_num;i++) {

        int rnd = Random::random_int(0, genes.size() - 1); // 随机产生某个基因的位置, 该位置的基因(箱子)会被删除
        // 存下要被删除箱子中的物体
        eliminate_objects.insert(eliminate_objects.end(), genes[rnd].objects.begin(), genes[rnd].objects.end());
        genes.erase(genes.begin() + rnd); // 删除rnd位置的箱子
        if (genes.empty()) break;
    }
    genes = ff(genes, eliminate_objects);

}

void Chromo::inversion() {
    int i = Random::random_int(0, genes.size() - 1);
    int j = Random::random_int(0, genes.size() - 1);
    std::swap(genes[i], genes[j]);
}