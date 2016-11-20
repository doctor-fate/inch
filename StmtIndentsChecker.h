#pragma once

#include <clang/AST/StmtVisitor.h>
#include "BaseChecker.h"
#include "DeclIndentsChecker.h"

class StmtIndentsChecker : public BaseChecker, public clang::StmtVisitor<StmtIndentsChecker> {
public:
    StmtIndentsChecker(const PositionManager &m, unsigned int inc);

    void VisitIfStmt(clang::IfStmt *s);

    void VisitWhileStmt(clang::WhileStmt *s);

    void VisitForStmt(clang::ForStmt *s);

    void VisitDoStmt(clang::DoStmt *s);

    void VisitSwitchStmt(clang::SwitchStmt *s);

    void VisitCallExpr(clang::CallExpr *e);

    void VisitLabelStmt(clang::LabelStmt *s);

    friend class DeclIndentsChecker;

private:
    void checkSwitchBody(clang::SwitchStmt *s, clang::CompoundStmt *cs);

    unsigned int visitSwitchCaseStmt(clang::SwitchCase *s, unsigned int cinc);

    void visitCompoundStmt(clang::CompoundStmt *s, Position pp);
};