cmake_minimum_required(VERSION 3.21)
project(advEx5)

set(CMAKE_CXX_STANDARD 14)

add_executable(advEx5 anomaly_detection_util.h anomaly_detection_util.cpp CLI.cpp CLI.h commands.h
        HybridAnomalyDetector.h HybridAnomalyDetector.cpp MainTrain.cpp minCircle.h minCircle.cpp
        SimpleAnomalyDetector.h SimpleAnomalyDetector.cpp timeseries.h timeseries.cpp)
