#ifndef VBOXGRABBER_HPP
#define VBOXGRABBER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

/*
 * Include the XPCOM headers
 */
#include <nsMemory.h>
#include <nsString.h>
#include <nsIServiceManager.h>
#include <nsEventQueueUtils.h>

#include <nsIExceptionService.h>

#include <string>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>

/*
 * VirtualBox XPCOM interface. This header is generated
 * from IDL which in turn is generated from a custom XML format.
 */
#include <VirtualBox_XPCOM.h>

#include <V4l2device.hpp>

class VboxGrabberException : public std::exception {
  private:
    std::string err_msg;

  public:
    VboxGrabberException(const char *msg) : err_msg(msg) {};
	VboxGrabberException(const std::string msg) : err_msg(msg) {};
    ~VboxGrabberException() throw() {};
    const char *what() const throw() { return this->err_msg.c_str(); };
};

class VboxGrabber
{

    public:
	
    VboxGrabber(std::string vmName, std::string dev, uint32_t width, uint32_t height,
				uint8_t screenID = 0);
	
    ~VboxGrabber();

    uint32_t grab();

    protected:

	std::string _dev;

	PRUint32 _format;
	uint8_t _screenID;
	const uint32_t _width, _height;
	
	nsCOMPtr<nsIServiceManager> _serviceManager;
	nsCOMPtr<nsIEventQueue> _eventQ;
	nsCOMPtr<nsIComponentManager> _manager;
	nsCOMPtr<IVirtualBox> _virtualBox;
    IMachine* _machine;
    nsCOMPtr<ISession> _session;
    IConsole* _console;
	
    IDisplay* _display;
	std::unique_ptr<V4l2device> _v4l2device;

	std::mutex _doneMutex;
	uint16_t _doneThreads;

	void oneMoreDone(){_doneMutex.lock(); _doneThreads++;  _doneMutex.unlock();}
	void popDone(){
		_doneMutex.lock();
		for(int i=0; i<_doneThreads; i++){
			_threadQueue.front().join();
			_threadQueue.pop();
		}
		_doneThreads = 0;
		_doneMutex.unlock();
	}
	
	std::queue<std::thread> _threadQueue;

	void takeScreenshot();

};

#endif //VBOXGRABBER_HPP
