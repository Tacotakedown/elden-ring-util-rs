#include "main.h"
#include "modUtils.hpp"

TimerPtr CreateTimer(unsigned int intervalMs) {
    return new ModUtils::Timer(intervalMs);
}

int CheckTimer(TimerPtr timer){
    ModUtils::Timer* cppTimer = reinterpret_cast<ModUtils::Timer*>(timer);
    return cppTimer->Check()? 1:0;
}

const char *_GetModuleName(bool mainProcessModule) {
    std::string moduleName = ModUtils::_GetModuleName(mainProcessModule);
    char* cModuleName = new char[moduleName.length()+1];
    strcpy(cModuleName, moduleName.c_str());
    return cModuleName;
}

const char *GetCurrentProcessName(){
    std::string currentProcessName = ModUtils::GetCurrentProcessName();
    char* cCurrentProcessName = new char[currentProcessName.length()+1];
    strcpy(cCurrentProcessName, currentProcessName.c_str());
    return cCurrentProcessName;
}

const char *GetCurrentModName(){
    std::string modName = ModUtils::GetCurrentModName();
    char* cModName = new char[modName.length() + 1];
    strcpy(cModName, modName.c_str());
    return cModName;
}

const char *GetModFolderPath(){
    std::string modFolder = ModUtils::GetModFolderPath();
    char* cModFolder = new char[modFolder.length() + 1];
    strcpy(cModFolder, modFolder.c_str());
    return cModFolder;
}

const char *FindDLL(const char *base_folder, const char *dll_name) {

    std::filesystem::path folderPath(base_folder);
    std::string dllName(dll_name);

    std::string dllPath = ModUtils::FindDLL(folderPath, dllName);
    char* cDllPath = new char[dllPath.length() + 1];
    strcpy(cDllPath, dllPath.c_str());
    return cDllPath;
}

int replace(char *str, const char *from, const char *to){
    std::string Str(str);
    std::string From(from);
    std::string To(to);
    bool replaced = ModUtils::replace(Str, From, To);

    int result = replaced ? 1 : 0;
    return result;
}

void ShowErrorPopup(const char *error) {
    std::string Error(error);

    ModUtils::ShowErrorPopup(Error);
}

