#pragma once

#include "PositionManager.h"

class BaseChecker {
protected:
    BaseChecker(const PositionManager &m, unsigned int inc, unsigned int ins);

    void checkBraces(const clang::Decl *d, Position csp);

    void checkBraces(const clang::Stmt *s, Position csp);

    PositionManager m;
    unsigned int inc;
    unsigned int ins;
};
