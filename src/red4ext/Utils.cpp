#include "stdafx.hpp"
#include "Utils.hpp"
#ifdef __APPLE__
#include "MacShims.hpp"
#endif

//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/stdout_color_sinks.h>
//
//void Utils::CreateLogger()
//{
//    auto rootDir = GetRootDir();
//    auto red4extDir = rootDir / L"red4ext";
//    auto logsDir = red4extDir / L"logs";
//    auto logFile = logsDir / "mod_settings.log";
//
//    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
//    auto file = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile.string(), true);
//
//    spdlog::sinks_init_list sinks = {console, file};
//
//    auto logger = std::make_shared<spdlog::logger>("", sinks);
//    spdlog::set_default_logger(logger);
//
//    logger->flush_on(spdlog::level::trace);
//    spdlog::set_level(spdlog::level::trace);
//}

std::filesystem::path Utils::GetRootDir()
{
#ifdef __APPLE__
    // The game root (the folder with red4ext/, r6/, archive/ and Cyberpunk2077.app): three folders above this dylib,
    // <root>/red4ext/plugins/mod_settings/mod_settings.dylib. Windows takes the same folder from the exe path (bin/x64/).
    return ModSettingsMac::GameRoot();
#else
    constexpr auto pathLength = MAX_PATH + 1;

    // Try to get the executable path until we can fit the length of the path.
    std::wstring filename;
    do
    {
        filename.resize(filename.size() + pathLength, '\0');

        auto length = GetModuleFileName(nullptr, filename.data(), static_cast<uint32_t>(filename.size()));
        if (length > 0)
        {
            // Resize it to the real, std::filesystem::path" will use the string's length instead of recounting it.
            filename.resize(length);
        }
    } while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

    auto rootDir = std::filesystem::path(filename)
                       .parent_path()  // Resolve to "x64" directory.
                       .parent_path()  // Resolve to "bin" directory.
                       .parent_path(); // Resolve to game root directory.

    return rootDir;
#endif
}

#ifdef __APPLE__
// The folder of this plugin, where user.ini lives (on Windows: <root>/red4ext/plugins/mod_settings/).
std::filesystem::path Utils::GetPluginDir()
{
    return ModSettingsMac::PluginDir();
}
#endif

std::wstring Utils::ToWString(const char* aText)
{
#ifdef __APPLE__
    // UTF-8 in, UTF-32 wchar_t out (mbstowcs depends on the C locale, which is "C" and not UTF-8 inside the game).
    return ModSettingsMac::Utf8ToWide(aText);
#else
    auto length = strlen(aText);

    std::wstring result(L"", length);
    mbstowcs(result.data(), aText, length);

    return result;
#endif
}
