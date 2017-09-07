#include "Common.h"
#include <TlHelp32.h>
#include <time.h>
#include <algorithm>

DWORD GetProcessId(const char *pszProcessName)
{
	DWORD id = 0;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pInfo;
	pInfo.dwSize = sizeof(pInfo);
	Process32First(hSnapShot, &pInfo);
	do
	{
		if (_strcmpi(pInfo.szExeFile, pszProcessName) == 0)
		{
			id = pInfo.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapShot, &pInfo) != FALSE);
	CloseHandle(hSnapShot);
	return id;
}

bool IsProcessExist(const char *pszProcessName)
{
	if (GetProcessId(pszProcessName) != 0)
	{
		return true;
	}
	return false;
}

//通过进程ID获取对应进程的命令行参数
#define ProcessBasicInformation 0

typedef struct
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct
{
	ULONG          AllocationSize;
	ULONG          ActualSize;
	ULONG          Flags;
	ULONG          Unknown1;
	UNICODE_STRING Unknown2;
	HANDLE         InputHandle;
	HANDLE         OutputHandle;
	HANDLE         ErrorHandle;
	UNICODE_STRING CurrentDirectory;
	HANDLE         CurrentDirectoryHandle;
	UNICODE_STRING SearchPaths;
	UNICODE_STRING ApplicationName;
	UNICODE_STRING CommandLine;
	PVOID          EnvironmentBlock;
	ULONG          Unknown[9];
	UNICODE_STRING Unknown3;
	UNICODE_STRING Unknown4;
	UNICODE_STRING Unknown5;
	UNICODE_STRING Unknown6;
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;

typedef struct
{
	ULONG               AllocationSize;
	ULONG               Unknown1;
	HINSTANCE           ProcessHinstance;
	PVOID               ListDlls;
	PPROCESS_PARAMETERS ProcessParameters;
	ULONG               Unknown2;
	HANDLE              Heap;
} PEB, *PPEB;

typedef struct
{
	DWORD ExitStatus;
	PPEB  PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}   PROCESS_BASIC_INFORMATION;

typedef LONG(WINAPI *PROCNTQSIP)(HANDLE, UINT, PVOID, ULONG, PULONG);
PROCNTQSIP NtQueryInformationProcess;

BOOL GetProcessCmdLine(DWORD dwId, LPWSTR wBuf, DWORD dwBufLen)
{
	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	PROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	BOOL                      bRet = FALSE;

	// Get process handle
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwId);
	if (!hProcess)
		return FALSE;

	// Retrieve information
	status = NtQueryInformationProcess(hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		NULL
		);

	if (status)
		goto cleanup;

	if (!ReadProcessMemory(hProcess,
		pbi.PebBaseAddress,
		&Peb,
		sizeof(PEB),
		&dwDummy
		)
		)
		goto cleanup;

	if (!ReadProcessMemory(hProcess,
		Peb.ProcessParameters,
		&ProcParam,
		sizeof(PROCESS_PARAMETERS),
		&dwDummy
		)
		)
		goto cleanup;

	lpAddress = ProcParam.CommandLine.Buffer;
	dwSize = ProcParam.CommandLine.Length;

	if (dwBufLen < dwSize)
		goto cleanup;

	if (!ReadProcessMemory(hProcess,
		lpAddress,
		wBuf,
		dwSize,
		&dwDummy
		)
		)
		goto cleanup;

	bRet = TRUE;

cleanup:

	CloseHandle(hProcess);

	return bRet;
}

BOOL GetCmdLineByProcessId(std::string &cmdLine, DWORD pid)
{
	BOOL bResult = FALSE;

	NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(GetModuleHandle("ntdll"), "NtQueryInformationProcess");

	if (!NtQueryInformationProcess)
		return bResult;

	WCHAR wstr[2048] = { 0 }; //这个长度针对LOL是够用的

	if (GetProcessCmdLine(pid, wstr, sizeof(wstr)))
	{
		//字符串转化
		cmdLine = UnicodeToGbk(wstr);
		bResult = TRUE;
	}
		
	return bResult;
}

//字符串转化函数
std::string UnicodeToGbk(wchar_t *ptszText)
{
	std::string result;
	int cbMultiByte = WideCharToMultiByte(CP_ACP, 0, ptszText, -1, NULL, 0, NULL, NULL);
	char *pszText = NULL;
	pszText = new char[cbMultiByte];
	WideCharToMultiByte(CP_ACP, 0, ptszText, -1, pszText, cbMultiByte, NULL, NULL);
	result = pszText;
	delete[] pszText;
	return result;
}

//UTF-8转GBK
std::string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	unsigned short * wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}

std::string GBKToUTF8(const std::string& strGbk)
{
	int n = MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, NULL, 0);
	WCHAR *str1 = new WCHAR[n + 1];
	MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char *utf8Str = new char[n + 1];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, utf8Str, n, NULL, NULL);
	std::string result(utf8Str);
	delete[]utf8Str;
	delete[]str1;
	return result;
}


//字符串处理函数
std::string & StdStrFormat(std::string & _str, const char * _Format, ...) 
{
	std::string tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscprintf(_Format, marker);

	if (num_of_chars > tmp.capacity()) {
		tmp.resize(num_of_chars + 1);
	}

	vsprintf_s((char *)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	_str = tmp.c_str();
	return _str;
}

bool FsWriteData(std::string filepath, std::string &data)
{
	bool result = false;
	HANDLE filehandle = CreateFile(filepath.c_str(),
		FILE_MAP_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (filehandle != INVALID_HANDLE_VALUE)
	{
		DWORD returnsize = 0;
		result = WriteFile(filehandle, data.c_str(), data.size(), &returnsize, NULL);
		CloseHandle(filehandle);
	}
	return result;
}

void StrToLower(std::string &str)
{
	transform(str.begin(), str.end(), str.begin(), tolower);
}

std::string GetRandomStr()
{
	const char* StrTable = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int Len = 0;

	time_t T = time(0);
	srand((unsigned int)T);
	Len = rand();
	Len = Len % 10 + 10;

	std::basic_string<CHAR> Cch = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::basic_string<CHAR> TmpRandomStr;
	unsigned int RandomValue = 0;
	unsigned int Count = Len;// % (cch.size() + 1);
	for (unsigned int i = 0; i < Count; i++)
	{
		//调用此函数时一定要添加_CRT_RAND_S这个宏定义，且要添加在项目属性页---->配置属性---->C/C++---->预处理器---->预处理器定义里面添加该宏定义
		RandomValue = rand();

		TmpRandomStr += Cch[RandomValue % (Cch.size())];
	}

	return TmpRandomStr;
}

std::string GenRand(int Count)
{
	char StrTemp[2] = { 0 };
	std::string Str;
	int i, x;
	const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	srand((unsigned)GetTickCount());
	for (i = 0; i < Count; ++i)
	{
		x = rand() % (sizeof(charset) - 1);
		wsprintfA(StrTemp, "%c", charset[x]);
		Str.append(StrTemp);
	}
	return Str;
}


void PowerOff()
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;
	LPTSTR MachineName = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		return;
	}
	if (!LookupPrivilegeValue(MachineName, SE_SHUTDOWN_NAME, &luid))
	{
		return;
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL); //到这里,是取得权限///
	ExitWindowsEx(EWX_POWEROFF, EWX_FORCE);
}

std::string GetUserComputerName()
{
	std::string strName = "";
	char buffer[MAX_PATH] = { 0 };
	DWORD ret = 0;
	if (::GetComputerNameA(buffer, &ret))
	{
		strName = buffer;
	}
	return strName;
}