#include "modUtils.hpp"

ModUtils::Timer::Timer(unsigned int intervalMs)
{
    this->intervalMs = intervalMs;
}

bool ModUtils::Timer::Check()
{
    if (firstCheck)
    {
        Reset();
        firstCheck = false;
    }

    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPassedCheckTime);
    if (diff.count() >= intervalMs)
    {
        lastPassedCheckTime = now;
        return true;
    }

    return false;
}

void ModUtils::Timer::Reset()
{
    lastPassedCheckTime = std::chrono::system_clock::now();
}

unsigned int intervalMs = 0;
bool firstCheck = true;
std::chrono::system_clock::time_point lastPassedCheckTime;

std::string ModUtils::_GetModuleName(bool mainProcessModule)
{
    HMODULE module = NULL;

    if (!mainProcessModule)
    {
        static char dummyStaticVarToGetModuleHandle = 'x';
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, &dummyStaticVarToGetModuleHandle, &module);
    }

    char lpFilename[MAX_PATH];
    GetModuleFileNameA(module, lpFilename, sizeof(lpFilename));
    std::string moduleName = strrchr(lpFilename, '\\');
    moduleName = moduleName.substr(1, moduleName.length());

    if (!mainProcessModule)
    {
        moduleName.erase(moduleName.find(".dll"), moduleName.length());
    }

    return moduleName;
}

std::string ModUtils::GetCurrentProcessName()
{
    return _GetModuleName(true);
}

std::string ModUtils::GetCurrentModName()
{
    static std::string currentModName = "NULL";
    if (currentModName == "NULL")
    {
        currentModName = _GetModuleName(false);
    }
    return currentModName;
}

std::string ModUtils::GetModFolderPath()
{
    return std::string("mods\\" + GetCurrentModName());
}

std::string ModUtils::FindDLL(const std::filesystem::path &base_folder, const std::string &dll_name)
{
    for (const auto &entry : std::filesystem::directory_iterator(base_folder))
    {
        if (entry.is_regular_file() && entry.path().filename() == dll_name)
        {
            return entry.path().string();
        }
        else if (entry.is_directory())
        {
            for (const auto &sub_entry : std::filesystem::directory_iterator(entry))
            {
                if (sub_entry.is_regular_file() && sub_entry.path().filename() == dll_name)
                {
                    return sub_entry.path().string();
                }
            }
        }
    }
    return "";
}

bool ModUtils::replace(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void ModUtils::ShowErrorPopup(std::string error)
{
    GetCurrentModName();
    MessageBox(NULL, error.c_str(), GetCurrentModName().c_str(), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
}

DWORD_PTR ModUtils::GetProcessBaseAddress(DWORD processId)
{
    DWORD_PTR baseAddress = 0;
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    HMODULE *moduleArray = nullptr;
    LPBYTE moduleArrayBytes = 0;
    DWORD bytesRequired = 0;

    if (processHandle)
    {
        if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
        {
            if (bytesRequired)
            {
                moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);
                if (moduleArrayBytes)
                {
                    unsigned int moduleCount;
                    moduleCount = bytesRequired / sizeof(HMODULE);
                    moduleArray = (HMODULE *)moduleArrayBytes;
                    if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
                    {
                        baseAddress = (DWORD_PTR)moduleArray[0];
                    }
                    LocalFree(moduleArrayBytes);
                }
            }
        }
        CloseHandle(processHandle);
    }
    return baseAddress;
}

void ModUtils::ToggleMemoryProtection(bool protectionEnabled, uintptr_t address, size_t size)
{
    static std::map<uintptr_t, DWORD> protectionHistory;
    if (protectionEnabled && protectionHistory.find(address) != protectionHistory.end())
    {
        VirtualProtect((void *)address, size, protectionHistory[address], &protectionHistory[address]);
        protectionHistory.erase(address);
    }
    else if (!protectionEnabled && protectionHistory.find(address) == protectionHistory.end())
    {
        DWORD oldProtection = 0;
        VirtualProtect((void *)address, size, PAGE_EXECUTE_READWRITE, &oldProtection);
        protectionHistory[address] = oldProtection;
    }
}

void ModUtils::MemCopy(uintptr_t destination, uintptr_t source, size_t numBytes)
{
    ToggleMemoryProtection(false, destination, numBytes);
    ToggleMemoryProtection(false, source, numBytes);
    memcpy((void *)destination, (void *)source, numBytes);
    ToggleMemoryProtection(true, source, numBytes);
    ToggleMemoryProtection(true, destination, numBytes);
}

void ModUtils::MemSet(uintptr_t address, unsigned char byte, size_t numBytes)
{
    ToggleMemoryProtection(false, address, numBytes);
    memset((void *)address, byte, numBytes);
    ToggleMemoryProtection(true, address, numBytes);
}

uintptr_t ModUtils::RelativeToAbsoluteAddress(uintptr_t relativeAddressLocation)
{
    uintptr_t absoluteAddress = 0;
    intptr_t relativeAddress = 0;
    MemCopy((uintptr_t)&relativeAddress, relativeAddressLocation, 4);
    absoluteAddress = relativeAddressLocation + 4 + relativeAddress;
    return absoluteAddress;
}

std::vector<std::string> ModUtils::TokenifyAobString(std::string aob)
{
    std::istringstream iss(aob);
    std::vector<std::string> aobTokens{
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}};
    return aobTokens;
}

