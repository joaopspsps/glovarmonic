#include <LittleFS.h>

#include "serial.hpp"

#define FS LittleFS

namespace fs {

bool setupFs() { return !!FS.begin(false); }

bool readQuick(const char *path, String *content) {
    auto file = FS.open(path);

    if (not file)
        return false;

    while (file.available())
        *content += file.readString();

    file.close();

    return true;
}

bool writeQuick(const char *path, const char *content) {
    auto file = FS.open(path, FILE_WRITE);

    if (not file or not file.print(content))
        return false;

    file.close();

    return true;
}

bool remove_(const char *path) { return FS.remove(path); }

} // namespace fs
