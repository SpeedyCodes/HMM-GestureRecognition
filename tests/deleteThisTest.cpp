///* Deleted...*/
#include <iostream>
#include <src/utils/externManager.h>
#include <src/utils/MediapipeInterface.h>

int main() {

    std::vector<std::vector<double>> result = externManager::getLandmarksFromVideo("C:/Users/dasha/PycharmProjects/Collected_videos/zero/zero7_0.avi");
    std::vector<int> preprocessedResult = externManager::preprocessData(result);
    std::cout << "End of the script" << std::endl;

    return 0;
}
//int main() {
//
//    MediapipeInterface mpi;
//    mpi.open();
//
//    return 0;
//}