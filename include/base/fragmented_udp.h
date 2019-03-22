#pragma once

#include "network.h"

#include <memory>
#include <map>
#include <vector>
#include <functional>

namespace Base
{
	static const uint32_t reliable_mtu = 576;

	bool operator<(const GUID& left, const GUID& right);

	class BaseFragmentedUDP
	{
	public:
		void cleanup(uint64_t timeout);
	protected:
		void base_send(const std::string &message, std::function<void(const char*, uint32_t)> send_method) const;
		std::string parse_package(const char *package, uint32_t package_size);
	private:
		struct MessageContext
		{
			std::string cache;
			std::vector<bool> received_framentation;
			uint64_t lastTimeStamp;
		};
		std::map<GUID, MessageContext> _message_cache;
	};

	class FragmentedUDPServer : public BaseFragmentedUDP
	{
	public:
		FragmentedUDPServer(std::unique_ptr<UDPServer> udp_server);
		HANDLE getReadEvent() const;
		std::string try_recvfrom(sockaddr_in *from);
		std::string recvfrom(sockaddr_in *from);
		void sendto(const std::string &message, const sockaddr_in *to);
		void reset(std::unique_ptr<UDPServer> udp_server);
	private:
		std::unique_ptr<UDPServer> _udp_server;
	};

	class FragmentedUDPClient : public BaseFragmentedUDP
	{
	public:
		FragmentedUDPClient(std::unique_ptr<UDPClient> udp_client);
		HANDLE getReadEvent() const;
		std::string try_recv();
		std::string recv();
		bool recv(uint32_t timeout, std::string &message);
		void send(const std::string &message);
		void reset(std::unique_ptr<UDPClient> udp_client);
	private:
		std::unique_ptr<UDPClient> _udp_client;
	};
}