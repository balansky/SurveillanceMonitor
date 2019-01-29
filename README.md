# SurveillanceMonitor
Surveillance System to Automatically Track and Detect Face.

### Setup

- Install [Opencv4](https://github.com/opencv/opencv)
- mkdir build & cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_GLOBALLY ON ..
- sudo make install

### Launch
- ./PiCamera -output="output directory to save the video" -camera_device=0(camera device number, default 0) -min_confidence=0.5(confidence threshold for face detection, default 0.5)


