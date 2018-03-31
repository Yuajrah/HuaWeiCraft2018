//
// Created by ruiy on 18-3-30.
//

#include  "GGA.h"
#include "data_format_change.h"
#include "ff.h"

GGA::GGA(std::vector<Vm> objects, Server server_info, int pop_size):
        objects(objects),
        server_info(server_info),
        pop_size(pop_size) {}

void GGA::initial() {
    /**
     * 随机初始化种群
     */
    for (int i=0;i<pop_size;i++) {
        /**
         * 生成物体的随机排列, 然后使用ff算法去装箱, 得到装箱结果, 利用装箱结果构造染色体(这波操作可能有点秀... :P)
         */
        Chromo chromo(ff(random_permutation(objects), server_info));
        populations.push_back(chromo);
    }
}

void GGA::select() {

}


