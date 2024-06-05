#pragma once

#include <Windows.h>
#include <string>
#include <cstdarg>
#include <fileapi.h>
#include <Psapi.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <xinput.h>
#include <sstream>
#include <filesystem>
#include <map>
#include <fstream>
#include <chrono>
#include <iomanip>

namespace ModUtils
{
    static HWND muWindow = NULL;
    static std::string muGameName = "ELDEN RING";
    static std::string muExpectedWindowName = "ELDEN RING™";
    static std::ofstream muLogFile;
    static const std::string muAobMask = "?";

    class Timer
    {
    private:
        unsigned int intervalMs;
        bool firstCheck;
        std::chrono::system_clock::time_point lastPassedCheckTime;

    public:
        Timer(unsigned int intervalMs);
        bool Check();
    };

    static std::string _GetModuleName(bool mainProcessModule);

    static std::string GetCurrentProcessName();

    static std::string GetCurrentModName();

    static std::string GetModFolderPath();

    std::string FindDLL(const std::filesystem::path &base_folder, const std::string &dll_name);

    bool replace(std::string &str, const std::string &from, const std::string &to);

    static void ShowErrorPopup(std::string error);

    static DWORD_PTR GetProcessBaseAddress(DWORD processId);

    static void ToggleMemoryProtection(bool protectionEnabled, uintptr_t address, size_t size);

    static void MemCopy(uintptr_t destination, uintptr_t source, size_t numBytes);

    static void MemSet(uintptr_t address, unsigned char byte, size_t numBytes);

    static uintptr_t RelativeToAbsoluteAddress(uintptr_t relativeAddressLocation);

    static std::vector<std::string> TokenifyAobString(std::string aob);

    static bool IsAobValid(std::vector<std::string> aobTokens);

    static bool VerifyAob(std::string aob);

    static bool VerifyAobs(std::vector<std::string> aobs);

    template <typename T>
    static std::string NumberToHexString(T number);

    static std::string NumberToHexString(unsigned char number);

    static uintptr_t AobScan(std::string aob);

    static std::vector<unsigned char> StringAobToRawAob(std::string aob);

    static std::string RawAobToStringAob(std::vector<unsigned char> rawAob);

    static bool CheckIfAobsMatch(std::string aob1, std::string aob2);

    static bool ReplaceExpectedBytesAtAddress(uintptr_t address, std::string expectedBytes, std::string newBytes);

    static void GetWindowHandleByName(std::string windowName);

    static BOOL CALLBACK EnumWindowHandles(HWND hwnd, LPARAM lParam);

    static void GetWindowHandleByEnumeration();

    static bool GetWindowHandle();

    static void AttemptToGetWindowHandle();

    static bool AreKeysPressed(std::vector<unsigned short> keys, bool trueWhileHolding = false, bool checkController = false);

    static bool AreKeysPressed(unsigned short key, bool trueWhileHolding = false, bool checkController = false);

    static void Hook(uintptr_t address, uintptr_t destination, size_t extraClearance = 0);

}
