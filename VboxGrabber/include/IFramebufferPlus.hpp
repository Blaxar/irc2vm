#ifndef IFRAMEBUFFERPLUS_HPP
#define IFRAMEBUFFERPLUS_HPP

/*
 * VirtualBox XPCOM interface. This header is generated
 * from IDL which in turn is generated from a custom XML format.
 */
#include <VirtualBox_XPCOM.h>

class IFramebufferPlusException : public std::exception {
  private:
    std::string err_msg;

  public:
    IFramebufferPlusException(const char *msg) : err_msg(msg) {};
	IFramebufferPlusException(const std::string msg) : err_msg(msg) {};
    ~IFramebufferPlusException() throw() {};
    const char *what() const throw() { return this->err_msg.c_str(); };
};

/* Header file */
class IFramebufferPlus : public IFramebuffer
{
    public:

	IFramebufferPlus(){}

	virtual uint32_t fetch(uint8_t** data) = 0;
	
	~IFramebufferPlus(){}

};

#endif //IFRAMEBUFFERPLUS_HPP
