#include "RPiCam.h"

using namespace libcamera;

int RPiCam::camera_count = 0;

RPiCam::RPiCam(CameraManager &manager, std::string id, int64_t fps, int res[2], int directoryNum) {

    camera_number = ++camera_count;

    this->id = id;
    this->fps = fps;
    this->res[0] = res[0];
    this->res[1] = res[1];
    this->directoryNum = directoryNum;

    camera = manager.get(id);

    windowName = "Cam " + std::to_string(camera_number);

}

RPiCam::RPiCam(CameraManager &manager, std::string id) {

    camera_number = ++camera_count;

    this->id = id;
    this->fps = 30;

    camera = manager.get(id);

    windowName = "Cam " + std::to_string(camera_number);

}

void RPiCam::reset() {

    camera->stop();
    camera->release();
    allocator->free(stream);
    allocator.reset();
    camera.reset();

    if (!daemonMode){
        cv::destroyWindow(windowName);
    }

    writer.release();

}

void RPiCam::requestComplete(Request *request) {

    // sees if request was cancelled
    if (request->status() == Request::RequestCancelled) { return; }

    // map of FrameBuffer instances associated with Stream that produced the images
    const std::map<const Stream *, FrameBuffer *> &buffers = request->buffers();

    // loops through every buffer in the request and accesses its metadata
    for (auto bufferPair : buffers) {

        FrameBuffer *buffer = bufferPair.second;
        const FrameMetadata &metadata = buffer->metadata();

        unsigned int nplane = 0;

        for (const FrameBuffer::Plane &plane : buffer->planes()) {

            uint8_t *address = this->mmapPlane(plane);

            unsigned int length = plane.length;

            int process = processPlane(address, length);

        }

        // reuses request and re-queues it to the camera
        request->reuse(Request::ReuseBuffers);
        this->camera->queueRequest(request);

    }

}

int RPiCam::allocateBuffers() {

    for (StreamConfiguration &cfg : *(config)) {

        int ret = allocator->allocate(cfg.stream());

        if (ret < 0) {

            std::cerr << "Cannot allocate buffers." << std::endl;
            return -ENOMEM;

        }
        
        size_t allocated = allocator->buffers(cfg.stream()).size();
    
    }

    return 0;

}

int RPiCam::setup() {

    // prevents another application from stealing and running off with the camera
    camera->acquire();
    
    config = camera->generateConfiguration( { StreamRole::Viewfinder } );

    config->sensorConfig->outputSize = Size(2304, 1296);

    StreamConfiguration rawConfig;

    rawConfig.size = Size(2304, 1296);
    rawConfig.pixelFormat = formats::SRGGB10_CSI2P;

    config->addConfiguration(rawConfig);
    
    // chooses the first (and only) config available for the camera
    StreamConfiguration& streamConfig = config->at(0); 
    // std::cout << "Default viewfinder config is: " << streamConfig.toString() << std::endl;

    // set resolution
    streamConfig.size.width = res[0];
    streamConfig.size.height = res[1];                                                            

    // used if we were to adjust the output sizing stored in streamConfig
    config->validate();
    // std::cout << "Validated viewfinder config is: " << streamConfig.toString() << std::endl;

    // provides a validated config
    camera->configure(config.get());

    std::cout << camera->id() << std::endl;

    const std::string filename = "/home/rsx/Desktop/videos/recording" + std::to_string(directoryNum) + "/cam_" + std::to_string(camera_number) + "_720p30" + ".avi";

    // creates file for this camera's output in.avi format
    // TODO: Add new file names for each run so stuff isn't overwritten
    writer = cv::VideoWriter(filename, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), static_cast<double>(fps), cv::Size(streamConfig.size.width, streamConfig.size.height));

    format = streamConfig.pixelFormat.toString();

    allocator = std::make_unique<FrameBufferAllocator>(camera);

    // fills request vector by creating Request instances from camera and associates a buffer for each of them
    this->allocateBuffers();

    stream = streamConfig.stream();
    const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);

    // fills request vector by creating Request instances from camera and associates a buffer for each of them
    for (unsigned int i = 0; i < buffers.size(); ++i) {

        std::unique_ptr<Request> request = camera->createRequest();

        // sets autofocus to on
        ControlList& controls = request->controls();

        controls.set(controls::AfMode, controls::AfModeContinuous);
        controls.set(controls::FrameDurationLimits, Span<const std::int64_t, 2>({1000000/fps, 1000000/fps}));

        std::cout << controls.get(controls::AfMode).value() << std::endl;

        if (!request) {

            std::cerr << "Cannot create request." << std::endl;
            return -ENOMEM;

        }

        const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());

        if (ret < 0) {

            std::cerr << "Cannot set buffer for request." << std::endl;
            return ret;

        }

        requests.push_back(std::move(request));

    }

    // uses concept of signals and slots (?)
    // Camera::bufferCompleted notifies apps that a buffer with img data is available
    // Camera::requestCompleted notifies apps that a request is completed and therefore all the buffers
    // within it are completed

    // connects slot function
    camera->requestCompleted.connect(this, &RPiCam::requestComplete);

    return 0;

}

int RPiCam::record() {

    // starts camera and queues up all previously created requests
    camera->start();

    for (std::unique_ptr<Request> &request : requests) {

        camera->queueRequest(request.get());

    }

    return 0;

}

// https://www.man7.org/linux/man-pages/man2/mmap.2.html
uint8_t* RPiCam::mmapPlane(const FrameBuffer::Plane &plane) {

    const int fd = plane.fd.get();

    // size_t actualLength = static_cast<size_t>(plane.length + plane.offset);

    // mmaps the plane with a read flag
    void *address = mmap(nullptr, plane.length, PROT_READ, MAP_SHARED, fd, 0);

    if (address == MAP_FAILED) {

        std::cout << "Failed to mmap plane." << std::endl;
        return nullptr;

    }

    // std::cout << "Mmap success! on Cam " << camera_number << std::endl;

    // this casting is necessary for indexing to work properly
    return static_cast<uint8_t*>(address);

}

int RPiCam::processPlane(uint8_t* planeAddr, unsigned int length) {

    cv::Mat raw_frame(res[1], res[0], CV_8UC4, planeAddr);

    cv::Mat color_frame;

    // removes padding data
    cv::cvtColor(raw_frame, color_frame, cv::COLOR_BGRA2BGR);

    cv::Mat formatted_frame;

    // rotates 180 degrees because frame is upside-down by default
    cv::rotate(color_frame, formatted_frame, cv::ROTATE_180);

    if (!daemonMode)
        cv::imshow(windowName, formatted_frame);

    writer.write(formatted_frame);

    cv::waitKey(1);

    if (munmap(planeAddr, length) == -1) {

        std::cout << "I couldn't unmmap this, yo" << std::endl;

    };

    return 0;

}