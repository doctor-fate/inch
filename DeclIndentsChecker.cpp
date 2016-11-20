#include "DeclIndentsChecker.h"

DeclIndentsChecker::DeclIndentsChecker(clang::ASTContext &context, unsigned int inc) : BaseChecker(
    context.getSourceManager(), inc) { }

void DeclIndentsChecker::VisitTranslationUnitDecl(clang::TranslationUnitDecl *unit) {
    VisitDeclContext(unit, unit);
}

void DeclIndentsChecker::VisitDeclContext(clang::Decl *d, clang::DeclContext *dc, bool indent) {
    Position dp = m.GetPosition(d);
    unsigned int inc = 0, oldInc = 0;

    if (indent) {
        inc = DetermineIndent(dp, dc);
        oldInc = this->inc;
        this->inc = inc;
    }

    for (auto cd : dc->decls()) {
        Position cdp = m.GetPosition(cd);
        if (cdp.Filename != "input.cc") {
            continue;
        }

        clang::FunctionDecl *fd = clang::dyn_cast_or_null<clang::FunctionDecl>(cd);
        if (fd != nullptr && fd->isImplicit()) {
            continue;
        }

        if (!cdp.CheckBeginColumn(this->inc) &&
            !(clang::isa<clang::RecordDecl>(cd) || clang::isa<clang::EnumDecl>(cd))) {
            Position::Throw(cdp);
        }

        Visit(cd);
    }

    this->inc = oldInc;
}

void DeclIndentsChecker::VisitRecordDecl(clang::RecordDecl *d) {
    Position p = m.FromSourceRange(d->getBraceRange());
    if (p.Begin.Line != p.End.Line) {
        VisitDeclContext(d, d, true);
    }
}

void DeclIndentsChecker::VisitFieldDecl(clang::FieldDecl *d) {
    Position p = m.GetPosition(d);
    p.CheckBeginColumnThrow(inc);
}

void DeclIndentsChecker::VisitEnumDecl(clang::EnumDecl *d) {
    Position dp = m.GetPosition(d);
    unsigned int inc = DetermineIndent(dp, d);
    unsigned int oldInc = this->inc;
    this->inc = inc;

    clang::Decl *prev = d;
    for (auto cd : d->decls()) {
        Position pp = m.GetPosition(prev), cdp = m.GetPosition(cd);
        if (!OnTheSameLine(cdp, pp) && !cdp.CheckBeginColumn(this->inc)) {
            Position::Throw(cdp);
        }
        prev = cd;
    }

    this->inc = oldInc;
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

        StmtIndentsChecker sic(m, inc);
        sic.visitCompoundStmt(cs, fp);
    }
}

void DeclIndentsChecker::VisitVarDecl(clang::VarDecl *d) {
    clang::Expr *ie = d->getInit();
    if (ie != nullptr) {
        StmtIndentsChecker sic(m, inc);
        sic.Visit(ie);
    }
}
