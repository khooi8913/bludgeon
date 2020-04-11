#ifndef _TYPEDECLARATION_H_
#define _TYPEDECLARATION_H_

#include "ir/ir.h"

namespace CODEGEN {

class typeDeclaration {
  public:
    const char *types[5] = {"derivedTypeDeclaration", "typedefDeclaration",
                            "parserTypeDeclaration", "controlTypeDeclaration",
                            "packageTypeDeclaration"};

    typeDeclaration() {}

    static IR::Type_Declaration *gen();
};

} // namespace CODEGEN

#endif
