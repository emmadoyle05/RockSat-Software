#include <libcamera/libcamera.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <thread>
#include <sys/mman.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace libcamera;

#ifndef RPICAM_H
#define RPICAM_H

class RPiCam {

    private:

        static int camera_count;
        int camera_number;

        std::shared_ptr<Camera> camera;
        std::unique_ptr<CameraConfiguration> config;
        std::unique_ptr<FrameBufferAllocator> allocator;
        std::vector<std::unique_ptr<Request>> requests;
        Stream* stream = nullptr;
        int64_t fps;
        int res[2];

        void requestComplete(Request *request);
        int allocateBuffers();
        uint8_t* mmapPlane(const FrameBuffer::Plane &plane);
        int processPlane(uint8_t* planeAddr, unsigned int length);
        std::string windowName;

        int directoryNum;

        cv::VideoWriter writer;

    public:

        std::string id;
        std::string format;

        bool daemonMode;

        // stops camera, frees allocator and memory, and releases camera
        void reset();

        // sets up camera with all the configurations needed
        int setup();

        // starts recording
        int record();

        // constructors
        RPiCam(CameraManager &manager, std::string id, int64_t fps, int res[2], int directoryNum);
        RPiCam(CameraManager &manager, std::string id);

};

#endif