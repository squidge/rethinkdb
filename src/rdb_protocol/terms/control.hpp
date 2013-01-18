#include "rdb_protocol/op.hpp"
#include "rdb_protocol/err.hpp"

namespace ql {
class all_term_t : public op_term_t {
public:
    all_term_t(env_t *env, const Term2 *term) : op_term_t(env, term, argspec_t(1,-1)) { }
private:
    virtual val_t *eval_impl() {
        for (size_t i = 0; i < num_args(); ++i) {
            env_checkpoint_t ect(env, &env_t::discard_checkpoint);
            val_t *v = arg(i);
            if (!v->as_datum()->as_bool()) {
                ect.reset(&env_t::merge_checkpoint);
                return v;
            } else if (i == num_args()-1) {
                ect.reset(&env_t::merge_checkpoint);
                return v;
            }
        }
        unreachable();
    }
    RDB_NAME("all")
};

class any_term_t : public op_term_t {
public:
    any_term_t(env_t *env, const Term2 *term) : op_term_t(env, term, argspec_t(1,-1)) { }
private:
    virtual val_t *eval_impl() {
        for (size_t i = 0; i < num_args(); ++i) {
            env_checkpoint_t ect(env, &env_t::discard_checkpoint);
            val_t *v = arg(i);
            if (v->as_datum()->as_bool()) {
                ect.reset(&env_t::merge_checkpoint);
                return v;
            }
        }
        return new_val(new datum_t(false, false));
        unreachable();
    }
    RDB_NAME("any")
};

class branch_term_t : public op_term_t {
public:
    branch_term_t(env_t *env, const Term2 *term) : op_term_t(env, term, argspec_t(3)) { }
private:
    virtual val_t *eval_impl() {
        bool b; {
            env_checkpoint_t ect(env, &env_t::discard_checkpoint);
            b = arg(0)->as_datum()->as_bool();
        }
        return b ? arg(1) : arg(2);
    }
    RDB_NAME("branch")
};


class funcall_term_t : public op_term_t {
public:
    funcall_term_t(env_t *env, const Term2 *term)
        : op_term_t(env, term, argspec_t(1,-1)) { }
private:
    virtual val_t *eval_impl() {
        func_t *f = arg(0)->as_func();
        std::vector<const datum_t *> args;
        for (size_t i = 1; i < num_args(); ++i) args.push_back(arg(i)->as_datum());
        return f->_call(args);
    }
    RDB_NAME("funcall")
};

} // namespace ql