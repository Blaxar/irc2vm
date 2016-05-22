file(GLOB_RECURSE VboxGrabber_src_files *.cpp)

add_executable(VboxGrabber ${VboxGrabber_src_files})
target_link_libraries(VboxGrabber ${FFMPEG_LIBRARIES} ${VBOX_LIBS})
set_target_properties(VboxGrabber PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DEFAULT_BIN_DIR})
