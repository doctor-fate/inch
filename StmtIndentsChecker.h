#pragma once

#include <clang/AST/StmtVisitor.h>
#include "BaseChecker.h"

class StmtIndentsChecker : public BaseChecker, public clang::StmtVisitor<StmtIndentsChecker> {
public:
    StmtIndentsChecker(const PositionManager &m, unsigned int inc, unsigned int ins);

    void VisitCompoundStmt(clang::CompoundStmt *s);

    void VisitIfStmt(clang::IfStmt *s);

    void VisitWhileStmt(clang::WhileStmt *s);

    void VisitForStmt(clang::ForStmt *s);

    void VisitDoStmt(clang::DoStmt *s);

    void VisitSwitchStmt(clang::SwitchStmt *s);

    void VisitCallExpr(clang::CallExpr *e);
private:
    void checkSwitchBody(clang::SwitchStmt *s, clang::CompoundStmt *cs);

    void visitSwitchCaseStmt(clang::SwitchCase *s, unsigned int cinc);
};