#include <base/network.h>
#include <base/logging.h>

#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <limits>

namespace Base
{
	WSAGuard::WSAGuard()
	{
		ENSURE_EQ_WIN32API(WSAStartup(MAKEWORD(2, 2), &wsaData), 0);
	}

	WSAGuard::~WSAGuard()
	{
		LOG_IF_NOT_EQ_WIN32API(WSACleanup(), 0);
	}

	class AddrInfoGuard
	{
	public:
		AddrInfoGuard(const wchar_t *serverAddress, const wchar_t *service, const ADDRINFOW *hint)
		{
			CHECK_EQ_WIN32API(GetAddrInfo(serverAddress, service, hint, &_result), 0);
		}
		const ADDRINFOW *get()
		{
			return _result;
		}
		~AddrInfoGuard()
		{
			FreeAddrInfo(_result);
		}
	private:
		ADDRINFOW * _result;
	};


	std::ostream& operator<<(std::ostream &os, net_rc rc)
	{
		switch (rc)
		{
		case net_rc::ok:
			os << "ok";
			break;
		case net_rc::cancelled:
			os << "cancelled";
			break;
		case net_rc::connreset:
			os << "connreset";
			break;
		case net_rc::wouldblock:
			os << "wouldblock";
			break;
		case net_rc::netfail:
			os << "netfail";
			break;
		case net_rc::truncated:
			os << "truncated";
			break;
		case net_rc::timeout:
			os << "timeout";
			break;
		case net_rc::unreachable:
			os << "unreachable";
			break;
		default:
			UNREACHABLE_ERROR;
			break;
		}
		return os;
	}


	SOCKETGuard::SOCKETGuard(int af, int type, int protocol) : SOCKETGuard()
	{
		initialize(af, type, protocol);
	}

	SOCKETGuard::SOCKETGuard() : _socket(INVALID_SOCKET), _readEvent(WSA_INVALID_EVENT)
	{
	}

	SOCKETGuard::~SOCKETGuard()
	{
		destroy();
	}

	void SOCKETGuard::initialize(int af, int type, int protocol)
	{
		destroy();
		_socket = ::socket(af, type, protocol);
		CHECK_NE_WIN32API(_socket, INVALID_SOCKET);
	}

	void SOCKETGuard::setopt(int level, int optname, bool value)
	{
		BOOL option = value;
		CHECK_EQ_WIN32API(::setsockopt(_socket, level, optname, (char*)&option, sizeof(BOOL)), 0)
			<< "level: " << level << ", optname: " << optname << ", value: " << value;
	}

	void SOCKETGuard::ioctl(long cmd, unsigned long *argp)
	{
		CHECK_EQ_WIN32API(::ioctlsocket(_socket, cmd, argp), 0);
	}

	net_rc SOCKETGuard::connect(const sockaddr * name, int namelen)
	{
		int socketrc = ::connect(_socket, name, namelen);
		return checkSocketRc(socketrc, "connect");
	}

	void SOCKETGuard::bind(const sockaddr * name, int namelen)
	{
		int rc = ::bind(_socket, name, namelen);
		if (rc == 0)
			return;
		if (WSAGetLastError() == WSAEADDRINUSE)
			throw NetworkAddressInUseException(getWin32LastErrorString());
		CHECK_EQ_WIN32API(WSAGetLastError(), 0);
	}

	net_rc SOCKETGuard::recvfrom(char *buf, int len, sockaddr *from, int *fromlen, int *recvlen)
	{
		int socketrc = ::recvfrom(_socket, buf, len, 0, from, fromlen);
		*recvlen = socketrc;
		return checkSocketRc(socketrc, "recvfrom");
	}

	net_rc SOCKETGuard::sendto(const char *buf, int len, const sockaddr *to, int tolen, int *sendlen)
	{
		int socketrc = ::sendto(_socket, buf, len, 0, to, tolen);
		*sendlen = socketrc;
		return checkSocketRc(socketrc, "sendto");
	}

