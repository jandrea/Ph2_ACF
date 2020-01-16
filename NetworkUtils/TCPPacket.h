#ifndef _TCPPacket_h_
#define _TCPPacket_h_

#include <string>

class TCPPacket
{
public:
	TCPPacket();
	virtual ~TCPPacket(void);

	static std::string encode(const std::string &message);

	bool        decode    (std::string& message);
	//Resets the storage buffer
	void        reset     (void);

	//Operator overload
	TCPPacket &operator+=(const std::string &buffer)
	{
		this->fBuffer += buffer;
		return *this;
	}

	friend std::ostream &operator<<(std::ostream &out, const TCPPacket &packet);

private:
	static constexpr uint32_t headerLength = 4;//sizeof(uint32_t); //THIS MUST BE 4

	std::string fBuffer; //This is Header + Message
};

#endif
