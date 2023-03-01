#pragma once

#define PRINT_(a){std::wcout << a << std::endl;}

#include <iostream>
//#include <string>
#include <vector>
#include <map>
//#include <algorithm> // لعمل تتالي على المصفوفات

#include "Tokens.h"

// المعرب اللغوي
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DedentSpecifier { // صنف يقوم بتحديد المسافة البادئة الحالية والاخيرة
public:
    int spaces = 0;
    DedentSpecifier* previous = nullptr;
};

class Lexer {
public:
    wstr fileName{}, input_{};
    wchar_t currentChar{};
    uint32_t tokLine = 1;
    uint32_t tokIndex = -1;
    uint32_t tokPos = 0;
    std::vector<Token> tokens_{};
    DedentSpecifier* dedentSpec = new DedentSpecifier; // حساب المسافات البادئة والراجعة

    ////////////

    Lexer(wstr _fileName, wstr _input);

    void advance();

    void make_token();


    bool symbol_lex();
    
    bool two_symbol_lex();

    bool word_lex();


    void skip_space();

    void make_indent();

    void make_newline();

    void make_number();

    void make_name();

    void make_string();

    void make_plus_equal();

    void make_minus_equal();

    void make_multiply_equal();

    void make_power_equal();

    void make_divide();

    void make_not_equals();

    void make_equals();

    void make_less_than();

    void make_greater_than();

    void skip_comment();
};
