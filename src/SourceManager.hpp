#ifndef SOURCEMANAGER_HPP
#define SOURCEMANAGER_HPP
#include "Location.hpp"
#include <string>

class SourceManager {
private:
    std::string source;
    Location location;
    size_t cursor = 0;

public:
    explicit SourceManager(const std::string& text) : source(text) {}
    char peek() const;
    char peek_next() const;
    char get();
    bool is_eof() const;
    Location get_location() const;
};


#endif //SOURCEMANAGER_HPP
