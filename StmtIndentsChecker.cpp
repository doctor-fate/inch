#include "StmtIndentsChecker.h"

StmtIndentsChecker::StmtIndentsChecker(const PositionManager &m, unsigned int inc, unsigned int ins)
    : BaseChecker(m, inc, ins) { }

void StmtIndentsChecker::VisitCompoundStmt(clang::CompoundStmt *s) {
    Position sp = m.GetPosition(s);
    if (sp.Begin.Line == sp.End.Line && s->body().end() - s->body().begin() == 1) {
        return;
    }

    inc += ins;

    for (clang::Stmt *cs : s->body()) {
        auto csp = m.GetPosition(cs);
        if (clang::isa<clang::NullStmt>(cs)) {
            continue;
        } else if (clang::isa<clang::LabelStmt>(cs)) {
            csp.CheckBeginColumnThrow(inc - ins);
        } else {
            csp.CheckBeginColumnThrow(inc);
        }
        Visit(cs);
    }

    inc -= ins;
}

void StmtIndentsChecker::VisitIfStmt(clang::IfStmt *s) {
    clang::Stmt *ts = s->getThen();
    Position::LC ifp = m.FromLocation(s->getIfLoc());
    Position tsp = m.GetPosition(ts);
    if (clang::isa<clang::CompoundStmt>(ts)) {
        CheckBraces(s, tsp);
        Visit(ts);
    } else {
        if (tsp.Begin.Line != ifp.Line && !tsp.CheckBeginColumn(inc + ins)) {
            std::cout << inc << ' ' << ins << std::endl;
            Position::Throw(tsp);
        }
        inc += ins;
        Visit(ts);
        inc -= ins;
    }

    clang::Stmt *es = s->getElse();
    if (es == nullptr) {
        return;
    }

    Position::LC elp = m.FromLocation(s->getElseLoc());
    Position esp = m.GetPosition(es);
    if (clang::isa<clang::CompoundStmt>(es)) {
        CheckBraces(es, esp);
        Visit(es);
    } else {
        if (esp.Begin.Line != elp.Line) {
            if (!esp.CheckBeginColumn(inc + ins)) {
                Position::Throw(esp);
            }
            inc += ins;
            Visit(es);
            inc -= ins;
        } else {
            Visit(es);
        }
    }
}

void StmtIndentsChecker::VisitWhileStmt(clang::WhileStmt *s) {
    clang::Stmt *bs = s->getBody();
    Position::LC sp = m.FromLocation(s->getCond()->getLocEnd());
    Position bsp = m.GetPosition(bs);
    if (clang::isa<clang::CompoundStmt>(bs)) {
        CheckBraces(sp, bsp);
        Visit(bs);
    } else {
        if (bsp.Begin.Line != sp.Line && !bsp.CheckBeginColumn(inc + ins)) {
            Position::Throw(bsp);
        }
        inc += ins;
        Visit(bs);
        inc -= ins;
    }
}

void StmtIndentsChecker::VisitForStmt(clang::ForStmt *s) {
    clang::Stmt *bs = s->getBody();
    Position::LC sp = m.FromLocation(s->getRParenLoc());
    Position bsp = m.GetPosition(bs);
    if (clang::isa<clang::CompoundStmt>(bs)) {
        CheckBraces(sp, bsp);
        Visit(bs);
    } else {
        if (bsp.Begin.Line != sp.Line && !bsp.CheckBeginColumn(inc + ins)) {
            Position::Throw(bsp);
        }
        inc += ins;
        Visit(bs);
        inc -= ins;
    }
}

void StmtIndentsChecker::VisitDoStmt(clang::DoStmt *s) {
    clang::Stmt *bs = s->getBody();
    Position::LC sp = m.FromLocation(s->getRParenLoc());
    Position bsp = m.GetPosition(bs);
    if (clang::isa<clang::CompoundStmt>(bs)) {
        CheckBraces(sp, bsp);
        Visit(bs);
    } else {
        if (bsp.Begin.Line != sp.Line && !bsp.CheckBeginColumn(inc + ins)) {
            Position::Throw(bsp);
        }
        inc += ins;
        Visit(bs);
        inc -= ins;
    }
}

void StmtIndentsChecker::VisitSwitchStmt(clang::SwitchStmt *s) {
    auto bs = s->getBody();
    auto cbs = clang::dyn_cast_or_null<clang::CompoundStmt>(bs);
    if (cbs != nullptr) {
        Position::LC sp = m.FromLocation(s->getCond()->getLocEnd());
        CheckBraces(sp, m.GetPosition(cbs));
        checkSwitchBody(s, cbs);
    }
}

void StmtIndentsChecker::VisitCaseStmt(clang::CaseStmt *s) {
    auto ss = s->getSubStmt();
    Position sp = m.GetPosition(s), ssp = m.GetPosition(ss);
    if (clang::isa<clang::CompoundStmt>(ss)) {
        CheckBraces(s, ssp);
    } else if (clang::isa<clang::CaseStmt>(ss) || clang::isa<clang::DefaultStmt>(ss)) {
        if (ssp.Begin.Line != sp.Begin.Line && !ssp.CheckBeginColumn(inc)) {
            Position::Throw(ssp);
        }
    } else {
        if (ssp.Begin.Line != sp.Begin.Line && !ssp.CheckBeginColumn(inc + ins)) {
            Position::Throw(ssp);
        }
    }

    Visit(ss);
}

void StmtIndentsChecker::checkSwitchBody(clang::SwitchStmt *s, clang::CompoundStmt *cs) {
    for (auto chs : cs->body()) {
        Position chsp = m.GetPosition(chs);
        if (clang::isa<clang::CaseStmt>(chs) || clang::isa<clang::DefaultStmt>(chs)) {
            chsp.CheckBeginColumnThrow(inc);
        } else if (clang::isa<clang::CompoundStmt>(chs)) {
            CheckBraces(s, chsp);
        } else {
            chsp.CheckBeginColumnThrow(inc + ins);
        }

        Visit(chs);
    }
}

void StmtIndentsChecker::VisitCallExpr(clang::CallExpr *e) {
    Position ep = m.GetPosition(e);
    clang::Expr *prev = e;

    if (e->getNumArgs() > 0) {
        clang::Expr *fae = e->getArg(0);
        Position faep = m.GetPosition(fae);

        if (faep.Begin.Line != ep.Begin.Line && faep.Begin.Column == ep.Begin.Column) {
            Position::Throw(faep);
        }

        prev = fae;
        for (auto *ae : e->arguments()) {
            auto pp = m.GetPosition(prev), aep = m.GetPosition(ae);
            if (aep.Begin.Line != pp.End.Line && aep.Begin.Column == ep.Begin.Column) {
                Position::Throw(aep);
            }
            prev = ae;
        }
    }
}