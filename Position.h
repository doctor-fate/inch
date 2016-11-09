#pragma once

#include <clang/Basic/SourceManager.h>
#include <clang/AST/Stmt.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>

struct Position {
    struct LC {
        LC();

        LC(clang::SourceLocation loc, const clang::SourceManager &m);

        unsigned int Line;
        unsigned int Column;
    };

    Position();

    Position(LC begin, LC end, std::string filename);

    static void Throw(const Position &p);

    bool CheckBeginColumn(unsigned int indent);

    void CheckBeginColumnThrow(unsigned int indent);

    bool CheckEndColumn(unsigned int indent);

    void CheckEndColumnThrow(unsigned int indent);

    LC Begin;
    LC End;
    std::string Filename;
};