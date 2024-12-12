#pragma once

namespace fs {

bool setupFs();
bool readQuick(const char *filename, String *content);
bool writeQuick(const char *filename, const char *content);
bool remove_(const char *filename);

} // namespace fs
