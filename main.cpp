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

void replace(std::stringstream &ss, const std::string &line) {
    bool rm = true;
    unsigned int n = 0;
    for (auto c = line.begin(); c != line.end(); c++) {
        if (*c == '\t' && rm) {
            ss << std::string(8 - n, ' ');
        } else {
            if (*c == ' ') {
                n = (n + 1) % 8;
            }
            rm = rm && isspace(*c);
            ss << *c;
        }
    }
    ss << std::endl;
}

std::string processFile(std::ifstream &in) {
    std::stringstream ss;
    char *buf = new char[512];
    while (in.getline(buf, 512)) {
        std::string line(buf);
        replace(ss, line);
    }
    delete[] buf;

    return ss.str();
}

std::shared_ptr<Position> check(const std::string &code) {
    auto ep = std::make_shared<Position>();
    auto ast = clang::tooling::buildASTFromCodeWithArgs(code, {
        "-I", "/usr/local/lib/clang/3.9.0/include",
        "-x", "c", "-std=c11"
    });
    if (ast == nullptr) {
        fprintf(stderr, "ast is null. probably bad source.");
        return ep;
    }
    clang::ASTContext &context = ast->getASTContext();

    DeclIndentsChecker icv(context, 0);
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
        return 1;
    }

    const char *filename = argv[1];
    std::ifstream in;
    in.open(filename);
    if (in.bad()) {
        fprintf(stderr, "can not open source file.");
        return 1;
    }

    std::string source = processFile(in);
    auto p = check(source);
    if (p != nullptr) {
        fprintf(stdout, "%d:%d--%d:%d", p->Begin.Line, p->Begin.Column, p->End.Line, p->End.Column);
    }

    in.close();
    return 0;
}