#include "aes_helper.h"
#include "base64.h"
//#include "VMProtectSDK.h"


//ecb aes解密
int aes_decrypt(char* in, unsigned int size, char* key, char* out)
{
	if (!in || !key || !out) return 0;
	AES_KEY aes;
	if (AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
	{
		return 0;
	}
	int len = size, en_len = 0;
	while (en_len < len)
	{
		AES_decrypt((unsigned char*)in, (unsigned char*)out, &aes);
		in += AES_BLOCK_SIZE;
		out += AES_BLOCK_SIZE;
		en_len += AES_BLOCK_SIZE;
	}
	return 1;
}

//ecb aes加密
int aes_encrypt(char* in, unsigned int size, char* key, char* out)
{
	if (!in || !key || !out) return 0;
	AES_KEY aes;
	if (AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
	{
		return 0;
	}
	int len = size, en_len = 0;
	while (en_len < len)//输入输出字符串够长，并且是AES_BLOCK_SIZE的整数倍，需要严格限制
	{
		AES_encrypt((unsigned char*)in, (unsigned char*)out, &aes);
		in += AES_BLOCK_SIZE;
		out += AES_BLOCK_SIZE;
		en_len += AES_BLOCK_SIZE;
	}
	return 1;
}

void ControlEnLength(std::string &strtext)
{
	unsigned int length = strtext.length();
	if (length % AES_BLOCK_SIZE != 0)
	{
		unsigned int addcount = AES_BLOCK_SIZE -length%AES_BLOCK_SIZE;
		unsigned int newsize = addcount + length;
		strtext.resize(newsize);
	}
}

void ControlDeLength(std::string &strtext)
{
	char *p = (char *)strtext.c_str();
	unsigned int size = 0;
	while (*p != '\0')
	{
		size++;
		p++;
	}
	strtext.resize(size + 1);
}

std::string Encoder(std::string srctext, std::string key)
{
	ControlEnLength(srctext);
	std::string entext;
	entext.resize(srctext.length());
	aes_encrypt((char *)srctext.c_str(), srctext.length(), (char *)key.c_str(), (char *)entext.c_str());//aes加密
	std::string base64text = base64_encode((unsigned char *)entext.c_str(), srctext.size());//base64编码
	return base64text;
}

std::string Decoder(std::string strctext, std::string key)
{
	std::string detext;
	std::string entext = base64_decode(strctext);
	detext.resize(entext.length());
	aes_decrypt((char *)entext.c_str(), entext.length(), (char *)key.c_str(), (char *)detext.c_str());
	ControlDeLength(detext);
	return detext;
}


