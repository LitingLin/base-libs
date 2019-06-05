#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Bcrypt.h>
#include <memory>
#include <vector>
#include <string>

namespace Base {
	namespace Crypto {
		class BCRYPT_ALG_HANDLE_GUARD
		{
		public:
			enum class AlgorithmType
			{
				SHA256, AES, RNG
			};
			BCRYPT_ALG_HANDLE_GUARD(AlgorithmType);
			~BCRYPT_ALG_HANDLE_GUARD();
			BCRYPT_ALG_HANDLE getHandle() const;
		private:
			BCRYPT_ALG_HANDLE handle;
		};

		class SHA256
		{
		public:
			SHA256();
			std::vector<unsigned char> hash(const std::vector<unsigned char>& data) const;
			std::vector<unsigned char> hash(const unsigned char* data, ULONG size) const;
		private:
			BCRYPT_ALG_HANDLE_GUARD _hALG;
			std::unique_ptr<unsigned char[]> _hashObject;
			DWORD _cbHashObject;
			DWORD _cbHash;
			std::unique_ptr<unsigned char[]> _hash;
		};

		class KeyObjectGuard
		{
		public:
			KeyObjectGuard(BCRYPT_ALG_HANDLE alg_handle, unsigned char* keyObject, ULONG keyObjectSize, unsigned char* key, ULONG keySize);
			~KeyObjectGuard();
			BCRYPT_KEY_HANDLE getHandle();
		private:
			BCRYPT_KEY_HANDLE _handle;
		};

		class AES128ECB
		{
		public:
			AES128ECB(const std::vector<unsigned char>& key);
			std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data);
			void encrypt(const unsigned char* data, unsigned char* buf, ULONG size);
			std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data);
			void decrypt(const unsigned char* data, unsigned char* buf, ULONG size);
			DWORD getBlockLen() const;
		private:
			BCRYPT_ALG_HANDLE_GUARD _hALG;
			ULONG _cbKeyObject;
			DWORD _cbBlockLen;
			std::unique_ptr<unsigned char[]> _keyObject;
			std::unique_ptr<KeyObjectGuard> _key;
		};

		class RNG
		{
		public:
			RNG();
			std::vector<unsigned char> getBytes(ULONG size);
		private:
			BCRYPT_ALG_HANDLE_GUARD _hALG;
		};

		std::string Base64Encode(const std::vector<unsigned char>& binary);
		std::vector<unsigned char> Base64Decode(const std::string& base64);
		std::string BinaryToHexadecimalString(const std::vector<unsigned char>& binary);
		std::vector<unsigned char> HexadecimalStringToBinary(const std::string& hex);
	}
}