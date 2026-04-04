// this code was made following this tutorial: https://libcamera.org/guides/application-developer.html
#include "RPiCam.h"
#include <string.h>
#include <filesystem>
#include <iostream>

using namespace libcamera;
using namespace std::chrono_literals;

namespace fs = std::filesystem;

//In seconds
#define CAMERA_RUN_LENGTH 30

std::vector<std::string> getCameras(CameraManager& cameraManager);
int runCam(RPiCam* cam);
int countDirectories();

int main(int argc, char** argv) {

    bool isDaemon;
    int res[2];
    int fps;
    int recLength ;

    // Run as daemon with "--daemon" to prevent a GUI from showing and crashing the daemon
    // (cv::imshow uses Qt which isnt avaliable when run as a daemon)
    // strcmp help from https://stackoverflow.com/a/2300747
    std::cout << argv[0] << std::endl;
    std::cout << argv[1] << std::endl;

    // std::string argList[5] = {"-d", "-r", "-f", "-c", "-l"};

    // name --daemon (daemon mode on; no options) -r {width} {height} -f {framerate} -l {length of recording in sec.}
    switch(argc) {

        // specify daemon
        case 2:

            std::cout << "daemon" << std::endl;

            isDaemon = strcmp(argv[1], "--daemon") == 0;
            res[0] = 1280;
            res[1] = 720;
            fps = 30;
            recLength = 120;
            
            break;

        // specify resolution
        case 5:

            isDaemon = strcmp(argv[1], "--daemon") == 0;
            try {

                res[0] = std::stoi(std::string(argv[3]));
                res[1] = std::stoi(std::string(argv[4]));

            } catch (const std::runtime_error e) {

                std::cerr << "Please input numbers for the resolution (-r) argument!" << std::endl;

            }

            fps = 30;
            recLength = 10;

            break;
        
        // specify framerate
        case 7:
            
            isDaemon = strcmp(argv[1], "--daemon") == 0;
            try {

                res[0] = std::stoi(std::string(argv[3]));
                res[1] = std::stoi(std::string(argv[4]));

            } catch (const std::runtime_error e) {

                std::cerr << "Please input numbers for the resolution (-r) argument!" << std::endl;

            }

            try {

                fps = std::stoi(std::string(argv[6]));

            } catch (const std::runtime_error e) {

                std::cerr << "Please input numbers for the framerate (-f) argument!" << std::endl;

            }

            recLength = 10;

            break;

        // specify rec length    
        case 9:

            isDaemon = strcmp(argv[1], "--daemon") == 0;
            try {

                res[0] = std::stoi(std::string(argv[3]));
                res[1] = std::stoi(std::string(argv[4]));

            } catch (const std::runtime_error e) {

                std::cerr << "Please input numbers for the resolution (-r) argument!" << std::endl;

            }

            try {

                fps = std::stoi(std::string(argv[6]));

            } catch (const std::runtime_error e) {

                std::cerr << "Please input numbers for the framerate (-f) argument!" << std::endl;

            }

            try {

                recLength = std::stoi(std::string(argv[8]));

            } catch (const std::runtime_error e) {

                std::cerr << "Please input numbers for the reording length (-l) argument!" << std::endl;

            }

            break;

            // no arguments provided, setting to non-daemon mode w/ defaults
        default:

            isDaemon = false;
            res[0] = 1280;
            res[1] = 720;
            fps = 30;
            recLength = 10;

            break;

    }
    
    // create a camera manager
    std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
    cm->start();

    int directoryNum = countDirectories();
    const std::filesystem::path directoryPath = "/home/rsx/Desktop/videos/recording" + std::to_string(directoryNum);
    try {

        std::filesystem::create_directories(directoryPath);

    } catch (std::filesystem::filesystem_error& err) {

        std::cerr << "Error creating directory." << std::endl;

    }
    

    std::vector<std::string> cameraIDs;
    cameraIDs = getCameras(*cm);

    // ensures we actually got cameras fr
    if (cameraIDs.size() == 0) { std::cout << "No cameras found." << std::endl; cm->stop(); return -1; }

    std::cout << cameraIDs.size() << std::endl;

    for (int i = 0; i < cameraIDs.size(); i++) {

        std::cout << i << std::endl;

        std::string cameraId = cameraIDs[i];
        RPiCam* cam = new RPiCam(*cm, cameraId, fps, res, directoryNum);
        cam->daemonMode = isDaemon;

        std::thread cam_thread{runCam, cam};
        cam_thread.detach();

    }

    std::this_thread::sleep_for(200ms + 10 * 1s);

    return 0;

}

int runCam(RPiCam* cam) {

    cam->setup();

    cam->record();

    // TODO: fix this later!
    std::this_thread::sleep_for(1s * 10);

    cam->reset();

    return 0;

}

// gets all of the cameras using the camera manager 
std::vector<std::string> getCameras(CameraManager& cameraManager) {

    auto cameras = cameraManager.cameras();
    std::vector<std::string> cameraIDs;

    if (cameras.empty()) {

        std::cout << "No cameras found." << std::endl;
        cameraManager.stop();

        // returns empty vector
        return std::vector<std::string>();

    }

    for (const auto &camera : cameras) {

        cameraIDs.push_back(camera->id());

    }

    return cameraIDs;

}

int countDirectories(){

    unsigned int dirCount = 0;

    const std::string path = "/home/rsx/Desktop/videos/";

    try {

        for (const auto& subdirectory : std::filesystem::directory_iterator(path)) {

            if (std::filesystem::is_directory(subdirectory.status())) {

                dirCount++;

            }

        }

    } catch (const std::filesystem::filesystem_error& err) {

        std::cerr << "Error accessing filesystem: " << err.what() << std::endl;

    }

    return dirCount;

}