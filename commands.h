

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
    virtual string read()=0;
    virtual void write(string text)=0;
    virtual void write(float f)=0;
    virtual void read(float* f)=0;
    virtual ~DefaultIO(){}

};

class Command{
    DefaultIO* dio;
public:
    string description;
    Command(DefaultIO* dio, string des):dio(dio){}
    virtual void execute()=0;
    virtual ~Command(){}
};

// I don't know what we need to put here- but this is the information we need
// to share with all commands classes
struct commonInfo {
    commonInfo();
};

class UploadCVS:public Command {
    // Constructor:
    UploadCVS(DefaultIO dio):Command(dio,"upload a time series csv file"){}
    void execute(commonInfo* info) {

    }
};
class Settings:public Command {
    // Constructor:
    Settings(DefaultIO dio): Command(dio, "algorithm settings"){}
};
class DetectAnomalies:public Command {
    // Constructor:
    DetectAnomalies(DefaultIO dio): Command(dio, "detect anomalies"){}
};
class Result:public Command {
    // Constructor:
    Result(DefaultIO dio): Command(dio, "display results"){}
};
class UploadAnomalies:public Command {
    // Constructor:
    UploadAnomalies(DefaultIO dio): Command(dio, "upload anomalies and analyze results"){}
};
class Exit:public Command {
    // Constructor:
    Exit(DefaultIO dio): Command(dio, "exit"){}
};



#endif /* COMMANDS_H_ */