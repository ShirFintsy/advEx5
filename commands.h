

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <utility>
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

    // you may add additional methods here
    void saveFile(string csvFile){

        ifstream file(csvFile);
        if(!file.is_open()) throw std::runtime_error("Could not open file");
        string reader;

        while(reader != "done"){
            getline(file, reader);
            reader = read();
        }
        file.close();
    }
};

// you may add here helper classes
struct Information {
    float threshold = 0.9;
    vector<AnomalyReport> detectReports;
};

// you may edit this class
class Command{
public:
    DefaultIO* dio;
    string order;
    Information info;
    Command(DefaultIO* dio, string  order):dio(dio), order(std::move(order)){}
    virtual void execute()=0;
    virtual ~Command(){}
};

// implement here your command classes
/**
 * this class is for uploading the train file and the test file
 */
class uploadTS:public Command {
public:
    uploadTS(DefaultIO* dio):Command(dio,"upload"){}
    void execute() override{
        dio->write("Please upload your local train CSV file.\n");
        dio->saveFile("tsTrain.csv");
        dio->write("Upload complete.\n");
        dio->write("Please upload your local test CSV file.\n");
        dio->saveFile("tsTest.csv");
        dio->write("Upload complete.\n");
    }
};

class setThreshold:public Command {
public:
    setThreshold(DefaultIO* dio):Command(dio,"settings"){}
    virtual void execute(Information info) {
        dio->write("The current correlation threshold is: ");
        dio->write(info.threshold);
        dio->write("\n");
        float newThreshold;
        while (true) {
            dio->read(&newThreshold);
            if(newThreshold < 1 && newThreshold > 0) break;
            dio->write("please choose a value between 0 and 1.\n");
        }
        info.threshold = newThreshold;
    }
};

class detAnomal:public Command {
public:
    detAnomal(DefaultIO* dio):Command(dio,"settings"){}
    virtual void execute(Information &info) {
    HybridAnomalyDetector detector;
    TimeSeries tsTrain("tsTrain.csv");
    TimeSeries tsTest("tsTest.csv");
    detector.setUserThreshold(info.threshold);
    detector.learnNormal(tsTrain);
    info.detectReports = detector.detect(tsTest);
    dio->write("anomaly detection complete.\n");
    }
};

class Result:public Command {
public:
    // Constructor:
    Result(DefaultIO* dio): Command(dio, "display results"){}
    virtual void execute(Information &info) {
        for (AnomalyReport report : info.detectReports) {
            const long timeStep = report.timeStep;
            const string description = report.description;
            dio->write((float)timeStep);
            dio->write("\t");
            dio->write(description);
            dio->write("\n");
        }
        dio->write("Done.\n");
    }
};


#endif /* COMMANDS_H_ */