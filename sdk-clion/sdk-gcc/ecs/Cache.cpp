//
// Created by ruiy on 18-4-13.
//

#include "Cache.h"

Cache::Cache(int l_,long int size_):l(l_),size(size_)
{
    head = (head_t *)calloc(l,sizeof(head_t));	// initialized to 0
    size /= sizeof(float);
    size -= l * sizeof(head_t) / sizeof(float);
    size = std::max(size, 2 * (long int) l);	// cache must be large enough for two columns
    lru_head.next = lru_head.prev = &lru_head;
}

Cache::~Cache()
{
    for(head_t *h = lru_head.next; h != &lru_head; h=h->next)
        free(h->data);
    free(head);
}

void Cache::lru_delete(head_t *h)
{
    // delete from current location
    h->prev->next = h->next;
    h->next->prev = h->prev;
}

void Cache::lru_insert(head_t *h)
{
    // insert to last position
    h->next = &lru_head;
    h->prev = lru_head.prev;
    h->prev->next = h;
    h->next->prev = h;
}

int Cache::get_data(const int index, float **data, int len)
{
    head_t *h = &head[index];
    if(h->len) lru_delete(h);
    int more = len - h->len;

    if(more > 0) {
        // free old space
        while(size < more)
        {
            head_t *old = lru_head.next;
            lru_delete(old);
            free(old->data);
            size += old->len;
            old->data = 0;
            old->len = 0;
        }

        // allocate new space
        h->data = (float *)realloc(h->data,sizeof(float)*len);
        size -= more;
        std::swap(h->len,len);
    }

    lru_insert(h);
    *data = h->data;
    return len;
}