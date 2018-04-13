//
// Created by ruiy on 18-4-13.
//

#ifndef SDK_CLION_CACHE_H
#define SDK_CLION_CACHE_H

#include <cmath>
#include <cctype>
#include <cfloat>
#include <cstring>
#include <cstdarg>
#include <climits>
#include <clocale>
#include <cstdlib>

#include "type_def.h"

class Buf
{
public:
    Buf(int l,long int size);
    ~Buf();

    int get_data(const int index, float **data, int len);

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
