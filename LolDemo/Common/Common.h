#pragma once
#include <windows.h>
#include <string>

DWORD GetProcessId(const char *pszProcessName);
bool IsProcessExist(const char *pszProcessName);
BOOL GetCmdLineByProcessId(std::string &cmdLine, DWORD pid);
std::string UnicodeToGbk(wchar_t *ptszText);
std::string UTF8ToGBK(const std::string& strUTF8);
std::string GBKToUTF8(const std::string& strGbk);
std::string & StdStrFormat(std::string & _str, const char * _Format, ...);
bool FsWriteData(std::string filepath, std::string &data);
void StrToLower(std::string &str);
std::string GetRandomStr();
std::string GenRand(int Count);
void PowerOff();
std::string GetUserComputerName();