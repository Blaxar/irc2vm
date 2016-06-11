#ifndef FRAMEFETCHER_HPP
#define FRAMEFETCHER_HPP

#include <string>

class FrameFetcherException : public std::exception {
  private:
    std::string err_msg;

  public:
    FrameFetcherException(const char *msg) : err_msg(msg) {};
	FrameFetcherException(const std::string msg) : err_msg(msg) {};
    ~FrameFetcherException() throw() {};
    const char *what() const throw() { return this->err_msg.c_str(); };
};

/* Header file */
class FrameFetcher
{
    public:

	FrameFetcher(){}

	virtual uint32_t fetch(uint8_t** data) = 0;
	
	~FrameFetcher(){}
	

};

#endif //FRAMEFETCHER_HPP
