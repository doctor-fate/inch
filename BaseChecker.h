#pragma once

#include "PositionManager.h"

class BaseChecker {
protected:
    BaseChecker(const PositionManager &m, unsigned int inc, unsigned int ins);

    void CheckBraces(const clang::Decl *d, Position csp);

    void CheckBraces(const clang::Stmt *s, Position csp);

    void CheckBraces(Position::LC loc, Position csp);

    PositionManager m;
    unsigned int inc;
    unsigned int ins;
};
