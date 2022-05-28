cmake_minimum_required(VERSION 3.2)

Project(opencv_action)

find_package(OpenCV REQUIRED)

##下面是输出信息
message(STATUS "Opnecv ;ibrary status: ")
message(STATUS "> version: ${OpenCV_VERSION} ")
message(STATUS "libraries: ${OpenCV_LIBS} ")
message(STATUS "> include: ${OpenCV_INCLUDE_DIRS}  ")


include_directories(${OpenCV_INCLUDE_DIRS}) 


# add_executable(lane_det lane_det.cpp)
# add_executable(montage montage.cpp)
# add_executable(qr_code_det qr_code_det.cpp)
# add_executable(perspective_adjust perspective_adjust.cpp)
# add_executable(replace_background replace_background.cpp)
add_executable(hu_moment_match hu_moment_match.cpp)

# target_link_libraries(lane_det ${OpenCV_LIBS})
# target_link_libraries(montage ${OpenCV_LIBS})
# target_link_libraries(qr_code_det ${OpenCV_LIBS})
# target_link_libraries(perspective_adjust ${OpenCV_LIBS})
# target_link_libraries(replace_background ${OpenCV_LIBS})
target_link_libraries(hu_moment_match ${OpenCV_LIBS})