#include "statement.h"

#include "assignmentOrMethodCallStatement.h"
#include "blockStatement.h"
#include "conditionalStatement.h"
#include "exitStatement.h"
#include "returnStatement.h"
#include "switchStatement.h"

namespace CODEGEN {
IR::Statement *statement::gen_rnd(bool is_in_func = false) {
    std::vector<int64_t> percent = {5, 70, 3, 5, 5, 2};
    IR::Statement *stmt = nullptr;
    bool use_default_stmt = false;
    switch (randind(percent)) {
    case 0: {
        stmt = switchStatement::gen();
        if (not stmt) {
            use_default_stmt = true;
        }
        break;
    }
    case 1: {
        use_default_stmt = true;
        break;
    }
    case 2: {
        stmt = conditionalStatement::gen_if_stat(is_in_func);
        break;
    }
    case 3: {
        stmt = returnStatement::gen_ret_stat(P4Scope::ret_type);
        break;
    }
    case 4: {
        if (is_in_func) {
            use_default_stmt = true;
        } else{
            stmt = exitStatement::gen();
        }
        break;
    }
    case 5: {
        stmt = blockStatement::gen(is_in_func);
        break;
    }
    }
    if (use_default_stmt) {
        stmt = assignmentOrMethodCallStatement::gen(is_in_func);
    }
    return stmt;
}
} // namespace CODEGEN