	net_rc SOCKETGuard::recv(char *buf, int size, int *recvlen)
	{
		int socketrc = ::recv(_socket, buf, size, 0);
		*recvlen = socketrc;
		return checkSocketRc(socketrc, "recv");
	}

	net_rc SOCKETGuard::send(const char *buf, int size, int *sendlen)
	{
		int socketrc = ::send(_socket, buf, size, 0);
		*sendlen = socketrc;
		return checkSocketRc(socketrc, "send");
	}

	bool SOCKETGuard::select(select_t select_type, uint32_t timeout)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(_socket, &fds);


		fd_set *readfds_ptr = nullptr;
		fd_set *writefds_ptr = nullptr;
		fd_set *exceptfds_ptr = nullptr;

		if (select_type == select_t::read)
			readfds_ptr = &fds;
		else if (select_type == select_t::write)
			writefds_ptr = &fds;
		else if (select_type == select_t::except)
			exceptfds_ptr = &fds;
		else
			UNREACHABLE_ERROR;

		struct timeval timeval;
		timeval.tv_sec = timeout / 1000;
		timeval.tv_usec = timeout % 1000;
		int rc = ::select(0, readfds_ptr, writefds_ptr, exceptfds_ptr, &timeval);
		CHECK_NE_WIN32API(rc, SOCKET_ERROR);
		return rc > 0;
	}

	uint16_t SOCKETGuard::get_port() const
	{
		sockaddr_in addr;
		int addr_len = sizeof(sockaddr);
		CHECK_EQ_WIN32API(getsockname(_socket, (sockaddr*)&addr, &addr_len), 0);
		return addr.sin_port;
	}

	SOCKET SOCKETGuard::get()
	{
		return _socket;
	}

	void SOCKETGuard::destroy()
	{
		if (_readEvent != WSA_INVALID_EVENT) {
			LOG_IF_FAILED_WIN32API(CloseHandle(_readEvent));
		}
		if (_socket != INVALID_SOCKET) {
			LOG_IF_NOT_EQ_WIN32API(closesocket(_socket), 0);
		}
	}

	HANDLE SOCKETGuard::getReadEvent()
	{
		if (!_readEvent) {
			_readEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			ENSURE_WIN32API(_readEvent);
			ENSURE_EQ_WIN32API(WSAEventSelect(_socket, _readEvent, FD_READ), 0);
		}
		return _readEvent;
	}

	net_rc SOCKETGuard::WSAErrorCodeToReturnCode(int WSAErrorCode)
	{
		if (WSAErrorCode == WSAENETRESET)
			return net_rc::netfail;
		else if (WSAErrorCode == WSAEWOULDBLOCK)
			return net_rc::wouldblock;
		else if (WSAErrorCode == WSAEMSGSIZE)
			return net_rc::truncated;
		else if (WSAErrorCode == WSAETIMEDOUT)
			return net_rc::timeout;
		else if (WSAErrorCode == WSAECONNRESET)
			return net_rc::connreset;
		else if (WSAErrorCode == WSAEINTR)
			return net_rc::cancelled;
		else if (WSAErrorCode == WSAENETUNREACH)
			return net_rc::unreachable;
		else
			UNREACHABLE_ERROR;

		return net_rc::ok;
	}

	bool SOCKETGuard::WSAErrorIsAcceptable(int WSAErrorCode)
	{
		int acceptableErrorCodes[] = {
			WSAENETRESET, WSAEWOULDBLOCK, WSAEMSGSIZE,
			WSAETIMEDOUT, WSAECONNRESET, WSAEINTR,
			WSAENETUNREACH
		};
		for (size_t i = 0; i < sizeof(acceptableErrorCodes) / sizeof(int); ++i)
		{
			if (WSAErrorCode == acceptableErrorCodes[i])
				return true;
		}
		return false;
	}

	net_rc SOCKETGuard::checkSocketRc(int socketrc, const char *func_name)
	{
		net_rc rc = net_rc::ok;
		if (socketrc == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			CHECK_WIN32API(WSAErrorIsAcceptable(errorCode)) << "error in WSA function " << func_name << ".\n";
			rc = WSAErrorCodeToReturnCode(errorCode);
		}
		return rc;
	}


	struct sockaddr buildIPv4SockAddr(uint16_t port, in_addr addr)
	{
		struct sockaddr sockaddr;
		memset(&sockaddr, 0, sizeof(sockaddr));
		sockaddr_in *sockaddrPtr = (sockaddr_in *)&sockaddr;
		sockaddrPtr->sin_family = AF_INET;
		sockaddrPtr->sin_port = port;
		sockaddrPtr->sin_addr = addr;

		return sockaddr;
	}

	void parseIPv4SockAddr(const struct sockaddr *sockaddr, uint16_t *port, in_addr *addr)
	{
		const sockaddr_in *sockaddr_ptr = (const sockaddr_in *)sockaddr;
		ENSURE_EQ(sockaddr_ptr->sin_family, AF_INET);
		*port = sockaddr_ptr->sin_port;
		*addr = sockaddr_ptr->sin_addr;
	}

	sockaddr_in getIPv4UDPSockAddr(const wchar_t* address, uint16_t port)
	{
		ADDRINFOW hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		AddrInfoGuard addrinfoGuard(address, 0, &hints);
		const ADDRINFOW *result = addrinfoGuard.get();
		((sockaddr_in*const)result->ai_addr)->sin_port = port;
		return *((sockaddr_in*)result->ai_addr);
	}

	UDPServer::UDPServer(const wchar_t *bindAddress, uint16_t port)
	{
		ADDRINFOW hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		AddrInfoGuard addrinfoGuard(bindAddress, 0, &hints);
		const ADDRINFOW *result = addrinfoGuard.get();
		((sockaddr_in*const)result->ai_addr)->sin_port = port;
		socket.initialize(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		//socket.setopt(IPPROTO_UDP, UDP_CHECKSUM_COVERAGE, true); // Enable checksum
		socket.setopt(IPPROTO_UDP, UDP_NOCHECKSUM, false); // Enable checksum
		socket.setopt(IPPROTO_IP, IP_DONTFRAGMENT, false); // Enable fragment
		unsigned long mode = 1;
		socket.ioctl(FIONBIO, &mode);
		socket.bind(result->ai_addr, int(result->ai_addrlen));
	}

	bool UDPServer::recvfrom(char *buf, int len, sockaddr_in *from, int *fromlen, uint32_t timeout, int *recvlen)
	{
		if (socket.select(SOCKETGuard::select_t::read, timeout))
			return try_recvfrom(buf, len, from, fromlen, recvlen);
		else
			return false;
	}

	void UDPServer::recvfrom(char* buf, int len, sockaddr_in* from, int* fromlen, int* recvlen)
	{
		while (!recvfrom(buf, len, from, fromlen, INFINITE, recvlen));
	}

	bool UDPServer::try_recvfrom(char* buf, int len, sockaddr_in* from, int* fromlen, int* recvlen)
	{
		net_rc rc = socket.recvfrom(buf, len, (sockaddr *)from, fromlen, recvlen);
		if (rc == net_rc::wouldblock)
			return false;
		CHECK_EQ_WIN32API(rc, net_rc::ok);
		return true;
	}

	bool UDPServer::sendto(const char * buf, int len, const sockaddr_in * to, int tolen, uint32_t timeout, int *sendlen)
	{
		if (socket.select(SOCKETGuard::select_t::write, timeout))
			return try_sendto(buf, len, to, tolen, sendlen);
		else
			return false;
	}

	bool UDPServer::try_sendto(const std::string& buf, const sockaddr_in* to, int* sendlen)
	{
		return try_sendto(buf.c_str(), int(buf.size()), to, sizeof(sockaddr_in), sendlen);
	}

	uint16_t UDPServer::get_port() const
	{
		return socket.get_port();
	}

	bool UDPServer::sendto(const std::string &buf, const sockaddr_in * to, uint32_t timeout, int *sendlen)
	{
		return sendto(buf.c_str(), int(buf.size()), to, sizeof(sockaddr_in), timeout, sendlen);
	}

	void UDPServer::sendto(const char* buf, int len, const sockaddr_in* to, int tolen, int* sendlen)
	{
		while (!sendto(buf, len, to, tolen, INFINITE, sendlen));
	}
	void UDPServer::sendto(const std::string &buf, const sockaddr_in* to, int* sendlen)
	{
		sendto(buf.c_str(), int(buf.size()), to, sizeof(sockaddr_in), sendlen);
	}

	bool UDPServer::try_sendto(const char* buf, int len, const sockaddr_in* to, int tolen, int* sendlen)
	{
		net_rc rc = socket.sendto(buf, len, (const sockaddr *)to, tolen, sendlen);
		if (rc == net_rc::wouldblock)
			return false;
		CHECK_EQ_WIN32API(rc, net_rc::ok);
		return true;
	}

	HANDLE UDPServer::getReadEvent()
	{
		return socket.getReadEvent();
	}

	UDPClient::UDPClient(const wchar_t *serverAddress, uint16_t port)
	{
		ADDRINFOW hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		AddrInfoGuard addrinfoGuard(serverAddress, 0, &hints);
		const ADDRINFOW *result = addrinfoGuard.get();
		((sockaddr_in*const)result->ai_addr)->sin_port = port;
		socket.initialize(result->ai_family, result->ai_socktype, result->ai_protocol);
		//socket.setopt(IPPROTO_UDP, UDP_CHECKSUM_COVERAGE, true); // Enable checksum
		socket.setopt(IPPROTO_UDP, UDP_NOCHECKSUM, false); // Enable checksum
		socket.setopt(IPPROTO_IP, IP_DONTFRAGMENT, false); // Enable fragment
		unsigned long mode = 1;
		socket.ioctl(FIONBIO, &mode);
		in_addr bindAddr;
		bindAddr.s_addr = INADDR_ANY;
		sockaddr bindaddr = buildIPv4SockAddr(0, bindAddr);
		socket.bind(&bindaddr, sizeof(bindaddr));
		CHECK_LE(result->ai_addrlen, (size_t)std::numeric_limits<int>::max());
		socket.connect(result->ai_addr, (int)result->ai_addrlen);
	}

	bool UDPClient::recv(char * data, int size, uint32_t timeout, int *recvlen)
	{
		if (socket.select(SOCKETGuard::select_t::read, timeout))
			return try_recv(data, size, recvlen);
		else
			return false;
	}

	void UDPClient::recv(char* data, int size, int* recvlen)
	{
		while (!recv(data, size, INFINITE, recvlen));
	}

	bool UDPClient::try_recv(char* data, int size, int* recvlen)
	{
		const net_rc rc = socket.recv(data, size, recvlen);
		if (rc == net_rc::wouldblock)
			return false;
		CHECK_EQ_WIN32API(rc, net_rc::ok);
		return true;
	}

	bool UDPClient::send(const char * data, int size, uint32_t timeout, int *sendlen)
	{
		if (socket.select(SOCKETGuard::select_t::write, timeout))
			return try_send(data, size, sendlen);
		else
			return false;
	}

	void UDPClient::send(const char* data, int size, int* sendlen)
	{
		while (!send(data, size, INFINITE, sendlen));
	}

	bool UDPClient::try_send(const char* data, int size, int* sendlen)
	{
		const net_rc rc = socket.send(data, size, sendlen);
		if (rc == net_rc::wouldblock)
			return false;
		CHECK_EQ_WIN32API(rc, net_rc::ok);
		return true;
	}

	HANDLE UDPClient::getReadEvent()
	{
		return socket.getReadEvent();
	}
}