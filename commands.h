

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

int* get_range(string s);
int get_N(vector<int*> clientRange);

class DefaultIO{
public:
    virtual string read()=0;
    virtual void write(string text)=0;
    virtual void write(float f)=0;
    virtual void read(float* f)=0;
    virtual ~DefaultIO(){}
    int saveFile(string csvFile) {// need to check this func

        ifstream file(csvFile);
        if(!file.is_open()) throw std::runtime_error("Could not open file");
        string reader;

        int numOfLines = -1;
        while(reader != "done"){
            numOfLines++;
            getline(file, reader);
            reader = read();
        }
        file.close();
        return numOfLines;
    }
};

struct SerialTimeStep {
    int start;
    int end;
    string description;
};

struct commonInfo {
    float threshold = 0.9;
    vector<AnomalyReport> detectReports;
    vector<SerialTimeStep> stp;
    int fileSize;
    commonInfo(){};
};

class Command {
protected:
    DefaultIO *dio;
public:
    string description;

    Command(DefaultIO *d, string des) {
        dio = d;
        description = des;
    }

    //Command(DefaultIO* dio, string des):dio(dio),description(des){}
    virtual void execute(commonInfo *info) = 0;

    virtual ~Command() {}

    int* get_range(string s){
        std::string delimiter = ",";
        size_t pos = 0;
        std::string token; // first
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            s.erase(0, pos + delimiter.length());
        }
        int start = stoi(token);
        int end = stoi(s);
        static int range[2];
        range[0] = start;
        range[1] = end;
        return range;
    }
    int get_N(vector<int*> clientRange){
        int sum = 0;
        for (int* curr: clientRange) {
            sum += curr[1] - curr[0] + 1;
        }return sum;
    }
};



class UploadCVS:public Command {
    // Constructor:
public:
    UploadCVS(DefaultIO* dio):Command(dio,"upload a time series csv file"){}
    virtual void execute(commonInfo* info) {
        dio->write("Please upload your local train CSV file.\n");
        dio->saveFile("tsTrain.csv");
        dio->write("Upload complete.\n");
        dio->write("Please upload your local test CSV file.\n");
        int size = dio->saveFile("tsTest.csv");
        info->fileSize = size;
        dio->write("Upload complete.\n");
    }
};
class Settings:public Command {
public:
    // Constructor:
    Settings(DefaultIO* dio): Command(dio, "algorithm settings"){}
    virtual void execute(commonInfo* info) {
        dio->write("The current correlation threshold is: ");
        dio->write(info->threshold);
        dio->write("\n");
        float newThreshold;
        while (true) {
            dio->read(&newThreshold);
            if (newThreshold <= 1 && newThreshold >= 0) break;
            dio->write("please choose a value between 0 and 1.\n");
        }
        info->threshold = newThreshold;
    }
};
class DetectAnomalies:public Command {
public:
    // Constructor:
    DetectAnomalies(DefaultIO* dio): Command(dio, "detect anomalies"){}
    virtual void execute(commonInfo* info) {
        HybridAnomalyDetector detector;
        //where did we get this file from?
        TimeSeries tsTrain("tsTrain.csv");
        TimeSeries tsTest("tsTest.csv");
        detector.setUserThreshold(info->threshold);
        detector.learnNormal(tsTrain);
        info->detectReports = detector.detect(tsTest);
        dio->write("anomaly detection complete.\n");

        SerialTimeStep steps;
        steps.description = "";
        steps.start = 0;
        steps.end = 0;

        for (AnomalyReport report : info->detectReports) {
            if (steps.description != report.description) {
                if (steps.description != "") {info->stp.push_back(steps);}
                steps.description = report.description;
                steps.start = report.timeStep;
                steps.end = report.timeStep;
            } else {
                if (report.timeStep == steps.end + 1) {
                    steps.end = report.timeStep;
                } else if (report.timeStep != steps.end) {
                    info->stp.push_back(steps);
                    steps.start = report.timeStep;
                    steps.end = report.timeStep;
                }
            }
        }
        info->stp.push_back(steps); // insert the last one
    }
};
class Result:public Command {
public:
    // Constructor:
    Result(DefaultIO* dio): Command(dio, "display results"){}
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
public:
    // Constructor:
    UploadAnomalies(DefaultIO* dio): Command(dio, "upload anomalies and analyze results"){}
    virtual void execute(commonInfo* info) {
        dio->write("Please upload your local anomalies file.");
        string lineInFile = "";

        vector<int*> clientRange;
        int p = 0;
        string s;
        while ((s = dio->read()) != "done") {
            int* range = get_range(s);
            clientRange.push_back(range);
            p++;
        }
        dio->write("Upload complete\n");

        int FP=0, TP=0;
        for (SerialTimeStep time: info->stp){
            for (int* curr: clientRange) {
                // curr[0]- start, curr[1]- end
                if ((curr[0] < time.start && curr[1] < time.start) || (curr[0] > time.start && curr[0] > time.end)) {
                    FP++;
                }
                if ((curr[0] <= time.end && curr[1] >= time.end) || (curr[0] <= time.start && curr[1] >= time.start)) {
                    TP++;
                }
            }
        }
        float positiveRate = (float) TP / (float) p;
        float N = (float) info->fileSize - (float) get_N(clientRange);
        float falseAlarm = (float) FP / N;

        dio->write("True Positive Rate: ");
        dio->write(positiveRate);
        dio->write("\n");
        dio->write("False Positive Rate: ");
        dio-> write(falseAlarm);
        dio->write("\n");
        
    }
};
class Exit:public Command {
public:
    // Constructor:
    Exit(DefaultIO* dio): Command(dio, "exit"){}
    virtual void execute(commonInfo* info) {
        return;
    }

};



#endif /* COMMANDS_H_ */