#include "surveillance.h"
#include "PiCamera.h"

const char* params
        = "{ help           | false | print usage          }"
          "{ camera_device  | 0     | camera device number }"
          "{ output         |       | video output directory }"
          "{ min_confidence | 0.5   | min confidence       }"
          "{ show           | false | show video output    }";



int main(int argc, char** argv){
    CommandLineParser parser(argc, argv, params);

    if (parser.get<bool>("help"))
    {
        parser.printMessage();
        return 0;
    }

    auto confidenceThreshold = parser.get<float>("min_confidence");
    String outputDir = parser.get<string>("output");
    int cameraDevice = parser.get<int>("camera_device");
    bool showOutput = parser.get<bool>("show");

    FaceSurveillance camera(outputDir, MODEL_PROTO, MODEL_BINARY, confidenceThreshold, cameraDevice=cameraDevice);

    camera.start(showOutput);

    return 0;
}

