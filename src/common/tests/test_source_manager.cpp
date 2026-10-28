#include <iostream>
#include <cassert>
#include "../SourceManager.hpp"

void test_source_manager() {
    SourceManager sm("ab\n c\t\n");
    assert(sm.peek() == 'a');
    assert(sm.peek_next() == 'b');
    assert(sm.get() == 'a');
    assert(sm.get_location().column == 2);

    assert(sm.get() == 'b');
    assert(sm.get() == '\n');
    assert(sm.get_location().row == 2);
    assert(sm.get_location().column == 1);

    assert(sm.get() == ' ');
    assert(sm.get() == 'c');
    assert(sm.get() == '\t');
    assert(sm.get() == '\n');
    assert(sm.get_location().row == 3);
    assert(sm.get_location().column == 1);
    assert(sm.is_eof());
    std::cout << "[PASS] test_source_manager\n";
}

void test_empty_source() {
    SourceManager sm("");
    assert(sm.is_eof());
    assert(sm.peek() == '\0');
    assert(sm.peek_next() == '\0');
    assert(sm.peek_offset(0) == '\0');
    assert(sm.get() == '\0');
    assert(sm.get_location().row == 1);
    assert(sm.get_location().column == 1);
    std::cout << "[PASS] test_empty_source\n";
}

void test_lookahead() {
    SourceManager sm("abc");
    assert(sm.peek_offset(0) == 'a');
    assert(sm.peek_offset(1) == 'b');
    assert(sm.peek_offset(2) == 'c');
    assert(sm.peek_offset(3) == '\0');
    assert(sm.peek_offset(100) == '\0');
    assert(sm.get_location().column == 1);

    sm.get();
    sm.get();
    assert(sm.peek() == 'c');
    assert(sm.peek_next() == '\0');
    assert(sm.peek_offset(1) == '\0');
    assert(!sm.is_eof());
    std::cout << "[PASS] test_lookahead\n";
}

void test_get_at_eof() {
    SourceManager sm("x");
    assert(sm.get() == 'x');
    assert(sm.is_eof());
    assert(sm.get() == '\0');
    assert(sm.get() == '\0');
    assert(sm.get_location().row == 1);
    assert(sm.get_location().column == 2);
    std::cout << "[PASS] test_get_at_eof\n";
}

void test_carriage_return() {
    SourceManager sm("a\r\nb");
    sm.get();
    assert(sm.get() == '\r');
    assert(sm.get_location().row == 1);
    assert(sm.get_location().column == 3);
    assert(sm.get() == '\n');
    assert(sm.get_location().row == 2);
    assert(sm.get_location().column == 1);
    assert(sm.get() == 'b');
    std::cout << "[PASS] test_carriage_return\n";
}

void test_utf8_bom() {
    SourceManager bom("\xEF\xBB\xBFvar");
    assert(bom.peek() == 'v');
    assert(bom.peek_offset(2) == 'r');
    assert(bom.get_location().row == 1);
    assert(bom.get_location().column == 1);
    assert(bom.get() == 'v');
    assert(bom.get_location().column == 2);

    SourceManager only_bom("\xEF\xBB\xBF");
    assert(only_bom.is_eof());

    SourceManager partial("\xEF\xBBx");
    assert(static_cast<unsigned char>(partial.peek()) == 0xEF);

    SourceManager inner("a\xEF\xBB\xBF");
    assert(inner.get() == 'a');
    assert(static_cast<unsigned char>(inner.peek()) == 0xEF);
    std::cout << "[PASS] test_utf8_bom\n";
}

int main() {
    test_source_manager();
    test_empty_source();
    test_lookahead();
    test_get_at_eof();
    test_carriage_return();
    test_utf8_bom();
}