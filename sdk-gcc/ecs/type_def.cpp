//
// Created by ruiy on 18-3-18.
//

#include "type_def.h"
bool operator< (const Allocat_server &server1, const Allocat_server &server2)
{
    if (server1.target == 0)
    {
        return server1.core < server2.core;
    }
    else
    {
        return server1.storage < server2.storage;
    }
}
