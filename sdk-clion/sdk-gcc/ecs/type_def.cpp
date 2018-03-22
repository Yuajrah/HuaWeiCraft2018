//
// Created by ruiy on 18-3-18.
//

#include "type_def.h"
bool operator< (const Allocat_server &server1, const Allocat_server &server2)
{
    if (server1.target == 0)
    {
        if( server1.core > server2.core)
        {
            return true;
        }
        else if (server1.core == server2.core)
        {
            if(server1.id > server2.id)
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        if(server1.storage > server2.storage)
        {
            return true;
        }
        else if (server1.storage == server2.storage)
        {
            if(server1.id > server2.id)
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }
}
