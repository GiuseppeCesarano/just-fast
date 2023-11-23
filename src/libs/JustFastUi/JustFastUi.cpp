#include "JustFastUi.h"
#include <codecvt>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/string.hpp>
#include <string>
#include <utility>

void JustFastUi::setQuitFunction(std::function<void()> q)
{
    quit = std::move(q);
}

JustFastUi::JustFastUi(const JustFastOptions& options)
    : statusMessage(L""), statusSelected(L"0"), currentPath { options.path }
    , isShowingHiddenFile { options.showHiddenFiles }
    , isSortFiles { options.sortFiles }
{
    int availableSpace = std::filesystem::space(currentPath).available / 1e9;
    int capacity = std::filesystem::space(currentPath).capacity / 1e9;
    diskSpaceAvailable = float(availableSpace) / float(capacity);
    spaceInfo = L"Free Space:" + std::to_wstring(availableSpace) + L" GiB " + L"(Total:" + std::to_wstring(capacity) + L"GiB)";

    currentPathCached = currentPath.wstring();
    Add(currentFolder);

    updateAllUi();
}

void JustFastUi::updateMainView(size_t cursorPosition)
{
    std::vector<std::wstring> currentFolderFiles;
    std::vector<std::wstring> currentFolderFolders;

    currentFolderEntries.clear();
    currentFolderSelected = cursorPosition;
    try {
        for (const auto& p : std::filesystem::directory_iterator(currentPath)) {
            if (isShowingHiddenFile || p.path().filename().string()[0] != '.') {

                if(isSortFiles) {
                    if(p.is_directory()) {
                        currentFolderFolders.emplace_back(p.path().filename().wstring());
                    }
                    else {
                        currentFolderFiles.emplace_back(p.path().filename().wstring());
                    }
                }
                else {
                    currentFolderEntries.emplace_back(p.path().filename().wstring());
                }
            }
        }
    } catch (std::filesystem::filesystem_error& error) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        statusMessage = converter.from_bytes(error.what());
        changePathAndUpdateViews(currentPath.parent_path());
    }

    if(isSortFiles) {
        std::sort(currentFolderFolders.begin(), currentFolderFolders.end());

        for (const auto& folder : currentFolderFolders) {
            currentFolderEntries.emplace_back(folder);
        }

        std::sort(currentFolderFiles.begin(), currentFolderFiles.end());

        for (const auto& file : currentFolderFiles) {
            currentFolderEntries.emplace_back(file);
        }
    }
}

void JustFastUi::updateParentView()
{
    parentFolderEntries.clear();
    for (const auto& p : std::filesystem::directory_iterator(currentPath.parent_path())) {
        if (isShowingHiddenFile || p.path().filename().string()[0] != '.') {
            parentFolderEntries.emplace_back(p.path().filename().wstring());
        }
        if (p.path().filename() == currentPath.filename()) {
            parentFolderSelected = parentFolderEntries.size() - 1;
        }
    }
}

void JustFastUi::updateOperationView()
{
    switch (filesystemOperations.getOperation()) {
    case FileSystemOperations::Operation::NOT_SELECTED:
        operationView = L"NO_MODE";
        break;
    case FileSystemOperations::Operation::COPY:
        operationView = L"COPY";
        break;
    case FileSystemOperations::Operation::MOVE:
        operationView = L"MOVE";
        break;
    case FileSystemOperations::Operation::DELETE:
        operationView = L"DELETE";
        break;
    }
}

void JustFastUi::updateSelectedCounter()
{
    statusSelected = L"(" + std::to_wstring(filesystemOperations.countSelectedFiles()) + L") ";
}

void JustFastUi::updateAllUi(size_t cursorPosition)
{
    updateMainView(cursorPosition);
    updateParentView();
    updateOperationView();
    updateSelectedCounter();
}

void JustFastUi::changePathAndUpdateViews(const std::filesystem::path& newPath)
{
    if (!std::filesystem::is_directory(newPath)) {
        return;
    }

    currentPath = newPath;
    currentPathCached = currentPath.wstring();
    updateMainView();
    updateParentView();
}

void JustFastUi::selectFile(const std::filesystem::path& selectedFile)
{
    filesystemOperations.appendSelectedFiles(selectedFile);
    updateSelectedCounter();
}

void JustFastUi::toggleHiddenFiles()
{
    isShowingHiddenFile = !isShowingHiddenFile;
    updateMainView();
    updateParentView();
}

void JustFastUi::selectOperation(FileSystemOperations::Operation o)
{
    filesystemOperations.setOperation(o);
    updateOperationView();
}

void JustFastUi::performOperation(const std::filesystem::path& dest)
{
    try {
        filesystemOperations.performOperation(dest);
        updateAllUi();
    } catch (std::filesystem::filesystem_error& error) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        statusMessage = converter.from_bytes(error.what());
        changePathAndUpdateViews(currentPath.parent_path());
    }
}

// clang-format off
ftxui::Element JustFastUi::Render()
{
    using namespace ftxui;

    if(filesystemOperations.lastOperationIsCompleated()){
	updateAllUi(currentFolderSelected);
	filesystemOperations.clearLastOperationStatus();
    }

    auto currentPathView = text(currentPathCached);
      
    auto mainView =
        hbox({
            parentFolder->Render() | frame,
            separator(),
            currentFolder->Render() | flex | frame
	});

    auto statusLine =
        hbox({
            text(L"["),
            gauge(0.5) | flex | size(WIDTH, EQUAL, 10),
            text(L"] "),
            text(spaceInfo),
            text(statusMessage) | center | flex,
            text(statusSelected + L" " + operationView)
	});

    return
        window(
            text(L"Just Fast") | bold | center,
            vbox({
                currentPathView,
                separator(),
                mainView | flex,
                separator(),
                statusLine
	    })
        );
}
// clang-format on

bool JustFastUi::OnEvent(ftxui::Event event)
{
    if (event == ftxui::Event::Character('l') || event == ftxui::Event::ArrowRight) {
        if (currentFolderEntries.empty()) {
            return true;
        }

        changePathAndUpdateViews(currentPath / currentFolderEntries[currentFolderSelected]);
        return true;
    }

    if (event == ftxui::Event::Character('h') || event == ftxui::Event::ArrowLeft) {
        changePathAndUpdateViews(currentPath.parent_path());
        return true;
    }

    if (event == ftxui::Event::Character('f')) {
        selectFile(currentPath / currentFolderEntries[currentFolderSelected]);
        return true;
    }

    if (event == ftxui::Event::Character(' ')) {
        performOperation(currentPath);
        return true;
    }

    if (event == ftxui::Event::Character('c')) {
        selectOperation(FileSystemOperations::Operation::COPY);
        return true;
    }

    if (event == ftxui::Event::Character('m')) {
        selectOperation(FileSystemOperations::Operation::MOVE);
        return true;
    }

    if (event == ftxui::Event::Character('d')) {
        selectOperation(FileSystemOperations::Operation::DELETE);
        return true;
    }

    if (event == ftxui::Event::Character('a')) {
        toggleHiddenFiles();
        return true;
    }

    if (event == ftxui::Event::Escape) {
        filesystemOperations.clearSelectedFiles();
        return true;
    }

    if (event == ftxui::Event::Character('q')) {
        quit();
    }

    return ComponentBase::OnEvent(event);
}
