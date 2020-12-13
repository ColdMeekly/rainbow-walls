#include "CUtility.h"

DWORD CUtility::findProcessByName(const char *processName, PROCESSENTRY32 *pEntry)
{
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;
	if (!Process32First(hSnapshot, &processEntry))
	{
		CloseHandle(hSnapshot);
		return 0;
	}
	do {
		if (!_strcmpi(processEntry.szExeFile, processName))
		{
			memcpy((void*)pEntry, (void *)&processEntry, sizeof(PROCESSENTRY32));
			CloseHandle(hSnapshot);
			return processEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &processEntry));
	CloseHandle(hSnapshot);
	return 0;
}

DWORD CUtility::getThreadByProcess(DWORD process)
{
	THREADENTRY32 ThreadEntry;
	ThreadEntry.dwSize = sizeof(THREADENTRY32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;
	if (!Thread32First(hSnapshot, &ThreadEntry))
	{
		CloseHandle(hSnapshot);
		return 0;
	}

	do {
		if (ThreadEntry.th32OwnerProcessID == process)
		{
			CloseHandle(hSnapshot);
			return ThreadEntry.th32ThreadID;
		}
	} while (Thread32Next(hSnapshot, &ThreadEntry));
	CloseHandle(hSnapshot);
	return 0;
}

bool CUtility::getModuleNamePointer(const char* moduleName, DWORD processID, MODULEENTRY32* OUT_moduleEntry)
{
	MODULEENTRY32 moduleEntry = { 0 };
	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return false;
	if (!Module32First(hSnapshot, &moduleEntry))
	{
		CloseHandle(hSnapshot);
		return false;
	}

	do {
		if (!_strcmpi(moduleEntry.szModule, moduleName))
		{
			CloseHandle(hSnapshot);
			*OUT_moduleEntry = moduleEntry;
			return true;
		}
	} while (Module32Next(hSnapshot, &moduleEntry));

	CloseHandle(hSnapshot);
	return false;
}


VOID CUtility::runSetDebugPrivs()
{
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hToken;
	TOKEN_PRIVILEGES priv;
	LUID debug;

	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken);
	LookupPrivilegeValue(NULL, "seDebugPrivilege", &debug);
	priv.PrivilegeCount = 1;
	priv.Privileges[0].Luid = debug;
	priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, false, &priv, NULL, NULL, NULL);
	CloseHandle(hToken);
	CloseHandle(hProcess);
}

VOID CUtility::runProcess()
{
	// Give our process debug privileges
	runSetDebugPrivs();
	// Wait for CSGO to start up
	while (!findProcessByName("csgo.exe", &this->__gameProcess)) Sleep(12);
	while (!getThreadByProcess(this->__gameProcess.th32ProcessID)) Sleep(12);
	// Get access to it
	this->__handleProcess = OpenProcess(PROCESS_ALL_ACCESS, false, this->__gameProcess.th32ProcessID);
	// Wait for all the necessary dll's to be loaded
	while (!getModuleNamePointer("client_panorama.dll", this->__gameProcess.th32ProcessID, &this->modClient));
	while (!getModuleNamePointer("engine.dll", this->__gameProcess.th32ProcessID, &this->modEngine));
	/// REDACTED: VGUI
	this->__HWNDcsgo = FindWindow(NULL, "Counter-Strike: Global Offensive");
}

DWORD* CUtility::FindPattern(MODULEENTRY32 moduleEntry, byte* pbMask, const char* pszMask, int _extra, size_t _offset) const
{
	byte *pBaseAddress = (byte*)moduleEntry.modBaseAddr;
	size_t nLength = (size_t)moduleEntry.modBaseSize;

	byte* moduleDump = (byte*)VirtualAlloc(0, nLength, MEM_COMMIT, PAGE_READWRITE);
	if (ReadProcessMemory(this->__handleProcess, (LPCVOID)pBaseAddress, moduleDump, nLength, NULL) == false)
	{
		char buf2[64];
		sprintf_s(buf2, "RPM Error code: %d", GetLastError());
		MessageBoxA(0, buf2, "Error", MB_OK);
		VirtualFree(moduleDump, nLength, MEM_RELEASE);
		return NULL;
	}

	auto DataCompare = [moduleDump](const byte* pData, const byte* mask, const char* cmask, byte chLast, size_t iEnd) -> bool {
		if (moduleDump[(size_t)pData + iEnd] != chLast) return false;
		for (; *cmask; ++cmask, ++pData, ++mask) {
			if (*cmask == 'x' && moduleDump[(size_t)pData] != *mask) {
				return false;
			}
		}

		return true;
	};

	auto iEnd = strlen(pszMask) - 1;
	auto chLast = pbMask[iEnd];

	auto pEnd = strlen(pszMask) - nLength;
	for (uintptr_t offset = 0; offset < pEnd; ++offset) {
		if (DataCompare((byte*)offset, pbMask, pszMask, chLast, iEnd)) {

			DWORD* foundAddress = *(DWORD**)&moduleDump[(size_t)offset + _offset];
			VirtualFree(moduleDump, nLength, MEM_RELEASE);
			return (DWORD*)((byte*)foundAddress + _extra);
		}
	}
	VirtualFree(moduleDump, nLength, MEM_RELEASE);
	return nullptr;
}



//byte* CUtility::FindPattern(const char * moduleName, byte* pbMask, const char* pszMask, int extra, size_t offset) const
//{
//	DWORD BaseAddress = 0, ModuleSize = 0;
//	CUtility::getModuleNamePointer(moduleName, this->__gameProcess.th32ParentProcessID, &BaseAddress, &ModuleSize);
//	byte *pBaseAddress = (byte*)BaseAddress;
//	size_t nLength = (size_t)ModuleSize;
//
//	byte* moduleDump = (byte*)VirtualAlloc(0, nLength, MEM_COMMIT, PAGE_READWRITE);
//	if (ReadProcessMemory(this->__handleProcess, (LPCVOID)pBaseAddress, moduleDump, nLength, NULL) == false)
//	{
//		char buf2[64];
//		sprintf(buf2, "RPM Error code: %d", GetLastError());
//		MessageBoxA(0, buf2, "Error", MB_OK);
//		return NULL;
//	}
//
//	auto DataCompare = [](const byte* pData, const byte* mask, const char* cmask, byte chLast, size_t iEnd) -> bool {
//		if (Util.Read<byte>((DWORD)pData + iEnd) != chLast) return false;
//		for (; *cmask; ++cmask, ++pData, ++mask) {
//			if (*cmask == 'x' && Util.Read<byte>((DWORD)pData) != *mask) {
//				return false;
//			}
//		}
//
//		return true;
//	};
//
//	auto iEnd = strlen(pszMask) - 1;
//	auto chLast = pbMask[iEnd];
//
//	auto* pEnd = pBaseAddress + nLength - strlen(pszMask);
//	for (; pBaseAddress < pEnd; ++pBaseAddress) {
//		if (DataCompare(pBaseAddress, pbMask, pszMask, chLast, iEnd)) {
//
//			return Util.Read<byte*>((DWORD)pBaseAddress + offset) + extra;
//		}
//	}
//
//	return nullptr;
//}
