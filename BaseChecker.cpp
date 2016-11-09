#include "BaseChecker.h"

BaseChecker::BaseChecker(const PositionManager &m, unsigned int inc, unsigned int ins) : m(m), inc(inc), ins(ins) { }

void BaseChecker::checkBraces(const clang::Decl *d, Position csp) {
    Position dp = m.GetPosition(d);
    if (csp.Begin.Line != dp.Begin.Line && !csp.CheckBeginColumn(inc)) {
        Position::Throw(csp);
    }

    if (csp.End.Line != csp.Begin.Line && !csp.CheckEndColumn(inc)) {
        Position::Throw(csp);
    }
}

void BaseChecker::checkBraces(const clang::Stmt *s, Position csp) {
    Position dp = m.GetPosition(s);
    if (csp.Begin.Line != dp.Begin.Line && !csp.CheckBeginColumn(inc)) {
        Position::Throw(csp);
    }

    if (csp.End.Line != csp.Begin.Line && !csp.CheckEndColumn(inc)) {
        Position::Throw(csp);
    }
}