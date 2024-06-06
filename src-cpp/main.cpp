#include "main.h"
#include "modUtils.hpp"

TimerPtr CreateTimer(unsigned int intervalMs)
{
    return new ModUtils::Timer(intervalMs);
}

int CheckTimer(TimerPtr timer)
{
    ModUtils::Timer *cppTimer = reinterpret_cast<ModUtils::Timer *>(timer);
    return cppTimer->Check() ? 1 : 0;
}

const char *_GetModuleName(bool mainProcessModule)
{
    std::string moduleName = ModUtils::_GetModuleName(mainProcessModule);
    char *cModuleName = new char[moduleName.length() + 1];
    strcpy(cModuleName, moduleName.c_str());
    return cModuleName;
}

const char *GetCurrentProcessName()
{
    std::string currentProcessName = ModUtils::GetCurrentProcessName();
    char *cCurrentProcessName = new char[currentProcessName.length() + 1];
    strcpy(cCurrentProcessName, currentProcessName.c_str());
    return cCurrentProcessName;
}

const char *GetCurrentModName()
{
    std::string modName = ModUtils::GetCurrentModName();
    char *cModName = new char[modName.length() + 1];
    strcpy(cModName, modName.c_str());
    return cModName;
}

const char *GetModFolderPath()
{
    std::string modFolder = ModUtils::GetModFolderPath();
    char *cModFolder = new char[modFolder.length() + 1];
    strcpy(cModFolder, modFolder.c_str());
    return cModFolder;
}

const char *FindDLL(const char *base_folder, const char *dll_name)
{

    std::filesystem::path folderPath(base_folder);
    std::string dllName(dll_name);

    std::string dllPath = ModUtils::FindDLL(folderPath, dllName);
    char *cDllPath = new char[dllPath.length() + 1];
    strcpy(cDllPath, dllPath.c_str());
    return cDllPath;
}

int replace(char *str, const char *from, const char *to)
{
    std::string Str(str);
    std::string From(from);
    std::string To(to);
    bool replaced = ModUtils::replace(Str, From, To);

    int result = replaced ? 1 : 0;
    return result;
}

void ShowErrorPopup(const char *error)
{
    std::string Error(error);

    ModUtils::ShowErrorPopup(Error);
}

DWORD_PTR GetProcessBaseAddress(DWORD processId)
{
    return ModUtils::GetProcessBaseAddress(processId);
}

uintptr_t RelativeToAbsoluteAddress(uintptr_t relativeAddressLocation)
{
    return (uintptr_t)ModUtils::GetProcessBaseAddress((DWORD)relativeAddressLocation);
}

void ToggleMemoryProtection(int protectionEnabled, uintptr_t address, size_t size)
{
    ModUtils::ToggleMemoryProtection(protectionEnabled == 1 ? true : false, address, size);
}

void MemCopy(uintptr_t destination, uintptr_t source, size_t numBytes)
{
    ModUtils::MemCopy(destination, source, numBytes);
}

void MemSet(uintptr_t address, unsigned char byte, size_t numBytes)
{
    ModUtils::MemSet(address, byte, numBytes);
}

const char **TokenifyAobString(const char *aob)
{
    std::string aobStr(aob);

    std::vector<std::string> tokens = ModUtils::TokenifyAobString(aobStr);

    const char **tokenArray = (const char **)malloc((tokens.size() + 1) * sizeof(const char *));

    if (tokenArray == NULL)
    {
        std::cout << "Failed to alocate memory for AOB String vec" << std::endl;
        return NULL;
    }

    for (size_t i = 0; i < tokens.size(); i++)
    {
        tokenArray[i] = strdup(tokens[i].c_str());
        if (tokenArray[i] == NULL)
        {
            std::cout << "failed to allocate memory for item in array:" << i << std::endl;

            for (size_t j = 0; j < i; j++)
            {
                free((void *)tokenArray[j]);
            }
            free(tokenArray);
            return NULL;
        }
    }
    tokenArray[tokens.size()] = NULL;

    return tokenArray;
}

