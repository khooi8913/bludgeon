#ifndef _VARIABLEDECLARATION_H_
#define _VARIABLEDECLARATION_H_

#include "ir/ir.h"


#include "common.h"
#include "codegen.h"
#include "scope.h"
#include "expression.h"
#include "expression_2.h"


#include "typeRef.h"

namespace CODEGEN {
class variableDeclaration {
public:
    const char *types[0] = {
    };

    variableDeclaration() {
    }

    ~variableDeclaration() {
    }

    static IR::Declaration_Variable *gen();
};
} // namespace CODEGEN



#endif
