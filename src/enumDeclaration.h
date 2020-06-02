#ifndef _ENUMDECLARATION_H_
#define _ENUMDECLARATION_H_

#include "ir/ir.h"


namespace CODEGEN {

class enumDeclaration {
  public:
    const char *types[0] = {};

    enumDeclaration() {}
    ~enumDeclaration() {
    }

    static IR::Type *gen(int type);
};

} // namespace CODEGEN

#endif
