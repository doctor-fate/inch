#include <iostream>
#include <fstream>
#include <sstream>

#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>
#include <clang/AST/DeclVisitor.h>
#include <clang/AST/StmtVisitor.h>
#include "Position.h"
#include "DeclIndentsChecker.h"
#include "DeclIndentDefiner.h"

size_t countTabs(const std::string &s) {
    size_t n = 0;
    for (auto c = s.begin(); c != s.end(); n++, c++) {
        if (*c != '\t') {
            break;
        }
    }

    return n;
}

std::string processFile(std::ifstream &in) {
    std::stringstream ss;
    char *buf = new char[512];
    while (in.getline(buf, 512)) {
        std::string line(buf);
        size_t n = countTabs(line);
        line.replace(0, n, n * 4, ' ');
        ss << line << std::endl;
    }
    delete[] buf;

    return ss.str();
}

std::shared_ptr<Position> check(const std::string &code) {
    auto ep = std::make_shared<Position>();
    auto ast = clang::tooling::buildASTFromCodeWithArgs(code, { "-x", "c", "-std=c11" });
    if (ast == nullptr) {
        fprintf(stderr, "ast is null. probably bad source.");
        return ep;
    }
    clang::ASTContext &context = ast->getASTContext();
    DeclIndentDefiner idv(context);
    int indent = idv.Indent(context.getTranslationUnitDecl());
    if (indent <= 0) {
        indent = 4;
    }

    DeclIndentsChecker icv(context, 0, (unsigned int) indent);
    try {
        icv.Visit(context.getTranslationUnitDecl());
    } catch (Position &p) {
        return std::make_shared<Position>(p);
    } catch (const std::exception &e) {
        fprintf(stderr, "%s", e.what());
        return ep;
    }

    return nullptr;
}

int main(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "specify source file.");
        return 0;
    }

    const char *filename = argv[1];
    std::ifstream in;
    in.open(filename);
    if (in.bad()) {
        fprintf(stderr, "can not open source file.");
        return 0;
    }

    std::string source = processFile(in);
    auto p = check(source);
    if (p != nullptr) {
        fprintf(stdout, "%d:%d--%d:%d", p->Begin.Line, p->Begin.Column, p->End.Line, p->End.Column);
    }

    in.close();
    return 0;
}