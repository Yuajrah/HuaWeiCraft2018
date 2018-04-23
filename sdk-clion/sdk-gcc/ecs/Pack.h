//
// Created by txink on 18-4-23.
//

#ifndef SDK_CLION_PACK_H
#define SDK_CLION_PACK_H

#include "type_def.h"


class Pack {
private:
    double finalScore;
    double onceScore;
    int vmTypeNum;
    int bestServerType;
    std::map<int, Server> Servers;
    std::map<int,int> vmNum;
    std::map<int,Vm> vmInfo;
    std::vector<double> paramA;

    void setA(int paramType);
    std::map<int,int> packOnce(std::map<int, int> vm_num,Server server);
    std::map<int,int> packOnceBest(std::map<int, int> vm_num);


    void ZeroOnePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos);
    void CompletePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int pos);
    void MultiplePack(std::vector<std::vector <int> > &dp, std::vector<std::vector<std::vector<int> > > &used, int C, int D, int U, int V, int W, int M, int pos);
    std::vector<int> getPath(std::vector<std::vector<std::vector<int> > > &used, std::map<int,Vm> vminfo, int U, int V);
    bool checkEmpty(std::map<int, int> vmnum);

public:
    Pack(std::map<int, Server> servers,std::map<int,int> vm_num,std::map<int,Vm> vm_info, int vm_typenum);

    std::vector<std::map<int,int>> packStepBest(std::vector<Server> &serverResult);
    std::vector<std::map<int,int>> packOneTypeServer();
    double getFinalScore(std::vector<Server> serverResult,std::map<int,int> vm_num);
    double getOnceScore(Server server, std::map<int,int> record);
};


#endif //SDK_CLION_PACK_H
