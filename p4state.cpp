#include "p4state.h"

namespace CODEGEN {
IR::MethodCallStatement *gen_methodcall_stat(
    cstring field_name, std::vector<cstring> hdr_fields_names,
    std::map<const cstring, const IR::Type *> hdr_fields_types) {
    auto pkt_call = new IR::Member(new IR::PathExpression("pkt"), "extract");

    IR::Vector<IR::Argument> *args = new IR::Vector<IR::Argument>();

    auto field_type = hdr_fields_types[field_name];

    auto mem = new IR::Member(new IR::PathExpression("hdr"), field_name);

    if (field_type->is<IR::Type_Stack>()) {
        IR::Argument *arg;
        auto tp_stack = field_type->to<IR::Type_Stack>();
        auto size = tp_stack->size->to<IR::Constant>()->value;
        auto arr_index =
            new IR::ArrayIndex(mem, new IR::Constant(rand() % size));

        if (tp_stack->elementType->is<IR::Type_Name>()) {
            auto tp_name = tp_stack->elementType->to<IR::Type_Name>();
            auto real_tp = P4Scope::get_type_by_name(tp_name->path->name.name);
            if (real_tp->is<IR::Type_HeaderUnion>()) {
                auto tp_hdr_union = real_tp->to<IR::Type_HeaderUnion>();
                auto rand_ind = rand() % (tp_hdr_union->fields.size());
                auto sf = tp_hdr_union->fields.at(rand_ind);
                arg = new IR::Argument(
                    new IR::Member(arr_index, IR::ID(sf->name.name)));
            } else {
                arg = new IR::Argument(arr_index);
            }
        } else {
            arg = new IR::Argument(arr_index);
        }

        args->push_back(arg);
    } else {
        IR::Argument *arg;
        auto field_ind = std::find(hdr_fields_names.begin(),
                                   hdr_fields_names.end(), field_name);
        hdr_fields_names.erase(field_ind);

        if (field_type->is<IR::Type_Name>()) {
            auto tp_name = field_type->to<IR::Type_Name>();
            auto real_tp = P4Scope::get_type_by_name(tp_name->path->name.name);
            if (real_tp->is<IR::Type_HeaderUnion>()) {
                auto tp_hdr_union = real_tp->to<IR::Type_HeaderUnion>();
                auto rand_ind = rand() % (tp_hdr_union->fields.size());
                auto sf = tp_hdr_union->fields.at(rand_ind);
                arg = new IR::Argument(
                    new IR::Member(mem, IR::ID(sf->name.name)));
            } else {
                arg = new IR::Argument(mem);
            }
        } else {
            arg = new IR::Argument(mem);
        }
        args->push_back(arg);
    }

    auto mce = new IR::MethodCallExpression(pkt_call, args);
    return new IR::MethodCallStatement(mce);
}

IR::MethodCallStatement *p4State::gen_hdr_extract(IR::Member *pkt_call,
                                                  IR::Expression *mem) {
    IR::Vector<IR::Argument> *args = new IR::Vector<IR::Argument>();
    IR::Argument *arg = new IR::Argument(mem);

    args->push_back(arg);
    auto mce = new IR::MethodCallExpression(pkt_call, args);
    return new IR::MethodCallStatement(mce);
}

void p4State::gen_hdr_union_extract(
    IR::IndexedVector<IR::StatOrDecl> &components,
    const IR::Type_HeaderUnion *hdru, IR::ArrayIndex *arr_ind,
    IR::Member *pkt_call) {
    auto sf = hdru->fields.at(0);
    // for (auto sf : hdru->fields) {
    // auto mem = new IR::Member(arr_ind,
    // sf->type->to<IR::Type_Name>()->path->name);
    auto mem = new IR::Member(arr_ind, sf->name);

    components.push_back(gen_hdr_extract(pkt_call, mem));
    // }
}

IR::ParserState *p4State::gen_start_state() {
    IR::IndexedVector<IR::StatOrDecl> components;
    IR::Expression *transition = new IR::PathExpression("parse_hdrs");
    auto ret = new IR::ParserState("start", components, transition);

    P4Scope::add_to_scope(ret);
    return ret;
}

IR::ParserState *p4State::gen_hdr_states() {
    IR::Expression *transition;
    IR::IndexedVector<IR::StatOrDecl> components;
    std::vector<cstring> hdr_fields_names;
    std::map<const cstring, const IR::Type *> hdr_fields_types;

    for (auto sf : P4Scope::sys_hdr->fields) {
        hdr_fields_names.push_back(sf->name.name);
        hdr_fields_types.emplace(sf->name.name, sf->type);
    }

    for (size_t i = 0; i < hdr_fields_names.size(); i++) {
        auto pkt_call =
            new IR::Member(new IR::PathExpression("pkt"), "extract");

        auto sf_name = hdr_fields_names.at(i);
        auto sf_type = hdr_fields_types[sf_name];
        auto mem = new IR::Member(new IR::PathExpression("hdr"), sf_name);
        if (auto sf_tp_s = sf_type->to<IR::Type_Stack>()) {
            auto size = sf_tp_s->getSize();
            auto ele_tp_name = sf_tp_s->elementType;
            auto ele_tp = P4Scope::get_type_by_name(
                ele_tp_name->to<IR::Type_Name>()->path->name.name);
            if (ele_tp->is<IR::Type_Header>()) {
                for (auto j = 0; j < size; j++) {
                    auto next_mem = new IR::Member(mem, "next");
                    components.push_back(gen_hdr_extract(pkt_call, next_mem));
                }
            } else if (auto hdru_tp = ele_tp->to<IR::Type_HeaderUnion>()) {
                for (auto j = 0; j < size; j++) {
                    auto arr_ind = new IR::ArrayIndex(mem, new IR::Constant(j));
                    gen_hdr_union_extract(components, hdru_tp, arr_ind,
                                          pkt_call);
                }
            } else {
                BUG("wtf here %s", sf_type->node_type_name());
            }
        } else if (sf_type->is<IR::Type_Name>()) {
            auto hdr_field_tp = P4Scope::get_type_by_name(
                sf_type->to<IR::Type_Name>()->path->name.name);
            if (hdr_field_tp->is<IR::Type_HeaderUnion>()) {
                auto hdru_tp = hdr_field_tp->to<IR::Type_HeaderUnion>();
                auto sf = hdru_tp->fields.at(0);
                auto hdr_mem = new IR::Member(mem, sf->name);
                components.push_back(gen_hdr_extract(pkt_call, hdr_mem));
            } else {
                components.push_back(gen_hdr_extract(pkt_call, mem));
            }
        } else {
            BUG("wtf here %s", sf_type->node_type_name());
        }
    }

    // transition part
    // transition = new IR::PathExpression(new IR::Path(IR::ID("state_0")));
    transition = new IR::PathExpression("accept");

    auto ret = new IR::ParserState("parse_hdrs", components, transition);
    P4Scope::add_to_scope(ret);
    return ret;
}

IR::ParserState *gen_state(cstring state_name) {
    IR::ID name(state_name);
    IR::IndexedVector<IR::StatOrDecl> components;
    std::vector<cstring> states = {
        "state_0", "state_1", "state_2", "state_3", "state_4",
    };

    P4Scope::start_local_scope();

    // variable decls
    for (int i = 0; i < 5; i++) {
        auto var_decl = variableDeclaration::gen();
        components.push_back(var_decl);
    }
    // statements
    for (int i = 0; i < 5; i++) {
        auto ass = assignmentOrMethodCallStatement::gen_assign();
        if (ass != nullptr) {
            components.push_back(ass);
        }
        break;
    }

    // expression
    IR::Expression *transition;
    IR::Vector<IR::Expression> exprs;
    IR::Vector<IR::SelectCase> cases;
    std::vector<const IR::Type *> types;

    switch (rand() % 3) {
    case 0: {
        transition = new IR::PathExpression("accept");
        break;
    }
    case 1: {
        transition = new IR::PathExpression("reject");
        break;
    }
    case 2: {
        transition = new IR::PathExpression(states.at(rand() % states.size()));
        break;
    }
    case 3: {
        size_t num = rand() % 3 + 1;
        auto get_le_flag = false;
        // auto get_le_flag = expression::get_list_expressions(exprs,
        // types,
        // num);
        if (get_le_flag == true) {
            for (int i = 0; i < rand() % 3 + 1; i++) {
                IR::Expression *keyset;
                IR::Vector<IR::Expression> args;
                for (size_t i = 0; i < types.size(); i++) {
                    auto tp = types.at(i)->to<IR::Type_Bits>();
                    int size = tp->size > 4 ? 4 : tp->size;
                    int val = rand() % (1 << size);
                    args.push_back(new IR::Constant(val));
                }
                keyset = new IR::ListExpression(args);
                auto states_at_rnd = states.at(rand() % states.size());
                auto sw_case = new IR::SelectCase(
                    keyset, new IR::PathExpression(states_at_rnd));
                cases.push_back(sw_case);
            }
            if (rand() % 2 == 0) {
                cases.push_back(
                    new IR::SelectCase(new IR::DefaultExpression,
                                       new IR::PathExpression("accept")));
            } else {
                cases.push_back(
                    new IR::SelectCase(new IR::DefaultExpression,
                                       new IR::PathExpression("reject")));
            }

            transition =
                new IR::SelectExpression(new IR::ListExpression(exprs), cases);
        } else {
            transition =
                new IR::PathExpression(states.at(rand() % states.size()));
        }
        break;
    }
    }

    P4Scope::end_local_scope();
    // add to scope
    auto ret = new IR::ParserState(name, components, transition);
    P4Scope::add_to_scope(ret);
    return ret;
}
} // namespace CODEGEN
