#include "DeclIndentsChecker.h"

DeclIndentsChecker::DeclIndentsChecker(clang::ASTContext &context, unsigned int inc, unsigned int ins) :
    BaseChecker(context.getSourceManager(), inc, ins) { }

void DeclIndentsChecker::VisitTranslationUnitDecl(clang::TranslationUnitDecl *unit) {
    VisitDeclContext(unit);
}

void DeclIndentsChecker::VisitDeclContext(clang::DeclContext *dc, bool indent) {
    if (indent) {
        this->inc += ins;
    }

    for (auto d : dc->decls()) {
        auto p = m.GetPosition(d);
        if (p.Filename != "input.cc") {
            continue;
        }

        auto fd = clang::dyn_cast_or_null<clang::FunctionDecl>(d);
        if (fd != nullptr && fd->isImplicit()) {
            continue;
        }

        if (!p.CheckBeginColumn(inc) && !(clang::isa<clang::RecordDecl>(d) || clang::isa<clang::EnumDecl>(d))) {
            Position::Throw(p);
        }

        Visit(d);
    }

    if (indent) {
        this->inc -= ins;
    }
}

void DeclIndentsChecker::VisitRecordDecl(clang::RecordDecl *d) {
    Position p = m.FromSourceRange(d->getBraceRange());
    if (p.Begin.Line != p.End.Line) {
        VisitDeclContext(d, true);
    }
}

void DeclIndentsChecker::VisitFieldDecl(clang::FieldDecl *d) {
    Position p = m.GetPosition(d);
    p.CheckBeginColumnThrow(inc);
}

void DeclIndentsChecker::VisitEnumDecl(clang::EnumDecl *d) {
    inc += ins;

    clang::Decl *prev = d;
    for (auto cd : d->decls()) {
        Position pp = m.GetPosition(prev), cp = m.GetPosition(cd);
        if (cp.Begin.Line != pp.Begin.Line && !cp.CheckBeginColumn(inc)) {
            Position::Throw(cp);
        }
        prev = cd;
    }

    inc -= ins;
}

void DeclIndentsChecker::VisitFunctionDecl(clang::FunctionDecl *d) {
    Position fp = m.GetPosition(d);
    clang::Decl *prev = d;

    if (d->getNumParams() > 0) {
        clang::ParmVarDecl *fpd = d->getParamDecl(0);
        Position fpdp = m.GetPosition(fpd);

        if (fpdp.Begin.Line != fp.Begin.Line && fpdp.Begin.Column == 1) {
            Position::Throw(fpdp);
        }

        prev = fpd;
        for (auto *pd : d->parameters()) {
            auto pp = m.GetPosition(prev), pdp = m.GetPosition(pd);
            if (pdp.Begin.Line != pp.Begin.Line && pdp.Begin.Column == 1) {
                Position::Throw(pdp);
            }
            prev = pd;
        }
    }

    if (d->doesThisDeclarationHaveABody()) {
        auto cs = clang::dyn_cast<clang::CompoundStmt>(d->getBody());
        auto csp = m.GetPosition(cs);
        CheckBraces(prev, csp);

        StmtIndentsChecker sic(m, inc, ins);
        sic.Visit(cs);
    }
}

void DeclIndentsChecker::VisitVarDecl(clang::VarDecl *d) {
    clang::Expr *ie = d->getInit();
    if (ie != nullptr) {
        Position dp = m.GetPosition(d), iep = m.GetPosition(ie);
        if (dp.Begin.Line != iep.Begin.Line || iep.Begin.Line != iep.End.Line) {
            Position::Throw(dp);
        }
    }
}