bool ModUtils::IsAobValid(std::vector<std::string> aobTokens)
{
    for (auto byte : aobTokens)
    {
        if (byte == muAobMask)
        {
            continue;
        }

        if (byte.length() != 2)
        {
            return false;
        }

        std::string whitelist = "0123456789abcdef";
        if (byte.find_first_not_of(whitelist) != std::string::npos)
        {
            return false;
        }
    }
    return true;
}

bool ModUtils::VerifyAob(std::string aob)
{
    std::vector<std::string> aobTokens = TokenifyAobString(aob);
    if (!IsAobValid(aobTokens))
    {
        ShowErrorPopup("AOB is invalid! (" + aob + ")");
        return false;
    };
    return true;
}

bool ModUtils::VerifyAobs(std::vector<std::string> aobs)
{
    for (auto aob : aobs)
    {
        if (!VerifyAob(aob))
        {
            return false;
        }
    }
    return true;
}

template <typename T>
std::string ModUtils::NumberToHexString(T number)
{
    std::stringstream stream;
    stream
        << std::setfill('0')
        << std::setw(sizeof(T) * 2)
        << std::hex
        << number;
    return stream.str();
}

std::string ModUtils::NumberToHexString(unsigned char number)
{
    std::stringstream stream;
    stream
        << std::setw(2)
        << std::setfill('0')
        << std::hex
        << (unsigned int)number; // The << operator overload for unsigned chars screws us over unless this cast is done
    return stream.str();
}

