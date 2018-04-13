//
// Created by ruiy on 18-4-13.
//

#ifndef SDK_CLION_CACHE_H
#define SDK_CLION_CACHE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <locale.h>

#include "svm.h"
#include "Solver.h"

#define INF HUGE_VAL
#define TAU 1e-12

//
// Kernel Cache
//
// l is the number of total data items
// size is the cache size limit in bytes
//


class Cache
{
public:
    Cache(int l,long int size);
    ~Cache();

    // request data [0,len)
    // return some position p where [p,len) need to be filled
    // (p >= len if nothing needs to be filled)
    int get_data(const int index, float **data, int len);
    void swap_index(int i, int j);

private:
    int l;
    long int size;
    struct head_t
    {
        head_t *prev, *next;	// a circular list
        float *data;
        int len;		// data[0,len) is cached in this entry
    };

    head_t *head;
    head_t lru_head;
    void lru_delete(head_t *h);
    void lru_insert(head_t *h);
};


#endif //SDK_CLION_CACHE_H
