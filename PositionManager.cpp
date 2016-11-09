#include "PositionManager.h"

PositionManager::PositionManager(const clang::SourceManager &m) : m(m) { }

Position PositionManager::GetPosition(const clang::Decl *d) {
    auto sr = d->getSourceRange();
    return getPosition(sr);
}

Position PositionManager::GetPosition(const clang::Stmt *s) {
    auto sr = s->getSourceRange();
    return getPosition(sr);
}

Position::LC PositionManager::FromLocation(clang::SourceLocation loc) {
    return Position::LC(loc, m);
}

Position PositionManager::getPosition(clang::SourceRange sr) {
    Position::LC begin(sr.getBegin(), m);
    Position::LC end(sr.getEnd(), m);
    std::string filename = m.getFilename(sr.getBegin());

    return Position(begin, end, filename);
}




