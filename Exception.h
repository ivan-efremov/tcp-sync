/**
 * @file Exception.h
 */

#pragma once

#include <stdexcept>


#define SOURCE_FILE_LINE    (std::string(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": "))


/**
  * class Exception
  * @short Базовый класс ошибок.
  */
class Exception: public std::runtime_error
{
	std::string  m_stack;
public:
	             Exception(const std::string& msg);
	virtual     ~Exception() throw();
	std::string  stack() const;
private:
	static std::string getStack();
};

/**
  * class Exception
  * @short Системный тип ошибок.
  */
class SystemException : public Exception
{
	int                 m_errno;
	std::string         m_message;
public:
	                    SystemException(const std::string& msg);
	virtual            ~SystemException() throw();
	virtual const char* what() const throw();
	int                 error() const;
	std::string         message() const;
private:
	static std::string  getSysError();
};
