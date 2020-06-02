#ifndef _TYPEREF_H_
#define _TYPEREF_H_

#include "ir/ir.h"

#include "baseType.h"
#include "headerStackType.h"
#include "typeName.h"

#include "common.h"

namespace CODEGEN {
class typeRef {
  public:
    const char *types[5] = {
        "baseType", "typeName",
        "specializedType", // not
        "headerStackType",
        "tupleType" // not
    };


    typeRef() {}

};
} // namespace CODEGEN

#endif
