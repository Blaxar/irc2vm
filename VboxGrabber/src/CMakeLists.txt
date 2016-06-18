file(GLOB_RECURSE VboxGrabber_src_files *.cpp)

add_executable(VboxGrabber ${VboxGrabber_src_files})
target_link_libraries(VboxGrabber ${CMAKE_THREAD_LIBS_INIT} ${VBOX_LIBS})
set_target_properties(VboxGrabber PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DEFAULT_BIN_DIR})
