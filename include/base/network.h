#pragma once

#include <ostream>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <base/exception.h>

namespace Base
{
    class ATTRIBUTE_INTERFACE NetworkAddressInUseException : public RuntimeException
    {
    public:
        NetworkAddressInUseException(const std::string& _Message)
                : RuntimeException(_Message, WSAEADDRINUSE, ErrorCodeType::WIN32API)
        {
        }

        NetworkAddressInUseException(const char* _Message)
                : RuntimeException(_Message, WSAEADDRINUSE, ErrorCodeType::WIN32API)
        {
        }
    };

	enum class net_rc {
		ok, cancelled, connreset, wouldblock, netfail, truncated, timeout, unreachable
	};

	ATTRIBUTE_INTERFACE
	std::ostream& operator<<(std::ostream &os, net_rc rc);

	class WSAGuard
	{
	public:
		WSAGuard();
		~WSAGuard();
	private:
		WSADATA wsaData;
	};

	class ATTRIBUTE_INTERFACE SOCKETGuard
	{
	public:
		SOCKETGuard();
		SOCKETGuard(int af, int type, int protocol);
		~SOCKETGuard();
		void initialize(int af, int type, int protocol);
		void setopt(int level, int optname, bool value);
		void ioctl(long cmd, unsigned long *argp);
		net_rc connect(const sockaddr * name, int namelen);
		void bind(const sockaddr * name, int namelen);
		net_rc recvfrom(char *buf, int len, sockaddr *from, int *fromlen, int *recvlen);
		net_rc sendto(const char *buf, int len, const sockaddr *to, int tolen, int *sendlen);
		net_rc recv(char *buf, int size, int *recvlen);
		net_rc send(const char *buf, int size, int *sendlen);
		enum class select_t { read, write, except };
		bool select(select_t select_type, uint32_t timeout);
		uint16_t get_port() const;
		SOCKET get();
		void destroy();
		HANDLE getReadEvent();
	private:
		net_rc WSAErrorCodeToReturnCode(int WSAErrorCode);
		bool WSAErrorIsAcceptable(int WSAErrorCode);
		net_rc checkSocketRc(int socketrc, const char *func_name);
		SOCKET _socket;
		HANDLE _readEvent;
	};

	ATTRIBUTE_INTERFACE
	sockaddr_in getIPv4UDPSockAddr(const wchar_t* address, uint16_t port);

	class ATTRIBUTE_INTERFACE UDPServer
	{
	public:
		UDPServer(const wchar_t *bindAddress, uint16_t port);
		bool recvfrom(char *buf, int len, sockaddr_in *from, int *fromlen, uint32_t timeout, int *recvlen);
		void recvfrom(char *buf, int len, sockaddr_in *from, int *fromlen, int *recvlen);
		bool try_recvfrom(char *buf, int len, sockaddr_in *from, int *fromlen, int *recvlen);
		bool sendto(const char *buf, int len, const sockaddr_in *to, int tolen, uint32_t timeout, int *sendlen);
		bool sendto(const std::string& buf, const sockaddr_in* to, uint32_t timeout, int* sendlen);
		void sendto(const char *buf, int len, const sockaddr_in *to, int tolen, int *sendlen);
		void sendto(const std::string &buf, const sockaddr_in* to, int* sendlen);
		bool try_sendto(const char *buf, int len, const sockaddr_in *to, int tolen, int *sendlen);
		bool try_sendto(const std::string &buf, const sockaddr_in *to, int *sendlen);
		uint16_t get_port() const;
		HANDLE getReadEvent();
	private:
		WSAGuard _WSAGuard;
		SOCKETGuard socket;
	};
		
	class ATTRIBUTE_INTERFACE UDPClient
	{
	public:
		UDPClient(const wchar_t *serverAddress, uint16_t port);
		bool recv(char *data, int size, uint32_t timeout, int *recvlen);
		void recv(char *data, int size, int *recvlen);
		bool try_recv(char *data, int size, int *recvlen);
		bool send(const char *data, int size, uint32_t timeout, int *sendlen);
		void send(const char *data, int size, int *sendlen);
		bool try_send(const char *data, int size, int *sendlen);
		HANDLE getReadEvent();
	private:
		WSAGuard _WSAGuard;
		SOCKETGuard socket;
	};
}