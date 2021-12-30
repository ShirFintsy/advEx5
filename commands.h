

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
    void saveFile(string csvFile) {// need to check this func

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
    float threshold = 0.9;
    vector<AnomalyReport> detectReports;
    commonInfo();
};

class UploadCVS:public Command {
    // Constructor:
    UploadCVS(DefaultIO dio):Command(dio,"upload a time series csv file"){}
    virtual void execute(commonInfo* info) {
        dio->write("Please upload your local train CSV file.\n");
        dio->saveFile("tsTrain.csv");
        dio->write("Upload complete.\n");
        dio->write("Please upload your local test CSV file.\n");
        dio->saveFile("tsTest.csv");
        dio->write("Upload complete.\n");
    }
};
class Settings:public Command {
    // Constructor:
    Settings(DefaultIO dio): Command(dio, "algorithm settings"){}
    virtual void execute(commonInfo* info) {
        dio->write("The current correlation threshold is: ");
        dio->write(info.threshold);
        dio->write("\n");
        float newThreshold;
        while (true) {
            dio->read(&newThreshold);
            if (newThreshold <= 1 && newThreshold >= 0) break;
            dio->write("please choose a value between 0 and 1.\n");
        }
        info.threshold = newThreshold;
    }
};
class DetectAnomalies:public Command {
    // Constructor:
    DetectAnomalies(DefaultIO dio): Command(dio, "detect anomalies"){}
    virtual void execute(commonInfo* info) {
        HybridAnomalyDetector detector;
        //where did we get this file from?
        TimeSeries tsTrain("tsTrain.csv");
        TimeSeries tsTest("tsTest.csv");
        detector.setUserThreshold(info.threshold);
        detector.learnNormal(tsTrain);
        info->detectReports = detector.detect(tsTest);
        dio->write("anomaly detection complete.\n");
        for (AnomalyReport report : info->detectReports) {
            //TODO: find the time step range, and add to a member in the struct
        }
    }
};
class Result:public Command {
    // Constructor:
    Result(DefaultIO dio): Command(dio, "display results"){}
    virtual void execute(commonInfo* info) {
        for (AnomalyReport report : info->detectReports) {
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
class UploadAnomalies:public Command {
    // Constructor:
    UploadAnomalies(DefaultIO dio): Command(dio, "upload anomalies and analyze results"){}
    virtual void execute(commonInfo* info) {

    }
};
class Exit:public Command {
    // Constructor:
    Exit(DefaultIO dio): Command(dio, "exit"){}
    virtual void execute(commonInfo* info) {
        dio->write("Please upload your local anomalies file.")
        // TODO:
        // get the file and get the details in it: testFile
        vector<AnomalyReport> newReports = detector.detect(testFile);
        //find the time steps...
        //calculate the YAHAS of time steps

    }
};



#endif /* COMMANDS_H_ */