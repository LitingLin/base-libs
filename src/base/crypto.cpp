#define WIN32_NO_STATUS
#include <base/crypto.h>

#include <base/logging.h>
#include <wincrypt.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")

namespace Base
{
	namespace Crypto
	{
		std::string BCryptOpenAlgorithmProviderErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_NOT_FOUND)
				return "No provider was found for the specified algorithm ID.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else if (status == STATUS_NO_MEMORY)
				return "A memory allocation failure occurred.";
			else
				return "Unknown NTSTATUS error";
		}

		BCRYPT_ALG_HANDLE_GUARD::BCRYPT_ALG_HANDLE_GUARD(AlgorithmType type)
		{
			if (type == AlgorithmType::SHA256)
			{
				L_ENSURE_NTSTATUS(BCryptOpenAlgorithmProvider(&handle, BCRYPT_SHA256_ALGORITHM, NULL, 0)) << BCryptOpenAlgorithmProviderErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			}
			else if (type == AlgorithmType::AES)
			{
				L_ENSURE_NTSTATUS(BCryptOpenAlgorithmProvider(&handle, BCRYPT_AES_ALGORITHM, NULL, 0)) << BCryptOpenAlgorithmProviderErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			}
			else if (type == AlgorithmType::RNG)
			{
				L_ENSURE_NTSTATUS(BCryptOpenAlgorithmProvider(&handle, BCRYPT_RNG_ALGORITHM, NULL, 0)) << BCryptOpenAlgorithmProviderErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			}
		}

		std::string BCryptCloseAlgorithmProviderErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The algorithm handle in the hAlgorithm parameter is not valid.";
			else
				return "Unknown NTSTATUS error";
		}

		BCRYPT_ALG_HANDLE_GUARD::~BCRYPT_ALG_HANDLE_GUARD()
		{
			L_LOG_IF_FAILED_NTSTATUS(BCryptCloseAlgorithmProvider(handle, 0)) << BCryptCloseAlgorithmProviderErrorString(LOG_GET_LEFT_EXPRESSION_RC);
		}

		BCRYPT_ALG_HANDLE BCRYPT_ALG_HANDLE_GUARD::getHandle() const
		{
			return handle;
		}

		std::string BCryptGetPropertyErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_BUFFER_TOO_SMALL)
				return "The buffer size specified by the cbOutput parameter is not large enough to hold the property value.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The handle in the hObject parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else if (status == STATUS_NOT_SUPPORTED)
				return "The named property specified by the pszProperty parameter is not supported.";
			else
				return "Unknown NTSTATUS error";
		}

		SHA256::SHA256()
			: _hALG(BCRYPT_ALG_HANDLE_GUARD::AlgorithmType::SHA256)
		{
			ULONG cbData = 0;
			//calculate the size of the buffer to hold the hash object
			L_ENSURE_NTSTATUS(BCryptGetProperty(
				_hALG.getHandle(),
				BCRYPT_OBJECT_LENGTH,
				(PBYTE)& _cbHashObject,
				sizeof(DWORD),
				&cbData,
				0)) << BCryptGetPropertyErrorString(LOG_GET_LEFT_EXPRESSION_RC);

			_hashObject.reset(new unsigned char[_cbHashObject]);

			//calculate the length of the hash
			L_ENSURE_NTSTATUS(BCryptGetProperty(
				_hALG.getHandle(),
				BCRYPT_HASH_LENGTH,
				(PBYTE)& _cbHash,
				sizeof(DWORD),
				&cbData,
				0)) << BCryptGetPropertyErrorString(LOG_GET_LEFT_EXPRESSION_RC);

			_hash.reset(new unsigned char[_cbHash]);
		}

		std::string BCryptCreateHashErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_BUFFER_TOO_SMALL)
				return "The size of the hash object specified by the cbHashObject parameter is not large enough to hold the hash object.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The algorithm handle in the hAlgorithm parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else if (status == STATUS_NOT_SUPPORTED)
				return "The algorithm provider specified by the hAlgorithm parameter does not support the hash interface.";
			else
				return "Unknown NTSTATUS error";
		}

		std::string BCryptDestroyHashErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The algorithm handle in the hHash parameter is not valid.";
			else
				return "Unknown NTSTATUS error";
		}

		class HashObjectGuard
		{
		public:
			HashObjectGuard(BCRYPT_ALG_HANDLE alg_handle, unsigned char* hash_object, DWORD hash_object_size)
			{
				L_ENSURE_NTSTATUS(BCryptCreateHash(alg_handle, &hash_handle, hash_object, hash_object_size, NULL, 0, 0)) << BCryptCreateHashErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			}
			~HashObjectGuard()
			{
				L_LOG_IF_FAILED_NTSTATUS(BCryptDestroyHash(hash_handle)) << BCryptDestroyHashErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			}
			BCRYPT_HASH_HANDLE getHandle() const
			{
				return hash_handle;
			}
		private:
			BCRYPT_HASH_HANDLE hash_handle;
		};

		std::string BCryptHashDataErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The hash handle in the hHash parameter is not valid. After the BCryptFinishHash function has been called for a hash handle, that handle cannot be reused.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else
				return "Unknown NTSTATUS error";
		}

		std::string BCryptFinishHashErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The hash handle in the hHash parameter is not valid. After the BCryptFinishHash function has been called for a hash handle, that handle cannot be reused.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid. This includes the case where cbOutput is not the same size as the hash.";
			else
				return "Unknown NTSTATUS error";
		}


		std::vector<unsigned char> SHA256::hash(const std::vector<unsigned char>& data) const
		{
			L_CHECK_LT(data.size(), std::numeric_limits<ULONG>::max());
			return hash(data.data(), (ULONG)data.size());
		}

		std::vector<unsigned char> SHA256::hash(const unsigned char* data, ULONG size) const
		{
			HashObjectGuard hash_object(_hALG.getHandle(), _hashObject.get(), _cbHashObject);

			//hash some data
			L_ENSURE_NTSTATUS(BCryptHashData(
				hash_object.getHandle(),
				(PBYTE)data,
				size,
				0)) << BCryptHashDataErrorString(LOG_GET_LEFT_EXPRESSION_RC);

			//close the hash
			L_ENSURE_NTSTATUS(BCryptFinishHash(
				hash_object.getHandle(),
				_hash.get(),
				_cbHash,
				0)) << BCryptFinishHashErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			std::vector<unsigned char> hash_string(_cbHash);
			memcpy(hash_string.data(), _hash.get(), _cbHash);

			return hash_string;
		}

		std::string BCryptSetPropertyErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The handle in the hObject parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else if (status == STATUS_NOT_SUPPORTED)
				return "The named property specified by the pszProperty parameter is not supported or is read-only.";
			else
				return "Unknown NTSTATUS error";
		}

		AES128ECB::AES128ECB(const std::vector<unsigned char>& key)
			: _hALG(BCRYPT_ALG_HANDLE_GUARD::AlgorithmType::AES)
		{
			L_CHECK_LT(key.size(), std::numeric_limits<ULONG>::max());
			ULONG cbData = 0;
			L_ENSURE_NTSTATUS(BCryptGetProperty(
				_hALG.getHandle(),
				BCRYPT_OBJECT_LENGTH,
				(PBYTE)& _cbKeyObject,
				sizeof(ULONG),
				&cbData,
				0)) << BCryptGetPropertyErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			_keyObject.reset(new unsigned char[_cbKeyObject]);

			L_ENSURE_NTSTATUS(BCryptGetProperty(
				_hALG.getHandle(),
				BCRYPT_BLOCK_LENGTH,
				(PBYTE)& _cbBlockLen,
				sizeof(DWORD),
				&cbData,
				0)) << BCryptGetPropertyErrorString(LOG_GET_LEFT_EXPRESSION_RC);

			L_ENSURE_NTSTATUS(BCryptSetProperty(_hALG.getHandle(),
				BCRYPT_CHAINING_MODE,
				(PBYTE)BCRYPT_CHAIN_MODE_ECB,
				sizeof(BCRYPT_CHAIN_MODE_ECB),
				0)) << BCryptSetPropertyErrorString(LOG_GET_LEFT_EXPRESSION_RC);

			_key = std::make_unique<KeyObjectGuard>(_hALG.getHandle(), _keyObject.get(), _cbKeyObject, (unsigned char*)key.data(), (ULONG)key.size());
		}

		std::string BCryptGenerateSymmetricKeyErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_BUFFER_TOO_SMALL)
				return "The size of the key object specified by the cbKeyObject parameter is not large enough to hold the key object.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The handle in the hObject parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else
				return "Unknown NTSTATUS error";
		}

		std::string BCryptDestroyKeyErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The key handle in the hKey parameter is not valid.";
			else
				return "Unknown NTSTATUS error";
		}

		KeyObjectGuard::KeyObjectGuard(BCRYPT_ALG_HANDLE alg_handle, unsigned char* keyObject, ULONG keyObjectSize, unsigned char* key, ULONG keySize)
		{
			L_ENSURE_NTSTATUS(BCryptGenerateSymmetricKey(
				alg_handle,
				&_handle,
				keyObject,
				keyObjectSize,
				key,
				keySize,
				0
			)) << BCryptGenerateSymmetricKeyErrorString(LOG_GET_LEFT_EXPRESSION_RC);
		}

		KeyObjectGuard::~KeyObjectGuard()
		{
			L_LOG_IF_FAILED_NTSTATUS(BCryptDestroyKey(_handle)) << BCryptDestroyKeyErrorString(LOG_GET_LEFT_EXPRESSION_RC);
		}

		BCRYPT_KEY_HANDLE KeyObjectGuard::getHandle()
		{
			return _handle;
		}

		std::string BCryptExportKeyErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_BUFFER_TOO_SMALL)
				return "The size specified by the cbOutput parameter is not large enough to hold the ciphertext.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The key handle in the hKey parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else if (status == STATUS_NOT_SUPPORTED)
				return "The specified BLOB type is not supported by the provider.";
			else
				return "Unknown NTSTATUS error";
		}

		std::string BCryptImportKeyErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_BUFFER_TOO_SMALL)
				return "The size of the key object specified by the cbKeyObject parameter is not large enough to hold the key object.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The algorithm handle in the hAlgorithm parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else if (status == STATUS_NOT_SUPPORTED)
				return "The algorithm provider specified by the hAlgorithm parameter does not support the BLOB type specified by the pszBlobType parameter.";
			else
				return "Unknown NTSTATUS error";
		}

		class KeyObjectGuard_Import
		{
		public:
			KeyObjectGuard_Import(BCRYPT_ALG_HANDLE alg_handle, unsigned char* keyObject, ULONG keyObjectSize, unsigned char* blob, ULONG blobSize)
			{
				L_ENSURE_NTSTATUS(BCryptImportKey(
					alg_handle,
					NULL,
					BCRYPT_OPAQUE_KEY_BLOB,
					&_handle,
					keyObject,
					keyObjectSize,
					blob,
					blobSize,
					0
				)) << BCryptImportKeyErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			}
			~KeyObjectGuard_Import()
			{
				L_LOG_IF_FAILED_NTSTATUS(BCryptDestroyKey(_handle)) << BCryptDestroyKeyErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			}
			BCRYPT_KEY_HANDLE getHandle()
			{
				return _handle;
			}
		private:
			BCRYPT_KEY_HANDLE _handle;
		};

		std::string BCryptEncryptErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_BUFFER_TOO_SMALL)
				return "The size specified by the cbOutput parameter is not large enough to hold the ciphertext.";
			else if (status == STATUS_INVALID_BUFFER_SIZE)
				return "The cbInput parameter is not a multiple of the algorithm's block size and the BCRYPT_BLOCK_PADDING or the BCRYPT_PAD_NONE flag was not specified in the dwFlags parameter.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The key handle in the hKey parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else if (status == STATUS_NOT_SUPPORTED)
				return "The algorithm does not support encryption.";
			else
				return "Unknown NTSTATUS error";
		}

		std::vector<unsigned char> AES128ECB::encrypt(const std::vector<unsigned char>& data)
		{
			ULONG cbCipherText = 0;

			L_ENSURE_NTSTATUS(BCryptEncrypt(
				_key->getHandle(),
				(PBYTE)data.data(),
				(ULONG)data.size(),
				NULL,
				NULL,
				NULL,
				NULL,
				0,
				&cbCipherText,
				0)) << BCryptEncryptErrorString(LOG_GET_LEFT_EXPRESSION_RC);

			std::vector<unsigned char> encryptedData;
			encryptedData.resize(cbCipherText);

			ULONG cbData = 0;
			L_ENSURE_NTSTATUS(BCryptEncrypt(
				_key->getHandle(),
				(PBYTE)data.data(),
				(ULONG)data.size(),
				NULL,
				NULL,
				NULL,
				(PBYTE)encryptedData.data(),
				cbCipherText,
				&cbData,
				0)) << BCryptEncryptErrorString(LOG_GET_LEFT_EXPRESSION_RC);

			return encryptedData;
		}

		void AES128ECB::encrypt(const unsigned char* data, unsigned char* buf, ULONG size)
		{
			ULONG cbData = 0;
			L_ENSURE_NTSTATUS(BCryptEncrypt(
				_key->getHandle(),
				(PBYTE)data,
				size,
				NULL,
				NULL,
				NULL,
				(PBYTE)buf,
				size,
				&cbData,
				0)) << BCryptEncryptErrorString(LOG_GET_LEFT_EXPRESSION_RC);
		}

		std::string BCryptDecryptErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_AUTH_TAG_MISMATCH)
				return "The computed authentication tag did not match the value supplied in the pPaddingInfo parameter.";
			else if (status == STATUS_BUFFER_TOO_SMALL)
				return "The size specified by the cbOutput parameter is not large enough to hold the ciphertext.";
			else if (status == STATUS_INVALID_BUFFER_SIZE)
				return "The cbInput parameter is not a multiple of the algorithm's block size, and the BCRYPT_BLOCK_PADDING flag was not specified in the dwFlags parameter.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The key handle in the hKey parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else if (status == STATUS_NOT_SUPPORTED)
				return "The algorithm does not support decryption.";
			else
				return "Unknown NTSTATUS error";
		}

		std::vector<unsigned char> AES128ECB::decrypt(const std::vector<unsigned char>& data)
		{
			L_CHECK_LT(data.size(), std::numeric_limits<ULONG>::max());
			ULONG cbPlainText = 0;

			L_ENSURE_NTSTATUS(BCryptDecrypt(
				_key->getHandle(),
				(PBYTE)data.data(),
				(ULONG)data.size(),
				NULL,
				NULL,
				NULL,
				NULL,
				0,
				&cbPlainText,
				0)) << BCryptDecryptErrorString(LOG_GET_LEFT_EXPRESSION_RC);

			std::vector<unsigned char> decryptedData;
			decryptedData.resize(cbPlainText);

			ULONG cbData;
			L_ENSURE_NTSTATUS(BCryptDecrypt(
				_key->getHandle(),
				(PBYTE)data.data(),
				(ULONG)data.size(),
				NULL,
				NULL,
				NULL,
				(PBYTE)decryptedData.data(),
				cbPlainText,
				&cbData,
				0)) << BCryptDecryptErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			return decryptedData;
		}

		void AES128ECB::decrypt(const unsigned char* data, unsigned char* buf, ULONG size)
		{
			ULONG cbData;
			L_ENSURE_NTSTATUS(BCryptDecrypt(
				_key->getHandle(),
				(PBYTE)data,
				size,
				NULL,
				NULL,
				NULL,
				(PBYTE)buf,
				size,
				&cbData,
				0)) << BCryptDecryptErrorString(LOG_GET_LEFT_EXPRESSION_RC);
		}

		DWORD AES128ECB::getBlockLen() const
		{
			return _cbBlockLen;
		}

		RNG::RNG()
			:_hALG(BCRYPT_ALG_HANDLE_GUARD::AlgorithmType::RNG)
		{
		}

		std::string BCryptGenRandomErrorString(NTSTATUS status)
		{
			if (status == STATUS_SUCCESS)
				return "The function was successful.";
			else if (status == STATUS_INVALID_HANDLE)
				return "The handle in the hAlgorithm parameter is not valid.";
			else if (status == STATUS_INVALID_PARAMETER)
				return "One or more parameters are not valid.";
			else
				return "Unknown NTSTATUS error";
		}

		std::vector<unsigned char> RNG::getBytes(ULONG size)
		{
			std::vector<unsigned char> randomNumbers;
			randomNumbers.resize(size);
			L_ENSURE_NTSTATUS(BCryptGenRandom(
				_hALG.getHandle(),
				(PBYTE)randomNumbers.data(),
				size,
				NULL
			)) << BCryptGenRandomErrorString(LOG_GET_LEFT_EXPRESSION_RC);
			return randomNumbers;
		}

		std::string Base64Encode(const std::vector<unsigned char>& data)
		{
			L_CHECK_LT(data.size(), std::numeric_limits<DWORD>::max());
			DWORD size;
			L_ENSURE_WIN32API(CryptBinaryToStringA((PBYTE)data.data(), (DWORD)data.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &size));
			std::string base64;
			base64.resize(size - 1);
			L_ENSURE_WIN32API(CryptBinaryToStringA((PBYTE)data.data(), (DWORD)data.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, (char*)base64.c_str(), &size));
			return base64;
		}

		std::vector<unsigned char> Base64Decode(const std::string& data)
		{
			L_CHECK_LT(data.size(), std::numeric_limits<DWORD>::max());
			DWORD size;
			L_CHECK_WIN32API(CryptStringToBinaryA(data.c_str(), (DWORD)data.size(), CRYPT_STRING_BASE64, NULL, &size, NULL, NULL));
			std::vector<unsigned char> plaindata;
			plaindata.resize(size);
			L_CHECK_WIN32API(CryptStringToBinaryA(data.c_str(), (DWORD)data.size(), CRYPT_STRING_BASE64, (PBYTE)plaindata.data(), &size, NULL, NULL));
			return plaindata;
		}

		std::string BinaryToHexadecimalString(const std::vector<unsigned char>& data)
		{
			L_CHECK_LT(data.size(), std::numeric_limits<DWORD>::max());
			DWORD size;
			L_ENSURE_WIN32API(CryptBinaryToStringA((PBYTE)data.data(), (DWORD)data.size(), CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, NULL, &size));
			std::string hex;
			hex.resize(size - 1);
			L_ENSURE_WIN32API(CryptBinaryToStringA((PBYTE)data.data(), (DWORD)data.size(), CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, (char*)hex.c_str(), &size));
			return hex;
		}

		std::vector<unsigned char> HexadecimalStringToBinary(const std::string& data)
		{
			L_CHECK_LT(data.size(), std::numeric_limits<DWORD>::max());
			DWORD size;
			L_CHECK_WIN32API(CryptStringToBinaryA(data.c_str(), (DWORD)data.size(), CRYPT_STRING_HEXRAW, NULL, &size, NULL, NULL));
			std::vector<unsigned char> plaindata;
			plaindata.resize(size);
			L_CHECK_WIN32API(CryptStringToBinaryA(data.c_str(), (DWORD)data.size(), CRYPT_STRING_HEXRAW, (PBYTE)plaindata.data(), &size, NULL, NULL));
			return plaindata;
		}
	}
}