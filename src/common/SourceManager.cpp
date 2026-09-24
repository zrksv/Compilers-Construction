#include "SourceManager.hpp"

bool SourceManager::is_eof() const {
    return cursor >= source.size();
}

char SourceManager::peek() const {
    if (is_eof()) {
        return '\0';
    }
    return source[cursor];
}

char SourceManager::peek_next() const {
    if (cursor + 1 >= source.size()) {
        return '\0';
    }
    return source[cursor + 1];
}

Location SourceManager::get_location() const {
    return location;
}

char SourceManager::get() {
    if (is_eof()) {
        return '\0';
    }

    char c = source[cursor];
    cursor++;

    if (c == '\n') {
        location.row++;
        location.column = 1;
    } else {
        location.column++;
    }

    return c;
}