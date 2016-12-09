#include "StmtIndentsChecker.h"

StmtIndentsChecker::StmtIndentsChecker(const PositionManager &m, unsigned int inc) : BaseChecker(m, inc) { }

void StmtIndentsChecker::VisitIfStmt(clang::IfStmt *s) {
    clang::Stmt *ts = s->getThen();
    Position cep = m.FromLocation(s->getCond()->getLocEnd());
    Position tsp = m.GetPosition(ts);
    if (clang::isa<clang::CompoundStmt>(ts)) {
        visitCompoundStmt((clang::CompoundStmt *) ts, cep);
    } else {
        if (!OnTheSameLine(tsp, cep)) {
            if (!HasIndent(tsp)) {
                Position::Throw(tsp);
            }

            auto old = inc;
            inc = tsp.Begin.Column - 1;
            Visit(ts);
            inc = old;
        } else {
            Visit(ts);
        }
    }

    clang::Stmt *es = s->getElse();
    if (es == nullptr) {
        return;
    }

    Position elp = m.FromLocation(s->getElseLoc());
    if (tsp.End.Line != elp.End.Line) {
        elp.CheckBeginColumnThrow(this->inc);
    }

    Position esp = m.GetPosition(es);
    if (clang::isa<clang::CompoundStmt>(es)) {
        visitCompoundStmt((clang::CompoundStmt *) es, elp);
    } else {
        if (!OnTheSameLine(esp, elp)) {
            if (!HasIndent(esp)) {
                Position::Throw(esp);
            }

            auto old = inc;
            inc = esp.Begin.Column - 1;
            Visit(es);
            inc = old;
        } else {
            Visit(es);
        }
    }
}

void StmtIndentsChecker::VisitWhileStmt(clang::WhileStmt *s) {
    clang::Stmt *bs = s->getBody();
    Position sp = m.FromLocation(s->getCond()->getLocEnd());
    Position bsp = m.GetPosition(bs);

    if (clang::isa<clang::CompoundStmt>(bs)) {
        visitCompoundStmt((clang::CompoundStmt *) bs, sp);
    } else {
        if (!OnTheSameLine(bsp, sp) && !HasIndent(bsp)) {
            Position::Throw(bsp);
        }

        auto old = inc;
        inc = bsp.Begin.Column - 1;
        Visit(bs);
        inc = old;
    }
}

void StmtIndentsChecker::VisitForStmt(clang::ForStmt *s) {
    clang::Stmt *bs = s->getBody();
    Position sp = m.FromLocation(s->getRParenLoc());
    Position bsp = m.GetPosition(bs);

    if (clang::isa<clang::CompoundStmt>(bs)) {
        visitCompoundStmt((clang::CompoundStmt *) bs, sp);
    } else {
        if (!OnTheSameLine(bsp, sp) && !HasIndent(bsp)) {
            Position::Throw(bsp);
        }

        auto old = inc;
        inc = bsp.Begin.Column - 1;
        Visit(bs);
        inc = old;
    }
}

