#include "BaseChecker.h"

BaseChecker::BaseChecker(const PositionManager &m, unsigned int inc, unsigned int ins) : m(m), inc(inc), ins(ins) { }

void BaseChecker::CheckBraces(const clang::Decl *d, Position csp) {
    Position dp = m.GetPosition(d);
    CheckBraces(dp.Begin, csp);
}

void BaseChecker::CheckBraces(const clang::Stmt *s, Position csp) {
    Position sp = m.GetPosition(s);
    CheckBraces(sp.Begin, csp);
}

void BaseChecker::CheckBraces(Position::LC loc, Position csp) {
    if (csp.Begin.Line != loc.Line && !csp.CheckBeginColumn(inc)) {
        Position::Throw(csp);
    }

    if (csp.End.Line != csp.Begin.Line && !csp.CheckEndColumn(inc)) {
        Position::Throw(csp);
    }
}
