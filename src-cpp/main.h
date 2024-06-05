#ifndef MAIN_C_H
#define MAIN_C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Windows.h>
#include <stddef.h>

// Forward declarations for C++ functions
typedef void *TimerPtr;
typedef void *FileHandle;
typedef void *WindowHandle;

TimerPtr CreateTimer(unsigned int intervalMs);
int CheckTimer(TimerPtr timer);
// void ResetTimer(TimerPtr timer);
// void DestroyTimer(TimerPtr timer);

const char *_GetModuleName(bool mainProcessModule);

const char *GetCurrentProcessName();
const char *GetCurrentModName();
const char *GetModFolderPath();

const char *FindDLL(const char *base_folder, const char *dll_name);

int replace(char *str, const char *from, const char *to);

void ShowErrorPopup(const char *error);

DWORD_PTR GetProcessBaseAddress(DWORD processId);

void ToggleMemoryProtection(int protectionEnabled, uintptr_t address, size_t size);

void MemCopy(uintptr_t destination, uintptr_t source, size_t numBytes);

void MemSet(uintptr_t address, unsigned char byte, size_t numBytes);

uintptr_t RelativeToAbsoluteAddress(uintptr_t relativeAddressLocation);

const char **TokenifyAobString(const char *aob);

int IsAobValid(const char **aobTokens);

int VerifyAob(const char *aob);

int VerifyAobs(const char **aobs);

const char *NumberToHexString(uintptr_t number);

uintptr_t AobScan(const char *aob);

unsigned char *StringAobToRawAob(const char *aob);

const char *RawAobToStringAob(unsigned char *rawAob);

int CheckIfAobsMatch(const char *aob1, const char *aob2);

int ReplaceExpectedBytesAtAddress(uintptr_t address, const char *expectedBytes, const char *newBytes);

void GetWindowHandleByName(const char *windowName);

BOOL EnumWindowHandles(HWND hwnd, LPARAM lParam);

void GetWindowHandleByEnumeration();

int GetWindowHandle();

void AttemptToGetWindowHandle();

int AreKeysPressed(unsigned short *keys, int keysCount, int trueWhileHolding, int checkController);

void Hook(uintptr_t address, uintptr_t destination, size_t extraClearance);

#ifdef __cplusplus
}
#endif

#endif // MAIN_C_H