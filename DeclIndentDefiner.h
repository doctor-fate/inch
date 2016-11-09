#pragma once

#include <clang/AST/DeclVisitor.h>
#include "PositionManager.h"

class DeclIndentDefiner : public clang::DeclVisitor<DeclIndentDefiner> {
public:
    DeclIndentDefiner(clang::ASTContext &context);

    void VisitFunctionDecl(clang::FunctionDecl *d);

    int Indent(clang::TranslationUnitDecl *d);

private:
    PositionManager m;
    int indent;
};
