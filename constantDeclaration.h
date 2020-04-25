#ifndef _CONSTANTDECLARATION_H_
#define _CONSTANTDECLARATION_H_

#include "ir/ir.h"

namespace CODEGEN {
class constantDeclaration {
  public:
    const char *types[0] = {};

    constantDeclaration() {}

    ~constantDeclaration() {}

    static IR::Declaration_Constant *gen();
};
} // namespace CODEGEN

#endif
