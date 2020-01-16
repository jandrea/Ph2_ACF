#include "../NetworkUtils/TCPTransmitterSocket.h"
#include "../NetworkUtils/TCPPacket.h"
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <string.h>
#include <iostream>

//========================================================================================================================
TCPTransmitterSocket::TCPTransmitterSocket(int socketId)
	: TCPSocket(socketId)
{
}

//========================================================================================================================
TCPTransmitterSocket::~TCPTransmitterSocket(void)
{
}

//========================================================================================================================
void TCPTransmitterSocket::sendPacket(const std::string& buffer)
{
	send(TCPPacket::encode(buffer));
}

//========================================================================================================================
void TCPTransmitterSocket::send(char const* buffer, std::size_t size)
{
	std::size_t sentBytes = ::send(getSocketId(), buffer, size, MSG_NOSIGNAL);
	if (sentBytes == static_cast<std::size_t>(-1))
	{
		switch (errno)
		{
		//case EINVAL:
		//case EBADF:
		//case ECONNRESET:
		//case ENXIO:
		case EPIPE:
		{
			// Fatal error. Programming bug
			throw std::runtime_error(std::string("Write: critical error: ") + strerror(errno));
		}
		//case EDQUOT:
		//case EFBIG:
		//case EIO:
		//case ENETDOWN:
		//case ENETUNREACH:
		case ENOSPC:
		{
			// Resource acquisition failure or device error
			throw std::runtime_error(std::string("Write: resource failure: ") + strerror(errno));
		}
		case EINTR:
			// TODO: Check for user interrupt flags.
			//       Beyond the scope of this project
			//       so continue normal operations.
		case EAGAIN:
		{
			// Temporary error.
			throw std::runtime_error(std::string("Write: temporary error: ") + strerror(errno));
		}
		default:
		{
			throw std::runtime_error(std::string("Write: returned -1: ") + strerror(errno));
		}
		}
	}
}

//========================================================================================================================
void TCPTransmitterSocket::send(const std::string &buffer)
{
	if(buffer.size() == 0)
	{
		std::cout << __PRETTY_FUNCTION__ << "I am sorry but I won't send an empty packet!" << std::endl;
		return;
	}
	send(&buffer.at(0), buffer.size());
}

//========================================================================================================================
void TCPTransmitterSocket::send(const std::vector<char> &buffer)
{
	if(buffer.size() == 0)
	{
		std::cout << __PRETTY_FUNCTION__ << "I am sorry but I won't send an empty packet!" << std::endl;
		return;
	}
	send(&buffer.at(0), buffer.size());
}

//========================================================================================================================
void TCPTransmitterSocket::setSendTimeout(unsigned int timeoutSeconds, unsigned int timeoutMicroSeconds)
{
	struct timeval tv;
	tv.tv_sec  = timeoutSeconds;
	tv.tv_usec = timeoutMicroSeconds;
	setsockopt(getSocketId(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);

}
