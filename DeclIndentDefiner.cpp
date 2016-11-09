#include "DeclIndentDefiner.h"

DeclIndentDefiner::DeclIndentDefiner(clang::ASTContext &context) : m(context.getSourceManager()), indent(-1) { }

void DeclIndentDefiner::VisitFunctionDecl(clang::FunctionDecl *d) {
    if (d->hasBody()) {
        auto cs = clang::dyn_cast<clang::CompoundStmt>(d->getBody());
        Position csp = m.GetPosition(cs);
        for (auto s : cs->body()) {
            Position sp = m.GetPosition(s);
            if (sp.Begin.Line != csp.Begin.Line) {
                indent = sp.Begin.Column - 1;
                return;
            }
        }
    }
}

int DeclIndentDefiner::Indent(clang::TranslationUnitDecl *d) {
    for (auto cd : d->decls()) {
        Position cdp = m.GetPosition(cd);
        if (cdp.Filename != "input.cc") {
            continue;
        }

        Visit(cd);
        if (indent != -1) {
            return indent;
        }
    }

    return -1;
}