void StmtIndentsChecker::VisitDoStmt(clang::DoStmt *s) {
    clang::Stmt *bs = s->getBody();
    Position sp = m.FromLocation(s->getDoLoc());
    Position bsp = m.GetPosition(bs);

    if (clang::isa<clang::CompoundStmt>(bs)) {
        visitCompoundStmt((clang::CompoundStmt *) bs, sp);
    } else {
        if (!OnTheSameLine(bsp, sp) && !HasIndent(bsp)) {
            Position::Throw(bsp);
        }

        auto old = inc;
        inc = bsp.Begin.Column - 1;
        Visit(bs);
        inc = old;
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

unsigned int StmtIndentsChecker::visitSwitchCaseStmt(clang::SwitchCase *s, unsigned int cinc) {
    clang::Stmt *ss = s->getSubStmt();
    Position sp = m.GetPosition(s), ssp = m.GetPosition(ss);
    if (clang::isa<clang::CompoundStmt>(ss)) {
        auto old = inc;
        if (!OnTheSameLine(sp, ssp)) {
            inc = ssp.Begin.Column - 1;
            visitCompoundStmt((clang::CompoundStmt *) ss, sp);
            inc = old;
        } else {
            if (!InOneLine(ssp)) {
                inc = (ssp.End.Column - 1);
                visitCompoundStmt((clang::CompoundStmt *) ss, sp);
                inc = old;
            }
            return 0;
        }
    } else if (clang::isa<clang::SwitchCase>(ss)) {
        return visitSwitchCaseStmt((clang::SwitchCase *) ss, cinc);
    } else {
        if (!OnTheSameLine(sp, ssp)) {
            if (ssp.Begin.Column <= cinc) {
                Position::Throw(ssp);
            }

            auto old = inc;
            inc = ssp.Begin.Column - 1;
            Visit(ss);
            inc = old;
        } else {
            Visit(ss);
            return 0;
        }
    }

    return (ssp.Begin.Column - 1) - cinc;
}

void StmtIndentsChecker::checkSwitchBody(clang::SwitchStmt *s, clang::CompoundStmt *cs) {
    clang::Stmt *fs = cs->body_front();
    Position fsp = m.GetPosition(fs);
    unsigned int cinc = fsp.Begin.Column - 1;
    unsigned int cind = 0;

    clang::SwitchCase *last = nullptr;
    for (auto chs : cs->body()) {
        if (clang::isa<clang::NullStmt>(chs)) {
            continue;
        }

        Position chsp = m.GetPosition(chs);
        if (clang::isa<clang::SwitchCase>(chs)) {
            chsp.CheckBeginColumnThrow(cinc);
            cind = visitSwitchCaseStmt((clang::SwitchCase *) chs, cinc);
            last = (clang::SwitchCase *) chs;
        } else {
            Position lp = m.GetPosition(last);
            if (OnTheSameLine(chsp, lp)) {
                if (clang::isa<clang::CompoundStmt>(chs)) {
                    visitCompoundStmt((clang::CompoundStmt *) chs, lp);
                } else {
                    Visit(chs);
                }
                continue;
            }

            if (cind == 0) {
                if ((cind = (chsp.Begin.Column - 1) - cinc) == 0) {
                    Position::Throw(chsp);
                }
            }

            chsp.CheckBeginColumnThrow(cinc + cind);
            auto old = inc;
            inc = cinc + cind;
            Visit(chs);
            inc = old;
        }
    }
}

void StmtIndentsChecker::VisitCallExpr(clang::CallExpr *e) {
    Position ep = m.GetPosition(e);

    if (e->getNumArgs() > 0) {
        clang::Expr *fae = e->getArg(0);
        Position faep = m.GetPosition(fae);

        if (faep.Begin.Line != ep.Begin.Line && faep.Begin.Column == ep.Begin.Column) {
            Position::Throw(faep);
        }

        clang::Expr *prev = fae;
        for (auto *ae : e->arguments()) {
            auto pp = m.GetPosition(prev), aep = m.GetPosition(ae);
            if (aep.Begin.Line != pp.End.Line && aep.Begin.Column == ep.Begin.Column) {
                Position::Throw(aep);
            }
            prev = ae;
        }
    }
}

void StmtIndentsChecker::visitCompoundStmt(clang::CompoundStmt *s, Position pp) {
    Position sp = m.GetPosition(s);
    CheckBraces(pp.Begin, sp);
    if (InOneLine(sp)) {
        return;
    }

    unsigned int inc = DetermineIndent(s);
    if ((inc == 0 || inc == this->inc) && !s->body_empty()) {
        Position::Throw(sp);
    }
    unsigned int oldInc = this->inc;
    this->inc = inc;

    clang::Stmt *pr = nullptr;
    for (clang::Stmt *cs : s->body()) {
        Position csp = m.GetPosition(cs);
        if (OnTheSameLine(csp, sp)) {
            Position::Throw(csp);
        }

        if (clang::isa<clang::NullStmt>(cs)) {
            continue;
        } else if (clang::isa<clang::LabelStmt>(cs)) {
            if (csp.Begin.Column > this->inc + 1) {
                Position::Throw(csp);
            }
        } else if (pr != nullptr && OnTheSameLine(m.GetPosition(pr), csp) && clang::isa<clang::Expr>(cs)) {
            continue;
        } else if (clang::isa<clang::CompoundStmt>(cs)) {
            visitCompoundStmt((clang::CompoundStmt *) cs, csp);
            pr = cs;
            continue;
        } else {
            csp.CheckBeginColumnThrow(this->inc);
        }

        Visit(cs);
        pr = cs;
    }

    this->inc = oldInc;
}

void StmtIndentsChecker::VisitLabelStmt(clang::LabelStmt *s) {
    Position sp = m.GetPosition(s);
    clang::Stmt *ss = s->getSubStmt();
    Position ssp = m.GetPosition(ss);

    unsigned int oldInc = this->inc;
    if (OnTheSameLine(ssp, sp)) {
        this->inc = sp.Begin.Column - 1;
    } else {
        this->inc = ssp.Begin.Column - 1;
    }

    Visit(ss);
    this->inc = oldInc;
}
