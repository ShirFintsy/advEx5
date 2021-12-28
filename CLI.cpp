#include "CLI.h"

// Constructor:
CLI::CLI(DefaultIO* dio) {
    this->dio = dio;
    commands.push_back(new UploadCVS(dio));
    commands.push_back(new Settings(dio));
    commands.push_back(new DetectAnomalies(dio));
    commands.push_back(new Result(dio));
    commands.push_back(new UploadAnomalies(dio));
    commands.push_back(new Exit(dio));
}

void CLI::start(){
    int index = 0;
    while (index != 5) {
        dio->write("Welcome To the Anomaly Detection Server.");
        dio->write("Please choose an option:");
        for (int i = 0; i < 5; i++) {
            dio->write(i);
            dio->write(commands[index]->description);
        }
        string opt = dio->read();
        index= opt[0] - '0' - 1;
        if(index >= 0 && index <= 6)
            commands[index]->execute(&sharedState);
    }

}

CLI::~CLI() {
    for (Command com : commands) {
        delete com;
    }
}
