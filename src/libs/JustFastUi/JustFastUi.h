#pragma once

#include "FileSystemOperations/FileSystemOperations.h"
#include <filesystem>
#include <ftxui/component/component.hpp>

struct JustFastOptions{
    std::filesystem::path path;
    bool showHiddenFiles;
    bool sortFiles = true;
    std::function<ftxui::Element(const ftxui::EntryState& state)> transformFolder = nullptr;
    std::function<ftxui::Element(const ftxui::EntryState& state)> transformFile = nullptr;
};

class JustFastUi : public ftxui::ComponentBase {

private:
    std::function<void()> quit;
    std::wstring spaceInfo, statusMessage, statusSelected, operationView, currentPathCached;
    std::filesystem::path currentPath;
    std::vector<std::wstring> parentFolderEntries;
    std::vector<std::wstring> currentFolderEntries;
    int parentFolderSelected = 0;
    int currentFolderSelected = 0;
    std::string currentFolderNameSelected = "";
    ftxui::Component parentFolder = ftxui::Menu(&parentFolderEntries, &parentFolderSelected);
    ftxui::Component currentFolder = ftxui::Container::Vertical({}, &currentFolderSelected);
    float diskSpaceAvailable;
    bool isShowingHiddenFile; 
    bool isSortFiles; 
    std::function<ftxui::Element(const ftxui::EntryState& state)> transformElementFolder = nullptr;
    std::function<ftxui::Element(const ftxui::EntryState& state)> transformElementFile = nullptr;

    void updateParentView();
    void updateMainView(size_t = 0);
    void updateOperationView();
    void updateSelectedCounter();
    void updateAllUi(size_t = 0);

    void changePathAndUpdateViews(const std::filesystem::path&);
    void selectFile(const std::filesystem::path&);
    void toggleHiddenFiles();
    void selectOperation(FileSystemOperations::Operation);
    void performOperation(const std::filesystem::path&);

    FileSystemOperations filesystemOperations;

public:
    JustFastUi(const JustFastOptions&);

    void setQuitFunction(std::function<void()>);

    ftxui::Element Render() override;
    bool OnEvent(ftxui::Event) override;
};
