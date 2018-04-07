//
// Created by ruiy on 18-3-31.
//

#ifndef SDK_CLION_RANDOM_H
#define SDK_CLION_RANDOM_H

#include <random>
#include <ctime>
#include <vector>

class Random {
public:
    static std::default_random_engine generator;
    //Random(time_t t = time(NULL)): generator(t) {}

    static uint32_t random_int(uint32_t min, uint32_t max) { // [min, max]
        return std::uniform_int_distribution<uint32_t>{min, max}(generator);
    }

    static double random_double(double min, double max) { // [min, max)
        return std::uniform_real_distribution<double>{min, max}(generator);
    }
};


#endif //SDK_CLION_RANDOM_H
