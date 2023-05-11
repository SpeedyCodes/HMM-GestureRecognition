//
// Created by dasha on 10.05.2023.
//

#ifndef MAIN_PY_EXTERNMANAGER_H
#define MAIN_PY_EXTERNMANAGER_H
#include <iostream>
#include <vector>

struct externManager {
    static std::vector<std::vector<double>> getLandmarksFromVideo(const char* videoPath);
};


#endif //MAIN_PY_EXTERNMANAGER_H
