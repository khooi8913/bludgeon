#ifndef _CONTROLTYPEDECLARATION_H_
#define _CONTROLTYPEDECLARATION_H_

#include "ir/ir.h"

#include "parameterList.h"

#include "scope.h"

namespace CODEGEN {
class controlTypeDeclaration {
  public:
    const char *types[0] = {};

    controlTypeDeclaration() {}

    ~controlTypeDeclaration() {}

};
} // namespace CODEGEN

#endif
