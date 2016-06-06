#include <stdio.h>
#include <stdlib.h>
#include <VboxGrabber.hpp>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

string usage = " <VM name> <path/to/dev/video> <output width> <output height> <fps>"s;

int parse_args(int argc, char* argv[], string& vmName,
			   string& devPath, int& width, int& height, float& fps)
{

	if(argc != 6){cerr<<"Not enough parameters."<<endl<<"usage: "<<argv[0]<<usage<<endl; return -1;}

	vmName = argv[1];
	devPath = argv[2];
	width = atoi(argv[3]); if(width <= 0){cerr<<"Invalid width."<<endl<<"usage: "<<argv[0]<<usage<<endl; return -1;}
	height = atoi(argv[4]); if(height <= 0){cerr<<"Invalid height."<<endl<<"usage: "<<argv[0]<<usage<<endl; return -1;}
    fps = atof(argv[5]); if(fps <= 0){cerr<<"Invalid fps."<<endl<<"usage: "<<argv[0]<<usage<<endl; return -1;}
	return 1;
}

int main(int argc, char* argv[])
{

	string vmName; string devPath; int width; int height; float fps;
	if(parse_args(argc, argv, vmName, devPath, width, height, fps)<0) return -1;
	
	std::cout << "Starting VM..." << std::endl;

	try{
	    VboxGrabber grabber(vmName, devPath, width, height);
	
	
		milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		milliseconds last = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		unsigned int nbFrames = 0;
	
		std::cout << "Starting frame grabing." << std::endl;
	
		while( (last-start) < 100s){
			grabber.grab();
			last = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			nbFrames++;
		}

		std::cout << "Grabbed " << nbFrames << " frames in 100 seconds." << std::endl;

	}catch(exception& e){
		cerr << e.what() << endl;
		return -1;
	}
	
	return 0;
	
}