int IsAobValid(const char **aobTokens)
{
    std::vector<std::string> tokens;
    for (size_t i = 0; aobTokens[i] != NULL; ++i)
    {
        tokens.push_back(std::string(aobTokens[i]));
    }

    bool isValid = ModUtils::IsAobValid(tokens);

    return isValid ? 1 : 0;
}

int VerifyAob(const char *abo)
{
    std::string aboString(abo);
    bool isValid = ModUtils::VerifyAob(aboString);
    return isValid ? 1 : 0;
}

int VerifyAobs(const char **abo)
{
    std::vector<std::string> bytes;
    for (size_t i = 0; abo[i] != NULL; ++i)
    {
        bytes.push_back(std::string(abo[i]));
    }

    bool isValid = ModUtils::VerifyAobs(bytes);

    return isValid ? 1 : 0;
}

// const char *NumberToHexString(uintptr_t number)
// {
//     std::string hex = ModUtils::NumberToHexString(number);
//     char *cHex = new char[hex.length() + 1];
//     strcpy(cHex, hex.c_str());
//     return cHex;
// }

uintptr_t AobScan(const char *aob)
{
    std::string aobString(aob);
    uintptr_t scanRes = ModUtils::AobScan(aobString);
    return scanRes;
}

unsigned char *StringAobToRawAob(const char *aob)
{
    std::string stringAob(aob);
    std::vector<unsigned char> rawAob = ModUtils::StringAobToRawAob(stringAob);

    unsigned char *cRawAob = new unsigned char[rawAob.size()];
    std::copy(rawAob.begin(), rawAob.end(), cRawAob);

    return cRawAob;
}

const char *RawAobToStringAob(unsigned char *rawAob)
{
    std::vector<unsigned char> rawAobVec;

    for (size_t i = 0; rawAob[i] != 0; ++i)
    {
        rawAobVec.push_back(rawAob[i]);
    }

    std::string stringAob = ModUtils::RawAobToStringAob(rawAobVec);

    char *cStringAob = new char[stringAob.length() + 1];

    std::strcpy(cStringAob, stringAob.c_str());

    return cStringAob;
}

int CheckIfAobsMatch(const char *aob1, const char *aob2)
{
    std::string Aob1String(aob1);
    std::string Aob2String(aob2);
    bool isEqual = ModUtils::CheckIfAobsMatch(Aob1String, Aob2String);
    return isEqual ? 1 : 0;
}

int ReplaceExpectedBytesAtAddress(uintptr_t address, const char *expectedBytes, const char *newBytes)
{
    std::string expectedBytesString(expectedBytes);
    std::string newBytesString(newBytes);

    bool result = ModUtils::ReplaceExpectedBytesAtAddress(address, expectedBytesString, newBytesString);
    return result ? 1 : 0;
}

void GetWindowHandleByName(const char *windowName)
{
    std::string windowNameString(windowName);
    ModUtils::GetWindowHandleByName(windowNameString);
}

// BOOL EnumWindowHandles(HWND hwnd, LPARAM lParam)
// {
//     BOOL result = ModUtils::EnumWindowHandles(hwnd, lParam);
//     return result;
// }

void GetWindowHandleByEnumeration()
{
    ModUtils::GetWindowHandleByEnumeration();
}

int GetWindowHandle()
{
    bool result = ModUtils::GetWindowHandle();
    return result ? 1 : 0;
}

void AttemptToGetWindowHandle()
{
    ModUtils::AttemptToGetWindowHandle();
}

// int AreKeysPressed(unsigned short *keys, int keysCount, int trueWhileHolding, int checkController)
// {
//     // bool result = ModUtils::
// }

void Hook(uintptr_t address, uintptr_t destination, size_t extraClearance)
{
    ModUtils::Hook(address, destination, extraClearance);
}