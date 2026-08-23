#include "rypch.h"

#include "Rynex/Utils/PlatformUtils.h"

#ifdef RY_PLATFORM_LINUX

#include <cstdlib>
#include <cstdio>
#include <array>

namespace Rynex {

    static std::string RunCommand(const std::string& cmd)
    {
        std::array<char, 512> buffer;
        std::string result;
        FILE* pipe = popen(cmd.c_str(), "r");
        if (nullptr == pipe)
            return result;
        while (nullptr != fgets(buffer.data(), (int)buffer.size(), pipe))
            result += buffer.data();
        pclose(pipe);
        // trim trailing newline
        if (!result.empty() && result.back() == '\n')
            result.pop_back();
        return result;
    }

    std::string FileDialoges::OpenFile(const char* filter, const char* beginDir)
    {
        std::string cmd = "zenity --file-selection 2>/dev/null";
        if (beginDir && beginDir[0] != '\0')
            cmd += std::string(" --filename=") + beginDir + "/";
        return RunCommand(cmd);
    }

    std::string FileDialoges::SaveFile(const char* filter)
    {
        return RunCommand("zenity --file-selection --save --confirm-overwrite 2>/dev/null");
    }

    std::string FileDialoges::SelectFolder()
    {
        return RunCommand("zenity --file-selection --directory 2>/dev/null");
    }

    void LaunchFile::OpenFileInDefaultApp(const std::filesystem::path& filpath,
        const std::filesystem::path& workingDirectory)
    {
        RY_CORE_ASSERT(filpath.has_filename(), "No File Name Found");
        std::string cmd = "xdg-open \"" + filpath.string() + "\" &";
        int ret = system(cmd.c_str());
        RY_CORE_ASSERT(0 == ret, "xdg-open failed");
        RY_CORE_INFO("Programm: '{}' opened via xdg-open", filpath.filename().string());
    }

}

#endif // RY_PLATFORM_LINUX