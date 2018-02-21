/**
 * @file Exception.cpp
 */

#include "Exception.h"
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <errno.h>
#include <cstring>
#include <cerrno>


/**
 * class Exception
 */
Exception::Exception(const std::string& msg)
	:std::runtime_error(msg),m_stack(getStack())
{
}

Exception::~Exception() throw()
{
}

std::string Exception::stack() const
{
	return m_stack;
}

std::string Exception::getStack()
{
	enum { BFSIZE = 1024 };

	void       *buffer[BFSIZE];
	int         n = backtrace(buffer, BFSIZE);
	char      **res = backtrace_symbols(buffer, n);
	std::string tmp;

	for(int i = 0; i < n; ++i) {
		std::string x = res[i];
		std::string plus;
		std::string addr;
		size_t pos = x.find("(");
		if(pos != std::string::npos) {
			x = x.substr(pos + 1); // skip executable name

			pos = x.find(")");
			if(pos != std::string::npos) {
				addr = x.substr(pos + 1);
				x = x.substr(0, pos);
			}

			pos = x.find("+");
			if(pos != std::string::npos) {
				plus = x.substr(pos);
				x = x.substr(0, pos);
			}
		}
		// Demangling
		#if 1
		{
			int status = 0;
			char* name = abi::__cxa_demangle( x.c_str(), NULL, NULL, &status);

			if(!status) {
				tmp += name;
				tmp += plus;
				tmp += addr;
			} else {
				tmp += res[i];
			}
			tmp += "\n";
			free(name);
		}
		#endif
		// dladdr() test
		#if 0
		{
			Dl_info info;
			int n = dladdr(buffer[i], &info);
			if (!n)
				printf("%d: dladdr() failed\n", i);
			else
			{
				size_t sz = 1000;
				int status = 0;
				abi::__cxa_demangle( info.dli_sname, zz, &sz, &status);

				printf("%d: %s: %s\n", i, info.dli_fname, info.dli_sname);
				if (!status)
					printf("                %s\n", zz);
			}
		} // end of dladdr() test
		#endif
	} // for (i)
	free(res);
	return tmp;
}


/**
 * class Exception
 */
SystemException::SystemException(const std::string& msg)
	:Exception(msg + ": " + getSysError()),
	 m_errno(errno),
	 m_message(getSysError())
{
}

SystemException::~SystemException() throw()
{
}

const char* SystemException::what() const throw()
{
	return Exception::what();
}

int SystemException::error() const
{
	return m_errno;
}

std::string SystemException::message() const
{
	return m_message;
}

std::string SystemException::getSysError()
{
	char buff[512] = { '\0' };
	return std::string(strerror_r(errno, buff, sizeof(buff)));
}
