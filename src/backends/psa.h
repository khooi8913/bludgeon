#ifndef _PSA_H_
#define _PSA_H_

#include "ir/ir.h"

namespace CODEGEN {

class PSA {

  public:
    PSA() {}
    ~PSA() {}

    static void generate_includes(std::ostream *ostream);
    static IR::P4Program *gen();

  private:
    static void set_probabilities();
    static IR::P4Parser *gen_p();
    static IR::P4Parser *gen_switch_ingress_parser();
    static IR::P4Control *gen_switch_ingress();
    static IR::MethodCallStatement *gen_deparser_emit_call();
    static IR::P4Control *gen_switch_ingress_deparser();
    static IR::P4Parser *gen_switch_egress_parser();
    static IR::P4Control *gen_switch_egress();
    static IR::P4Control *gen_switch_egress_deparser();
    static IR::Declaration_Instance *gen_ingress_pipe_decl();
    static IR::Declaration_Instance *gen_egress_pipe_decl();
    static IR::Declaration_Instance *gen_main();
    static IR::Type_Struct *gen_metadata_t();
    static IR::Type_Struct *gen_empty_t();
    static void gen_psa_md_t();
};
} // namespace CODEGEN

#endif // ifndef _PSA_H_
