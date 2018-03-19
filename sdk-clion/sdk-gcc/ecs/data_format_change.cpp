//
// Created by caocongcong on 18-3-19.
//
#include "data_format_change.h"
std::string change_map_char(std::map<int, int>data)
{

    std::map<int,int>::iterator it;

    it = data.begin();
    int sum = 0;
    std::string tmp_string;
    while(it != data.end())
    {
        sum += it->second;
        tmp_string += "flavor";
        tmp_string += std::to_string(it->first);
        tmp_string += " ";
        tmp_string += std::to_string(it->second);
        tmp_string += "\n";
        it ++;
    }
    tmp_string += "\n";
    std::string head = std::to_string(sum);
    head += "\n\n";
    std::string result = head+tmp_string;
    return result;
}


std::string change_format(std::vector<std::map<int,int>> result_code)
{
    std::string result;
    result = std::to_string(result_code.size());
    result += "\n";
    for (size_t i =0; i < result_code.size(); i++) {
        result += std::to_string(i+1);
        for (std::map<int, int>::iterator j=result_code[i].begin(); j!=result_code[i].end(); j++)
        {
            result += " ";
            result += "flavor";
            result += std::to_string(j->first);
            result += " ";
            result += std::to_string(j->second);
        }
        result += "\n";
    }
    return result;
}
