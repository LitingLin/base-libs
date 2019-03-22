#include <base/fragmented_udp.h>

#include <base/utils.h>
#include <base/sync.h>

namespace Base
{
	void BaseFragmentedUDP::cleanup(uint64_t timeout)
	{
		uint64_t timestamp = GetTickCount64();
		auto it = _message_cache.begin();
		while (it != _message_cache.end()) {
			if (timestamp - it->second.lastTimeStamp > timeout)
				_message_cache.erase(it++);
			else
				++it;
		}
	}

	void BaseFragmentedUDP::base_send(const std::string& message, std::function<void(const char*, uint32_t)> send_method) const
	{
		char buf[reliable_mtu];
		GUID *guid = (GUID*)buf;
		generateGUID(guid);
		uint64_t *index = (uint64_t*)(buf + sizeof(GUID));
		uint64_t *total_packages = (uint64_t*)(buf + sizeof(GUID) + sizeof(uint64_t));
		char *payload = buf + sizeof(GUID) + sizeof(uint64_t) + sizeof(uint64_t);

		const uint32_t payload_size = reliable_mtu - sizeof(GUID) - sizeof(uint64_t) - sizeof(uint64_t);

		*total_packages = (message.size() + payload_size - 1) / payload_size;

		for (uint64_t i = 0; i < *total_packages; ++i)
		{
			*index = i;
			size_t offset = size_t(i*payload_size);
			const char *iter = &message[offset];

			size_t this_package_payload_size = message.size() - offset;
			if (this_package_payload_size > payload_size)
				this_package_payload_size = payload_size;

			memcpy(payload, iter, this_package_payload_size);

			uint32_t this_package_size = uint32_t(this_package_payload_size) + sizeof(GUID) + sizeof(uint64_t) + sizeof(uint64_t);

			send_method(buf, this_package_size);
		}
	}

	std::string BaseFragmentedUDP::parse_package(const char* package, uint32_t package_size)
	{
		if (package_size <= sizeof(GUID) + sizeof(uint64_t) + sizeof(uint64_t))
			return std::string();

		const GUID *guid = (const GUID*)package;
		const uint64_t index = *((const uint64_t*)(package + sizeof(GUID)));
		const uint64_t total_packages = *((const uint64_t*)(package + sizeof(GUID) + sizeof(uint64_t)));

		if (index >= total_packages)
			return std::string();

		if (index != total_packages - 1 && package_size != reliable_mtu)
			return std::string();

		const uint32_t payload_size = reliable_mtu - sizeof(GUID) - sizeof(uint64_t) - sizeof(uint64_t);

		std::map<GUID, MessageContext>::iterator iter = _message_cache.find(*guid);

		if (iter == _message_cache.end())
		{
			auto rv = _message_cache.insert(std::pair<GUID, MessageContext>{
				*guid, {
					std::string(size_t(payload_size * total_packages), 0),
					std::vector<bool>(size_t(total_packages), false),
					0 }});
			iter = rv.first;
		}

		auto &messageContext = iter->second;
		if (total_packages != messageContext.received_framentation.size())
			return std::string();

		messageContext.lastTimeStamp = GetTickCount64();
		if (messageContext.received_framentation[size_t(index)])
			return std::string();
		messageContext.received_framentation[size_t(index)] = true;
		const char *payload = package + sizeof(GUID) + sizeof(uint64_t) + sizeof(uint64_t);
		uint32_t this_package_payload_size = payload_size - (reliable_mtu - package_size);
		if (index == total_packages - 1)
		{
			messageContext.cache = messageContext.cache.substr(0, size_t((total_packages - 1) * payload_size + this_package_payload_size));
		}

		char *cache = &messageContext.cache[size_t(index * payload_size)];
		memcpy(cache, payload, this_package_payload_size);

		bool isFullyReceived = true;
		for (bool fragementation_received : messageContext.received_framentation)
		{
			if (!fragementation_received)
			{
				isFullyReceived = false;
				break;
			}
		}
		if (isFullyReceived)
		{
			std::string message = messageContext.cache;
			_message_cache.erase(iter);
			return message;
		}
		return std::string();
	}

	FragmentedUDPServer::FragmentedUDPServer(std::unique_ptr<UDPServer> udp_server)
		: _udp_server(std::move(udp_server))
	{
	}

	HANDLE FragmentedUDPServer::getReadEvent() const
	{
		return _udp_server->getReadEvent();
	}

	std::string FragmentedUDPServer::try_recvfrom(sockaddr_in* from)
	{
		char buf[reliable_mtu];
		int from_len = sizeof(sockaddr_in);
		int recv_len;
		try
		{
			if (!_udp_server->try_recvfrom(buf, reliable_mtu, from, &from_len, &recv_len))
				return std::string();
		}
		catch (...)
		{
			return std::string();
		}
		return parse_package(buf, recv_len);
	}

	std::string FragmentedUDPServer::recvfrom(sockaddr_in* from)
	{
		while (true)
		{
			waitForObject(getReadEvent());
			std::string message = try_recvfrom(from);
			if (!message.empty())
				return message;
		}
	}

	void FragmentedUDPServer::sendto(const std::string& message, const sockaddr_in* to)
	{
		std::function<void(const char*, uint32_t)> send_method = [&](const char* package, uint32_t package_size)
		{
			int to_len = sizeof(sockaddr_in);
			int sent_len;
			_udp_server->sendto(package, package_size, to, to_len, &sent_len);
		};

		base_send(message, send_method);
	}

	void FragmentedUDPServer::reset(std::unique_ptr<UDPServer> udp_server)
	{
		_udp_server = std::move(udp_server);
		cleanup(0);
	}

	FragmentedUDPClient::FragmentedUDPClient(std::unique_ptr<UDPClient> udp_client)
		: _udp_client(std::move(udp_client))
	{
	}

	HANDLE FragmentedUDPClient::getReadEvent() const
	{
		return _udp_client->getReadEvent();
	}

	std::string FragmentedUDPClient::try_recv()
	{
		char buf[reliable_mtu];
		int recv_len;
		if (!_udp_client->try_recv(buf, reliable_mtu, &recv_len))
			return std::string();

		return parse_package(buf, recv_len);
	}

	std::string FragmentedUDPClient::recv()
	{
		while (true)
		{
			waitForObject(getReadEvent());
			std::string message = try_recv();
			if (!message.empty())
				return message;
		}
	}

	bool FragmentedUDPClient::recv(uint32_t timeout, std::string &message)
	{
		while (true)
		{
			if (!waitForObject_timeout(getReadEvent(), timeout))
				return false;

			message = try_recv();
			if (!message.empty())
				return true;
		}
	}

	void FragmentedUDPClient::send(const std::string& message)
	{
		if (message.empty())
			return;

		std::function<void(const char*, uint32_t)> send_method = [&](const char* package, uint32_t package_size)
		{
			int sent_len;
			_udp_client->send(package, package_size, &sent_len);
		};

		base_send(message, send_method);
	}

	void FragmentedUDPClient::reset(std::unique_ptr<UDPClient> udp_client)
	{
		_udp_client = std::move(udp_client);
		cleanup(0);
	}
}