#include "common.h"

#include "actionDeclaration.h"
#include "actionList.h"
#include "controlDeclaration.h"
#include "enumDeclaration.h"
#include "headerStackType.h"
#include "headerTypeDeclaration.h"
#include "headerUnionDeclaration.h"
#include "ir/ir.h"
#include "structTypeDeclaration.h"
#include "tableDeclaration.h"
#include "typedefDeclaration.h"
#include <cstdlib>
#include <cstring>
#include <string>

const std::vector<cstring> str_keywords = {"if", "else", "key", "actions"};

namespace CODEGEN {
cstring randstr(size_t len) {
    cstring ret;
    std::stringstream ss;

    while (1) {
        ss.str("");
        for (size_t i = 0; i < len; i++) {
            ss << alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        ret = ss.str();
        if (std::find(str_keywords.begin(), str_keywords.end(), ret) !=
            str_keywords.end()) {
            continue;
        }

        if (P4Scope::used_names.find(ret) == P4Scope::used_names.end()) {
            P4Scope::used_names.insert(ret);
            break;
        }
    }

    return ret;
}

int randind(std::vector<int> &percent) {

    int sum = 0;
    for (auto i : percent) {
        sum += i;
    }

    int rand_num = rand() % sum;
    int ret = 0;

    int ret_sum = 0;
    for (auto i : percent) {
        ret_sum += i;
        if (ret_sum > rand_num) {
            break;
        }
        ret = ret + 1;
    }

    return ret;
}
} // namespace CODEGEN
