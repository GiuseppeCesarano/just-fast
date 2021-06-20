#include "JustFastUi/JustFastUi.h"
#include <cxxopts.hpp>
#include <filesystem>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <string>

cxxopts::ParseResult parseArgs(cxxopts::Options opts, int* argc, char*** argv)
{
    try {
        cxxopts::ParseResult res = opts.parse(*argc, *argv);
        return res;
    } catch (const cxxopts::OptionParseException& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

void startJustFast(const JustFastOptions& options)
{
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    auto ui = ftxui::Make<JustFastUi>(options);
    ui->setQuitFunction(screen.ExitLoopClosure());
    screen.Loop(ui);
}

std::filesystem::path normalizePath(std::filesystem::path p)
{
    p.lexically_normal();
    if (p.is_absolute()) {
        return p;
    } else {
        return (std::filesystem::current_path() / p);
    }
}

int main(int argc, char* argv[])
{

    cxxopts::Options cliOptions(PROJECT_NAME, PROJECT_DESCRIPTION);

    // clang-format off
    cliOptions.add_options()
	("h,help", "Prints this help messange")
	("v,version", "Prints version")
	("a,all", "List entries starting with .", cxxopts::value<bool>()->default_value("false"))
	("p,path", "Change starting path", cxxopts::value<std::string>()->default_value(""));
    // clang-format on

    auto cliResult = parseArgs(cliOptions, &argc, &argv);

    if (cliResult.count("help")) {
        std::cout << cliOptions.help() << std::endl;
        exit(0);
    }

    if (cliResult.count("version")) {
        std::cout << PROJECT_NAME << "'s version: " << PROJECT_VERSION << std::endl;
        exit(0);
    }

    //Creating options
    JustFastOptions options;
    //TODO: Change that check when file opening is supported.
    if (cliResult["path"].as<std::string>() == "") {
        options.path = std::filesystem::current_path();
    } else {
        try {
            options.path = normalizePath(cliResult["path"].as<std::string>());
        } catch (std::filesystem::filesystem_error& error) {
            std::cerr << "Can't open parsed path" << std::endl;
            exit(1);
        }
    }
    options.showHiddenFiles = cliResult["all"].as<bool>();

    startJustFast(options);

    return 0;
}
