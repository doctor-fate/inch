#pragma once

#include <clang/AST/DeclVisitor.h>
#include "BaseChecker.h"
#include "StmtIndentsChecker.h"

class DeclIndentsChecker : public BaseChecker, public clang::DeclVisitor<DeclIndentsChecker> {
public:
    DeclIndentsChecker(clang::ASTContext &context, unsigned int inc, unsigned int ins);

    void VisitTranslationUnitDecl(clang::TranslationUnitDecl *unit);

    void VisitDeclContext(clang::DeclContext *dc, bool indent = false);

    void VisitRecordDecl(clang::RecordDecl *d);

    void VisitFieldDecl(clang::FieldDecl *d);

    void VisitEnumDecl(clang::EnumDecl *d);

    void VisitFunctionDecl(clang::FunctionDecl *d);

    void VisitVarDecl(clang::VarDecl *d);
};