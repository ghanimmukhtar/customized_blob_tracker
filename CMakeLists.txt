project(customized_blob_tracker)

cmake_minimum_required(VERSION 2.6)

find_package(VISP)
find_package(Boost REQUIRED COMPONENTS system )

set(CMAKE_BUILD_TYPE Debug)

# set the list of source files
set(tutorial_cpp
  tutorial-image-conversion.cpp
  tutorial-kinect-sensor.cpp
  tutorial-barcode-detector.cpp
  tutorial-video-recorder.cpp
  tutorial-blob-tracker-video.cpp
  tutorial-blob-auto-tracker.cpp
  tutorial-blob-tracker-live-firewire.cpp
  tutorial-blob-tracker-live-v4l2.cpp
  tutorial-blob-tracker-kinect.cpp)

list(APPEND tutorial_data "${CMAKE_CURRENT_SOURCE_DIR}/target.pgm" )

foreach(cpp ${tutorial_cpp})
  visp_add_target(${cpp})
  if(COMMAND visp_add_dependency)
    visp_add_dependency(${cpp} "tutorials")
  endif()
endforeach()

# Copy the data files to the same location than the target
foreach(data ${tutorial_data})
  visp_copy_data(tutorial-blob-auto-tracker.cpp ${data})
endforeach()
