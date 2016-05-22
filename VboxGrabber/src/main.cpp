#include <stdio.h>
#include <stdlib.h>
#include <VboxGrabber.hpp>
#include <V4l2FrameBuffer.hpp>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono;
using namespace std::this_thread;

int main(int argc, char* argv[])
{

	std::cout << "Starting VM..." << std::endl;
	VboxGrabber grabber("Small Windows XP", 0, new V4l2FrameBuffer());

	uint8_t* data = (uint8_t*) malloc(800*600*4*sizeof(uint8_t));

    milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	milliseconds last = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	unsigned int nbFrames = 0;
	
	std::cout << "Starting frame grabing." << std::endl;
	
	while( (last-start).count() < 100000){
		grabber.grab(&data, 800, 600);
		last = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		nbFrames++;
	}
	
	std::cout << "Grabbed " << nbFrames << " frames in 100 seconds." << std::endl;
	free(data);
	return 0;
	
}
