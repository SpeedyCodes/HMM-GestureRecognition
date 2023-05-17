//
// Created by dasha on 10.05.2023.
//

#ifndef MAIN_PY_EXTERNMANAGER_H
#define MAIN_PY_EXTERNMANAGER_H
#include <iostream>
#include <vector>

struct externManager {
    static std::vector<std::vector<double>> getLandmarksFromVideo(const char* absoluteVideoPath);
    static std::vector<int> preprocessData(const std::vector<std::vector<double>>& data);
    static std::vector<std::vector<int>> preprocessData(const std::vector<std::vector<std::vector<double>>>& data);
};


#endif //MAIN_PY_EXTERNMANAGER_H
