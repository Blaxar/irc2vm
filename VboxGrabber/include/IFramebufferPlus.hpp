#ifndef IFRAMEBUFFERPLUS_HPP
#define IFRAMEBUFFERPLUS_HPP

/*
 * VirtualBox XPCOM interface. This header is generated
 * from IDL which in turn is generated from a custom XML format.
 */
#include <VirtualBox_XPCOM.h>

/* Header file */
class IFramebufferPlus : public IFramebuffer
{
    public:

	IFramebufferPlus(){}

	virtual uint32_t fetch(uint8_t** data) = 0;
	
	~IFramebufferPlus(){}

};

#endif //IFRAMEBUFFERPLUS_HPP
