#include "stdafx.h"
#include "resource.h"
#include <string>
#include <iostream>
#include "Common/Common.h"
#include "Common/aes_helper.h"
#include "VMProtectSDK.h"
#include <windows.h>
#include <time.h>


#pragma comment(lib, "libcrypto.lib")

std::string head = "";
bool flag = true;

//回调函数,用于接收采集器抓到的数据
void _stdcall ReceiveMessage(char *args)
{ 
	std::string message = args;
	std::string key = "bullup2017@{lol}";//你的密码
	std::string decodeStr = Decoder(message, key);
	//std::string result = UTF8ToGBK(decodeStr);
	FILE* fp = fopen("C:/Users/Public/Bullup/log.txt", "wb");
	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		fputs(decodeStr.c_str(), fp);
		fclose(fp);
	}
	printf("%s\n", decodeStr.c_str());
	int code = decodeStr.find(head);
	if (code > 0){
		flag = false;
		exit(0);
	}
}
//UserInfo
//actions
//gameMode

typedef void(_stdcall *pMain)(DWORD procAddress);
int _tmain(int argc, _TCHAR* argv[]){
	head = argv[1];
	//head = "UserInfo";
	std::string filePath = "C:\\Users\\Public\\Bullup\\auto_program\\LolDataHelperNew.dll";
	HMODULE hModule = LoadLibrary(filePath.c_str());
	if (hModule == NULL){
		std::cout <<1235<< std::endl;
		return -1;
	}
	pMain Main = (pMain)GetProcAddress(hModule, "Main");
	Main((DWORD)ReceiveMessage);
	while (flag){
		continue;
	}
	//system("pause");
	return 0;
}

