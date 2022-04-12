#include "FileSystemOperations.h"
#include <algorithm>

void FileSystemOperations::setSelectedFiles(const std::vector<std::filesystem::path>& sFiles)
{
    selectedFiles = sFiles;
}

void FileSystemOperations::appendSelectedFiles(const std::filesystem::path& fileToAppend)
{
    auto it = std::find(std::begin(selectedFiles), std::end(selectedFiles), fileToAppend);

    /**
     * Why are there void casts?
     * 
     * Ternary operator arguments have to return the same type,
     * but .erase and .push_back methods return different things:
     *     .erase     -> iterator
     *     .push_back -> void
     *
     * Of course, you can skip the void cast at the .push_back method,
     * but I kept it in order to make that piece of code more consistent.
     *
     * And now I hope there's no need for any hatred :P
     */
    (it != std::end(selectedFiles))
        ? static_cast<void>(selectedFiles.erase(it))
        : static_cast<void>(selectedFiles.push_back(fileToAppend));
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
