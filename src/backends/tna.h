#ifndef _TNA_H_
#define _TNA_H_

#include "ir/ir.h"

namespace CODEGEN {

class TNA {

  public:
    TNA() {}
    ~TNA() {}

    static void generate_includes(std::ostream *ostream);
    static IR::P4Program *gen();

  private:
    static IR::P4Parser *gen_p();
    static IR::Declaration_Instance *gen_main();
    static IR::P4Parser *gen_switch_ingress_parser();
    static IR::P4Control *gen_switch_ingress();
    static IR::MethodCallStatement *gen_deparser_emit_call();
    static IR::P4Control *gen_switch_ingress_deparser();
    static IR::P4Parser *gen_switch_egress_parser();
    static IR::P4Control *gen_switch_egress_deparser();
    static IR::P4Control *gen_switch_egress();
    static IR::Declaration_Instance *gen_package_decl();
    static void gen_tf_md_t();
    static IR::Type_Struct *gen_ingress_metadata_t();
    static IR::Type_Struct *gen_egress_metadata_t();
};
} // namespace CODEGEN

#endif // ifndef _TNA_H_
