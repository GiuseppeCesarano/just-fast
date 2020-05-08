#include "JustFastUi/JustFastUi.h"
#include <cxxopts.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>

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

void startJustFast()
{
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    JustFastUi ui;
    ui.setQuitFunction(screen.ExitLoopClosure());
    screen.Loop(&ui);
}

int main(int argc, char* argv[])
{

    cxxopts::Options cliOptions(PROJECT_NAME, PROJECT_DESCRIPTION);

    // clang-format off

    cliOptions.add_options()
	("h,help", "Prints this help messange");

    // clang-format on

    auto cliResult = parseArgs(cliOptions, &argc, &argv);

    if (cliResult.count("help")) {
        std::cout << cliOptions.help() << std::endl;
        exit(0);
    }

    startJustFast();

    return 0;
}
