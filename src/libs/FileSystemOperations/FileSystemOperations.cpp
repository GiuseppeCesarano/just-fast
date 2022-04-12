#include "FileSystemOperations.h"
#include <algorithm>

void FileSystemOperations::setSelectedFiles(const std::vector<std::filesystem::path>& sFiles)
{
    selectedFiles = sFiles;
}

void FileSystemOperations::appendSelectedFiles(const std::filesystem::path& fileToAppend)
{
    //I hate that.
    auto it = std::find(selectedFiles.begin(), selectedFiles.end(), fileToAppend);
    if (it != selectedFiles.end()) {
        selectedFiles.erase(it);
    } else {
        selectedFiles.push_back(fileToAppend);
    }
}

void FileSystemOperations::clearSelectedFiles()
{
    selectedFiles.clear();
}

std::vector<std::filesystem::path> FileSystemOperations::getSelectedFiles()
{
    return selectedFiles;
}

size_t FileSystemOperations::countSelectedFiles()
{
    return selectedFiles.size();
}

void FileSystemOperations::setOperation(FileSystemOperations::Operation o)
{
    selectedOperation = o;
}

FileSystemOperations::Operation FileSystemOperations::getOperation()
{
    return selectedOperation;
}

bool FileSystemOperations::lastOperationIsCompleated()
{
    try {
        return lastOperationCompleated.get();
    } catch (std::future_error& error) {
        return false;
    }
}

void FileSystemOperations::clearLastOperationStatus()
{
    lastOperationCompleated = std::async(std::launch::deferred, []() { return false; });
}

void FileSystemOperations::clearOperation()
{
    selectedOperation = NOT_SELECTED;
}

void FileSystemOperations::performOperation(const std::filesystem::path& dest)
{
    if (selectedOperation == NOT_SELECTED) {
        return;
    };

    lastOperationCompleated = std::async(
        std::launch::async, [](const std::vector<std::filesystem::path>& selectedFiles, const std::filesystem::path& dest, Operation selectedOperation) -> bool {
            try {
                switch (selectedOperation) {

                case NOT_SELECTED:
                    return false;
                    break;

                case COPY:
                    for (const auto& p : selectedFiles) {
                        std::filesystem::copy(p, dest / p.filename(), std::filesystem::copy_options::recursive);
                    }
                    break;

                case DELETE:
                    for (const auto& p : selectedFiles) {
                        std::filesystem::remove_all(p);
                    }
                    break;

                case MOVE:
                    for (const auto& p : selectedFiles) {
                        std::filesystem::rename(p, dest / p.filename());
                    }
                    break;
                }
            } catch (std::filesystem::filesystem_error& e) {
                throw;
            }
            return true;
        },
        selectedFiles, dest, selectedOperation);

    clearOperation();
    clearSelectedFiles();
}
