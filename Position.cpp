#include <stdlib.h>
#include "Position.h"

std::ostream &operator<<(std::ostream &os, const Position::LC &loc) {
    return os << "{\"Line\":" << loc.Line << ",\"Column\":" << loc.Column << "}";
}

std::ostream &operator<<(std::ostream &os, const Position &p) {
    return os << "{\"Begin\":" << p.Begin << ",\"End\":" << p.End << ",\"Filename\":\"" << p.Filename << "\"}";
}

Position::Position() { }

Position::LC::LC(clang::SourceLocation loc, const clang::SourceManager &m) {
    Line = m.getExpansionLineNumber(loc);
    Column = m.getExpansionColumnNumber(loc);
}

Position::LC::LC() : Line(0), Column(0) { }

Position::Position(Position::LC begin, Position::LC end, std::string filename) :
    Begin(begin), End(end), Filename(filename) { }

void Position::Throw(const Position &p) {
    throw p;
}

bool Position::CheckBeginColumn(unsigned int indent) {
    return Begin.Column == indent + 1;
}

void Position::CheckBeginColumnThrow(unsigned int indent) {
    if (!CheckBeginColumn(indent)) {
        Throw(*this);
    }
}

bool Position::CheckEndColumn(unsigned int indent) {
    return End.Column == indent + 1;
}

void Position::CheckEndColumnThrow(unsigned int indent) {
    if (!CheckEndColumn(indent)) {
        Throw(*this);
    }
}
