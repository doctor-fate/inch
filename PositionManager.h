#pragma once

#include <clang/Basic/SourceManager.h>
#include <clang/AST/Decl.h>
#include "Position.h"

class PositionManager {
public:
    PositionManager(const clang::SourceManager &m);

    Position GetPosition(const clang::Decl *d);

    Position GetPosition(const clang::Stmt *s);

    Position::LC FromLocation(clang::SourceLocation loc);

private:
    Position getPosition(clang::SourceRange sr);

    const clang::SourceManager &m;
};
