#pragma once

#include "PositionManager.h"

class BaseChecker {
protected:
    BaseChecker(const PositionManager &m, unsigned int inc);

    void CheckBraces(const clang::Decl *d, Position csp);

    void CheckBraces(Position::LC loc, Position csp);

    unsigned int DetermineIndent(Position dp, clang::DeclContext *dc);

    unsigned int DetermineIndent(clang::CompoundStmt *s);

    bool InOneLine(Position p);

    bool OnTheSameLine(Position a, Position b);

    bool HasIndent(Position p);

    PositionManager m;
    unsigned int inc;
};
