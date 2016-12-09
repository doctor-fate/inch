#include "BaseChecker.h"

BaseChecker::BaseChecker(const PositionManager &m, unsigned int inc) : m(m), inc(inc) { }

void BaseChecker::CheckBraces(const clang::Decl *d, Position csp) {
    Position dp = m.GetPosition(d);
    CheckBraces(dp.Begin, csp);
}

void BaseChecker::CheckBraces(Position::LC loc, Position csp) {
    if (csp.Begin.Line != loc.Line && !csp.CheckBeginColumn(inc)) {
        Position::Throw(csp);
    }

    if (csp.End.Line != csp.Begin.Line && !csp.CheckEndColumn(inc)) {
        Position::Throw(csp);
    }
}

unsigned int BaseChecker::DetermineIndent(Position dp, clang::DeclContext *dc) {
    for (auto cd : dc->decls()) {
        Position cdp = m.GetPosition(cd);
        if (cdp.Filename != "input.cc") {
            continue;
        }

        clang::FunctionDecl *fd = clang::dyn_cast_or_null<clang::FunctionDecl>(dc);
        if (fd != nullptr && fd->isImplicit()) {
            continue;
        }

        if (!OnTheSameLine(cdp, dp)) {
            return cdp.Begin.Column - 1;
        }
    }

    return 0;
}

unsigned int BaseChecker::DetermineIndent(clang::CompoundStmt *s) {
    Position sp = m.GetPosition(s);
    clang::LabelStmt *ls = nullptr;
    for (auto cs : s->body()) {
        Position csp = m.GetPosition(cs);
        if (clang::isa<clang::NullStmt>(cs)) {
            continue;
        }
        if (clang::isa<clang::LabelStmt>(cs)) {
            if (ls == nullptr) {
                ls = clang::dyn_cast<clang::LabelStmt>(cs);
            }
            continue;
        }

        if (!OnTheSameLine(csp, sp)) {
            return csp.Begin.Column - 1;
        }
    }

    if (ls != nullptr) {
        Position lsp = m.GetPosition(ls);
        clang::Stmt *lss = ls->getSubStmt();
        if (lss != nullptr) {
            Position lssp = m.GetPosition(lss);
            if (!OnTheSameLine(lsp, lssp)) {
                return lssp.Begin.Column - 1;
            }
        }
        return lsp.Begin.Column - 1;
    }

    return 0;
}

bool BaseChecker::OnTheSameLine(Position a, Position b) {
    return a.Begin.Line == b.Begin.Line;
}

bool BaseChecker::InOneLine(Position p) {
    return p.Begin.Line == p.End.Line;
}

bool BaseChecker::HasIndent(Position p) {
    return p.Begin.Column - 1 > inc;
}