uintptr_t ModUtils::AobScan(std::string aob)
{
    std::vector<std::string> aobTokens = TokenifyAobString(aob);

    DWORD processId = GetCurrentProcessId();
    uintptr_t regionStart = GetProcessBaseAddress(processId);
    std::cout << "Process name: " << GetCurrentProcessName() << std::endl;
    std::cout << "Process ID: " << processId << std::endl;
    std::cout << "Process base address: " << NumberToHexString(regionStart) << std::endl;
    std::cout << "AOB: " << aob << std::endl;

    if (!VerifyAob(aob))
    {
        return 0;
    };

    size_t numRegionsChecked = 0;
    size_t maxRegionsToCheck = 10000;
    uintptr_t currentAddress = 0;
    while (numRegionsChecked < maxRegionsToCheck)
    {
        MEMORY_BASIC_INFORMATION memoryInfo = {0};
        if (VirtualQuery((void *)regionStart, &memoryInfo, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
        {
            DWORD error = GetLastError();
            if (error == ERROR_INVALID_PARAMETER)
            {
                std::cout << "Reached end of scannable memory." << std::endl;
            }
            else
            {
                std::cout << "VirtualQuery failed, error code: " << error << std::endl;
            }
            break;
        }
        regionStart = (uintptr_t)memoryInfo.BaseAddress;
        uintptr_t regionSize = (uintptr_t)memoryInfo.RegionSize;
        uintptr_t regionEnd = regionStart + regionSize;
        uintptr_t protection = (uintptr_t)memoryInfo.Protect;
        uintptr_t state = (uintptr_t)memoryInfo.State;

        bool isMemoryReadable = (protection == PAGE_EXECUTE_READWRITE || protection == PAGE_READWRITE || protection == PAGE_READONLY || protection == PAGE_WRITECOPY || protection == PAGE_EXECUTE_WRITECOPY) && state == MEM_COMMIT;
        if (isMemoryReadable)
        {
            std::cout << "Checking region: " << NumberToHexString(regionStart) << std::endl;
            currentAddress = regionStart;
            while (currentAddress < regionEnd - aobTokens.size())
            {
                for (size_t i = 0; i < aobTokens.size(); i++)
                {
                    if (aobTokens[i] == muAobMask)
                    {
                        currentAddress++;
                        continue;
                    }
                    else if (*(unsigned char *)currentAddress != (unsigned char)std::stoul(aobTokens[i], nullptr, 16))
                    {
                        currentAddress++;
                        break;
                    }
                    else if (i == aobTokens.size() - 1)
                    {
                        uintptr_t signature = currentAddress - aobTokens.size() + 1;
                        std::cout << "Found signature at " << NumberToHexString(signature) << std::endl;
                        return signature;
                    }
                    currentAddress++;
                }
            }
        }
        else
        {
            std::cout << "Skipped region: " << NumberToHexString(regionStart) << std::endl;
        }

        numRegionsChecked++;
        regionStart += memoryInfo.RegionSize;
    }

    std::cout << "Stopped at: " << NumberToHexString(currentAddress) << ", num regions checked: " << numRegionsChecked << std::endl;
    ShowErrorPopup("Could not find signature!");
    return 0;
}

std::vector<unsigned char> ModUtils::StringAobToRawAob(std::string aob)
{
    std::vector<unsigned char> rawAob;
    std::vector<std::string> tokenifiedAob = TokenifyAobString(aob);
    for (size_t i = 0; i < tokenifiedAob.size(); i++)
    {
        if (tokenifiedAob[i] == muAobMask)
        {
            ShowErrorPopup("Cannot convert AOB with mask to raw AOB");
            return std::vector<unsigned char>();
        }

        unsigned char byte = (unsigned char)std::stoul(tokenifiedAob[i], nullptr, 16);
        rawAob.push_back(byte);
    }
    return rawAob;
}

std::string ModUtils::RawAobToStringAob(std::vector<unsigned char> rawAob)
{
    std::string aob;
    for (auto byte : rawAob)
    {
        std::string string = NumberToHexString(byte);
        aob += string + " ";
    }
    aob.pop_back();
    return aob;
}

bool ModUtils::CheckIfAobsMatch(std::string aob1, std::string aob2)
{
    std::vector<std::string> aob1Tokens = TokenifyAobString(aob1);
    std::vector<std::string> aob2Tokens = TokenifyAobString(aob2);

    size_t shortestAobLength = aob1Tokens.size() < aob2Tokens.size() ? aob1Tokens.size() : aob2Tokens.size();
    for (size_t i = 0; i < shortestAobLength; i++)
    {
        bool tokenIsMasked = aob1Tokens[i] == muAobMask || aob2Tokens[i] == muAobMask;
        if (tokenIsMasked)
        {
            continue;
        }

        if (aob1Tokens[i] != aob2Tokens[i])
        {
            ShowErrorPopup("Bytes do not match!");
            return false;
        }
    }
    return true;
}

bool ModUtils::ReplaceExpectedBytesAtAddress(uintptr_t address, std::string expectedBytes, std::string newBytes)
{
    if (!VerifyAobs({expectedBytes, newBytes}))
    {
        return false;
    }

    std::vector<std::string> expectedBytesTokens = TokenifyAobString(expectedBytes);
    std::vector<unsigned char> existingBytesBuffer(expectedBytesTokens.size(), 0);
    MemCopy((uintptr_t)&existingBytesBuffer[0], address, existingBytesBuffer.size());
    std::string existingBytes = RawAobToStringAob(existingBytesBuffer);

    std::cout << "Bytes at address: " << existingBytes << std::endl;
    std::cout << "Expected bytes: " << expectedBytes << std::endl;
    std::cout << "New bytes: " << newBytes << std::endl;

    if (CheckIfAobsMatch(existingBytes, expectedBytes))
    {
        std::cout << "Bytes match" << std::endl;
        std::vector<unsigned char> rawNewBytes = StringAobToRawAob(newBytes);
        MemCopy(address, (uintptr_t)&rawNewBytes[0], rawNewBytes.size());
        std::cout << "Patch applied" << std::endl;
        return true;
    }

    return false;
}

void ModUtils::GetWindowHandleByName(std::string windowName)
{
    if (muWindow == NULL)
    {
        for (size_t i = 0; i < 10000; i++)
        {
            HWND hwnd = FindWindowExA(NULL, NULL, NULL, windowName.c_str());
            DWORD processId = 0;
            GetWindowThreadProcessId(hwnd, &processId);
            if (processId == GetCurrentProcessId())
            {
                muWindow = hwnd;
                std::cout << "FindWindowExA: found window handle" << std::endl;
                break;
            }
            Sleep(1);
        }
    }
}

BOOL CALLBACK ModUtils::EnumWindowHandles(HWND hwnd, LPARAM lParam)
{
    DWORD processId = NULL;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == GetCurrentProcessId())
    {
        char buffer[100];
        GetWindowTextA(hwnd, buffer, 100);
        std::cout << "Found window belonging to ER: " << buffer << std::endl;
        if (std::string(buffer).find(muGameName) != std::string::npos)
        {
            std::cout << buffer << " handle selected" << std::endl;
            muWindow = hwnd;
            return false;
        }
    }
    return true;
}

void ModUtils::GetWindowHandleByEnumeration()
{
    if (muWindow == NULL)
    {
        std::cout << "Enumerating windows..." << std::endl;
        for (size_t i = 0; i < 10000; i++)
        {
            EnumWindows(&ModUtils::EnumWindowHandles, NULL);
            if (muWindow != NULL)
            {
                break;
            }
            Sleep(1);
        }
    }
}

bool ModUtils::GetWindowHandle()
{
    std::cout << "Finding application window..." << std::endl;

    GetWindowHandleByName(muExpectedWindowName);

    // From experience it can be tricky to find the game window consistently using only one technique,
    // (seems to differ from machine to machine for some reason) so we have this extra backup technique.
    GetWindowHandleByEnumeration();

    return (muWindow == NULL) ? false : true;
}

void ModUtils::AttemptToGetWindowHandle()
{
    static bool hasAttemptedToGetWindowHandle = false;

    if (!hasAttemptedToGetWindowHandle)
    {
        if (GetWindowHandle())
        {
            char buffer[100];
            GetWindowTextA(muWindow, buffer, 100);
            std::cout << "Found application window: " << buffer << std::endl;
        }
        else
        {
            std::cout << "Failed to get window handle, inputs will be detected globally!" << std::endl;
        }
        hasAttemptedToGetWindowHandle = true;
    }
}

// bool ModUtils::AreKeysPressed(std::vector<unsigned short> keys, bool trueWhileHolding = false, bool checkController = false)
// {
//     static std::vector<std::vector<unsigned short>> notReleasedKeys;

//     AttemptToGetWindowHandle();

//     bool ignoreOutOfFocusInput = muWindow != NULL && muWindow != GetForegroundWindow();
//     if (ignoreOutOfFocusInput)
//     {
//         return false;
//     }

//     size_t numKeys = keys.size();
//     size_t numKeysBeingPressed = 0;

//     if (checkController)
//     {
//         for (DWORD controllerIndex = 0; controllerIndex < XUSER_MAX_COUNT; controllerIndex++)
//         {
//             XINPUT_STATE state = {0};
//             DWORD result = XInputGetState(controllerIndex, &state);
//             if (result == ERROR_SUCCESS)
//             {
//                 for (auto key : keys)
//                 {
//                     if ((key & state.Gamepad.wButtons) == key)
//                     {
//                         numKeysBeingPressed++;
//                     }
//                 }
//             }
//         }
//     }
//     else
//     {
//         for (auto key : keys)
//         {
//             if (GetAsyncKeyState(key))
//             {
//                 numKeysBeingPressed++;
//             }
//         }
//     }

//     auto iterator = std::find(notReleasedKeys.begin(), notReleasedKeys.end(), keys);
//     bool keysBeingHeld = iterator != notReleasedKeys.end();
//     if (numKeysBeingPressed == numKeys)
//     {
//         if (keysBeingHeld)
//         {
//             if (!trueWhileHolding)
//             {
//                 return false;
//             }
//         }
//         else
//         {
//             notReleasedKeys.push_back(keys);
//         }
//     }
//     else
//     {
//         if (keysBeingHeld)
//         {
//             notReleasedKeys.erase(iterator);
//         }
//         return false;
//     }

//     return true;
// }

// bool ModUtils::AreKeysPressed(unsigned short key, bool trueWhileHolding = false, bool checkController = false)
// {
//     return AreKeysPressed({key}, trueWhileHolding, checkController);
// }

void ModUtils::Hook(uintptr_t address, uintptr_t destination, size_t extraClearance)
{
    size_t clearance = 14 + extraClearance;
    ModUtils::MemSet(address, 0x90, clearance);
    *(uintptr_t *)address = 0x0000000025ff;
    ModUtils::MemCopy((address + 6), (uintptr_t)&destination, 8);
    std::cout << "Created jump from " << ModUtils::NumberToHexString(address) << " to " << ModUtils::NumberToHexString(destination) << " with a clearance of " << clearance << std::endl;
}
