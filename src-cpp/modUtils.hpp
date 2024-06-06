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

static HWND muWindow = NULL;
static std::string muGameName = "ELDEN RING";
static std::string muExpectedWindowName = "ELDEN RING™";
static std::ofstream muLogFile;
static const std::string muAobMask = "?";

namespace ModUtils
{

    class Timer
    {
    private:
        unsigned int intervalMs;
        bool firstCheck;
        std::chrono::system_clock::time_point lastPassedCheckTime;

    public:
        Timer(unsigned int intervalMs);
        void Reset();
        bool Check();
    };

    std::string _GetModuleName(bool mainProcessModule);

    std::string GetCurrentProcessName();

    std::string GetCurrentModName();

    std::string GetModFolderPath();

    std::string FindDLL(const std::filesystem::path &base_folder, const std::string &dll_name);

    bool replace(std::string &str, const std::string &from, const std::string &to);

    void ShowErrorPopup(std::string error);

    DWORD_PTR GetProcessBaseAddress(DWORD processId);

    uintptr_t RelativeToAbsoluteAddress(uintptr_t relativeAddressLocation);

    void ToggleMemoryProtection(bool protectionEnabled, uintptr_t address, size_t size);

    void MemCopy(uintptr_t destination, uintptr_t source, size_t numBytes);

    void MemSet(uintptr_t address, unsigned char byte, size_t numBytes);

    std::vector<std::string> TokenifyAobString(std::string aob);

    bool IsAobValid(std::vector<std::string> aobTokens);

    bool VerifyAob(std::string aob);

    bool VerifyAobs(std::vector<std::string> aobs);

    template <typename T>
    std::string NumberToHexString(T number);

    std::string NumberToHexString(unsigned char number);

    uintptr_t AobScan(std::string aob);

    std::vector<unsigned char> StringAobToRawAob(std::string aob);

    std::string RawAobToStringAob(std::vector<unsigned char> rawAob);

    bool CheckIfAobsMatch(std::string aob1, std::string aob2);

    bool ReplaceExpectedBytesAtAddress(uintptr_t address, std::string expectedBytes, std::string newBytes);

    void GetWindowHandleByName(std::string windowName);

    BOOL CALLBACK EnumWindowHandles(HWND hwnd, LPARAM lParam);

    void GetWindowHandleByEnumeration();

    bool GetWindowHandle();

    void AttemptToGetWindowHandle();

    //  bool AreKeysPressed(std::vector<unsigned short> keys, bool trueWhileHolding = false, bool checkController = false);

    //  bool AreKeysPressed(unsigned short key, bool trueWhileHolding = false, bool checkController = false);

    void Hook(uintptr_t address, uintptr_t destination, size_t extraClearance = 0);

}
