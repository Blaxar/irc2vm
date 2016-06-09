#include <stdio.h>
#include <stdlib.h>
#include <VboxGrabber.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include <signal.h>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

sig_atomic_t _stop = 0;

string usage = " <VM name> <path/to/dev/video> <output width> <output height> <fps>"s;

void sigint_handler(int param)
{
  _stop = 1;
}

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

	signal(SIGINT, sigint_handler);

	string vmName; string devPath; int width; int height; float fps;
	if(parse_args(argc, argv, vmName, devPath, width, height, fps)<0) return -1;

	milliseconds frame_interval((int)(1000/fps));

	try{
	    VboxGrabber grabber(vmName, devPath, width, height);
	
	
		milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		milliseconds prev, last;
	    uint64_t nbFrames = 0;
	
		std::cout << "Starting frame grabing." << std::endl;
	
		while(_stop==0){
			prev = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		    grabber.grab();
			last = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			std::this_thread::sleep_for(frame_interval-(last-prev));
			nbFrames++;
		}

		std::cout << "Grabbed " << nbFrames << " frames in "<< duration_cast<seconds>(last-start).count() <<" seconds." << std::endl;

	}catch(exception& e){
		cerr << e.what() << endl;
		return -1;
	}
	
	return 0;
	
}
