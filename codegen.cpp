#include "codegen.h"


#include "scope.h"
#include "headerTypeDeclaration.h"
#include "headerStackType.h"
#include "headerUnionDeclaration.h"
#include "structTypeDeclaration.h"
#include "enumDeclaration.h"
#include "actionDeclaration.h"
#include "typedefDeclaration.h"
#include "controlDeclaration.h"
#include "tableDeclaration.h"
#include "actionList.h"
#include "functionDeclaration.h"
#include "p4parser.h"


namespace CODEGEN {


IR::Node* CGenerator::gen() {
	IR::Node* n = nullptr;
 	while (1) {
		switch (rand()%2) {
			case 0: {
						n = headerTypeDeclaration::gen();
						break;
					}
			case 1: {
						if (P4Scope::get_num_type_header() > 0) {
							n = headerUnionDeclaration::gen(); 
						}
						break;
					}
		}

		if (n != nullptr) {
			break;
		}
	}

	return n;
}

IR::Node* CGenerator::gen_act() {
	IR::Node* n = nullptr;
	while (1) {
		auto act_decl = new actionDeclaration();
		n = act_decl->gen();
		if (n != nullptr) {
			break;
		}
	}

	return n;
}

IR::Node* CGenerator::gen_struct() {
	IR::Node* n = nullptr;
	while (1) {
		n = structTypeDeclaration::gen();
		if (n != nullptr) {
			break;
		}
	}
	return n;
}

IR::Node* CGenerator::gen_t_enum() {
	IR::Node* n = nullptr;
	while (1) {
		// auto t_enum = new enumDeclaration(rand()%2);
		// Tao: always generating SerEnum
		auto t_enum = new enumDeclaration(1);
		n = t_enum->gen();
		if (n != nullptr) {
			break;
		}
	}
	return n;
}

IR::Node* CGenerator::gen_tpdef() {
	IR::Node* n = nullptr;
	while (1) {
		auto tpdef = new typedefDeclaration();
		n = tpdef->gen();
		if (n != nullptr) {
			break;
		}
	}
	return n;
}

IR::Node* CGenerator::gen_ctrldef() {
	IR::Node* n = nullptr;
	while (1) {
		auto ctrldef = new controlDeclaration();
		n = ctrldef->gen();
		if (n != nullptr) {
			break;
		}
	}
	return n;
}

IR::Node* CGenerator::gen_actlist() {
	auto names = P4Scope::get_name_nodir_p4acts();
	return nullptr;
}

IR::Node* CGenerator::gen_tab() {
    auto tab_gen = new tableDeclaration();
    return tab_gen->gen();
}

IR::Node* CGenerator::gen_func() {
	auto func_gen = new functionDeclaration();
	return func_gen->gen();
}

IR::Node* CGenerator::gen_sys_parser() {
    auto p_gen = new p4Parser();
    return p_gen->gen_sys_p();
}

void CGenerator::emitBmv2Top(std::ostream *ostream) {
	*ostream << "#include <core.p4>\n";
	*ostream << "#include <v1model.p4>\n\n";
}

void CGenerator::emitBmv2Bottom(std::ostream *ostream) {
    // *ostream << "parser p(packet_in b, out Headers h, inout Meta m, inout standard_metadata_t sm) {\n"
    //     "state start {"
    //     "transition accept;}}\n\n";

    *ostream << "control vrfy(inout Headers h, inout Meta m) { apply {} }\n\n";
    *ostream << "control update(inout Headers h, inout Meta m) { apply {} }\n\n";
    *ostream << "control egress(inout Headers h, inout Meta m, inout standard_metadata_t sm) { apply {} }\n\n";
    *ostream << "control deparser(packet_out b, in Headers h) { apply {} }\n\n";
    // *ostream << "control ingress(inout Headers h, inout Meta m, inout standard_metadata_t sm) { apply {} }\n\n";

    *ostream << "V1Switch(p(), vrfy(), ingress(), egress(), update(), deparser()) main;\n\n";
}

void CGenerator::emitTFTop(std::ostream *ostream) {
    *ostream << "#include <core.p4>\n";
    *ostream << "#include <tna.p4>\n\n";
}

void CGenerator::emitTFBottom(std::ostream *ostream) {
    *ostream << "parser SwitchIngressParser(\n"
            "packet_in pkt,\n"
            "out Headers hdr,\n"
            "out Meta ig_md,\n"
            "out ingress_intrinsic_metadata_t ig_intr_md) {\n"
        "state start {\n"
            "pkt.extract(ig_intr_md);\n"
            "transition select(ig_intr_md.resubmit_flag) {\n"
                    "1 : parse_resubmit;\n"
                    "0 : parse_port_metadata;\n"
            "}\n"
        "}\n"
        "state parse_resubmit {\n"
            "transition reject;\n"
        "}\n"
        "state parse_port_metadata {\n"
            "pkt.advance(PORT_METADATA_SIZE);\n"
            "transition accept;\n"
        "}\n"
    "}\n"
    "control SwitchIngressDeparser(\n"
            "packet_out pkt,\n"
            "inout Headers hdr,\n"
            "in Meta ig_md,\n"
            "in ingress_intrinsic_metadata_for_deparser_t ig_dprsr_md) {\n"
        "apply {\n"
        "}\n"
    "}\n";
    *ostream << "parser SwitchEgressParser(\n"
            "packet_in pkt,\n"
            "out Headers hdr,\n"
            "out Meta eg_md,\n"
            "out egress_intrinsic_metadata_t eg_intr_md) {\n"
        "state start {\n"
            "pkt.extract(eg_intr_md);\n"
            "transition accept;\n"
        "}\n"
    "}\n"
    "control SwitchEgressDeparser(\n"
            "packet_out pkt,\n"
            "inout Headers hdr,\n"
            "in Meta eg_md,\n"
            "in egress_intrinsic_metadata_for_deparser_t eg_intr_dprs_md) {\n"
        "apply {\n"
        "}\n"
    "}\n"
    "control SwitchEgress(\n"
            "inout Headers hdr,\n"
            "inout Meta eg_md,\n"
            "in egress_intrinsic_metadata_t eg_intr_md,\n"
            "in egress_intrinsic_metadata_from_parser_t eg_intr_md_from_prsr,\n"
            "inout egress_intrinsic_metadata_for_deparser_t eg_intr_dprs_md,\n"
            "inout egress_intrinsic_metadata_for_output_port_t eg_intr_oport_md) {\n"
        "apply {}\n"
    "}\n";

    *ostream << "Pipeline(SwitchIngressParser(),\n"
                 "ingress(),\n"
                 "SwitchIngressDeparser(),\n"
                 "SwitchEgressParser(),\n"
                 "SwitchEgress(),\n"
                 "SwitchEgressDeparser()) pipe;\n\n";

    *ostream << "Switch(pipe) main;\n";
}

void CGenerator::gen_p4_code() {
	auto objects = new IR::Vector<IR::Node>();

    objects->push_back(CODEGEN::headerTypeDeclaration::gen_eth());
	objects->push_back(gen()); // generate hearder or header union
	objects->push_back(gen());
	objects->push_back(gen_struct()); // generate struct
	objects->push_back(gen_struct());
	objects->push_back(CODEGEN::structTypeDeclaration::gen_Headers()); // generate struct Headers
	objects->push_back(CODEGEN::structTypeDeclaration::gen_Meta()); // generate struct Meta
    if (flag == 0) {
        CGenerator::emitBmv2Top(ostream);
	    CODEGEN::structTypeDeclaration::gen_Sm(); // generate struct standard_metadata_t
	    // objects->push_back(>gen_ctrldef());
	    objects->push_back(gen_func());
        objects->push_back(gen_sys_parser());
	    objects->push_back(CODEGEN::controlDeclaration::gen_ing_ctrl());
    }
    else if (flag == 1) {
        CGenerator::emitTFTop(ostream);
        CODEGEN::structTypeDeclaration::gen_tf_md_t(); // generate tofino metadatas
        // 
        objects->push_back(gen_func());
        objects->push_back(CODEGEN::controlDeclaration::gen_tf_ing_ctrl());
    }
    else {
        BUG("flag must be 0 or 1");
    }
	IR::P4Program *program = new IR::P4Program(*objects);

	// output to the file
    CODEGEN::SubToP4 top4(ostream, false);
	program->apply(top4);
    if (flag == 0) {
        CGenerator::emitBmv2Bottom(ostream);
    }
    else if (flag == 1) {
        CGenerator::emitTFBottom(ostream);
    }
    else {
        BUG("flag must be 0 or 1");
    }

    ostream->flush();
}


} // namespace CODEGEN
