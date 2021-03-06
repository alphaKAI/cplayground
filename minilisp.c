#include "cplayground.h"
#include "sds/sds.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int op_width_table[] = {
    [OpPop] = 1,         [OpPush] = 2,     [OpAllocLvars] = 2,
    [OpFreeLvars] = 1,   [OpGetLocal] = 2, [OpSetLocal] = 2,
    [OpSetArgLocal] = 2, [OpAdd] = 1,      [OpSub] = 1,
    [OpMul] = 1,         [OpDiv] = 1,      [OpMod] = 1,
    [OpEq] = 1,          [OpNeq] = 1,      [OpLt] = 1,
    [OpLeq] = 1,         [OpGt] = 1,       [OpGeq] = 1,
    [OpPrint] = 1,       [OpPrintln] = 1,  [OpJumpRel] = 2,
    [OpFuncDef] = 2,     [OpCall] = 3,     [OpReturn] = 1,
    [OpVarDef] = 2,      [OpGetVar] = 2,   [OpSetVar] = 2,
    [OpBranch] = 2,      [OpMakeList] = 2, [OpSetArgFrom] = 3,
    [OpDumpEnv] = 1};

#define SpecifyPlaceAndSetNameItSelf(e) [e] = #e

static const char *op_to_string[] = {
    SpecifyPlaceAndSetNameItSelf(OpPop),
    SpecifyPlaceAndSetNameItSelf(OpPush),
    SpecifyPlaceAndSetNameItSelf(OpAllocLvars),
    SpecifyPlaceAndSetNameItSelf(OpFreeLvars),
    SpecifyPlaceAndSetNameItSelf(OpGetLocal),
    SpecifyPlaceAndSetNameItSelf(OpSetLocal),
    SpecifyPlaceAndSetNameItSelf(OpSetArgLocal),
    SpecifyPlaceAndSetNameItSelf(OpAdd),
    SpecifyPlaceAndSetNameItSelf(OpSub),
    SpecifyPlaceAndSetNameItSelf(OpMul),
    SpecifyPlaceAndSetNameItSelf(OpDiv),
    SpecifyPlaceAndSetNameItSelf(OpMod),
    SpecifyPlaceAndSetNameItSelf(OpEq),
    SpecifyPlaceAndSetNameItSelf(OpNeq),
    SpecifyPlaceAndSetNameItSelf(OpLt),
    SpecifyPlaceAndSetNameItSelf(OpLeq),
    SpecifyPlaceAndSetNameItSelf(OpGt),
    SpecifyPlaceAndSetNameItSelf(OpGeq),
    SpecifyPlaceAndSetNameItSelf(OpPrint),
    SpecifyPlaceAndSetNameItSelf(OpPrintln),
    SpecifyPlaceAndSetNameItSelf(OpJumpRel),
    SpecifyPlaceAndSetNameItSelf(OpFuncDef),
    SpecifyPlaceAndSetNameItSelf(OpCall),
    SpecifyPlaceAndSetNameItSelf(OpReturn),
    SpecifyPlaceAndSetNameItSelf(OpVarDef),
    SpecifyPlaceAndSetNameItSelf(OpGetVar),
    SpecifyPlaceAndSetNameItSelf(OpSetVar),
    SpecifyPlaceAndSetNameItSelf(OpBranch),
    SpecifyPlaceAndSetNameItSelf(OpMakeList),
    SpecifyPlaceAndSetNameItSelf(OpSetArgFrom),
    SpecifyPlaceAndSetNameItSelf(OpDumpEnv)};

VMValue *new_VMValue(int ty, void *e) {
  VMValue *vmvalue = xmalloc(sizeof(VMValue));
  vmvalue->ty = ty;

  switch (ty) {
  case VValue:
    vmvalue->val = e;
    break;
  case VFunc:
    vmvalue->func = e;
    break;
  default:
    fprintf(stderr, "Invalid type tag\n");
    exit(EXIT_FAILURE);
  }

  return vmvalue;
}

void free_VMValue(VMValue **v_ptr) {
  switch ((*v_ptr)->ty) {
  case VValue:
    free_SexpObject(&(*v_ptr)->val);
    break;
  case VFunc:
    free_VMFunction(&(*v_ptr)->func);
    break;
  default:
    fprintf(stderr, "Invalid type tag\n");
    exit(EXIT_FAILURE);
  }
  xfree(v_ptr);
}

VMValue *dup_VMValue(VMValue *value) {
  switch (value->ty) {
  case VValue:
    return new_VMValue(VValue, dup_SexpObject(value->val));
    break;
  case VFunc:
    return new_VMValue(VFunc, value->func);
  default:
    fprintf(stderr, "Invalid type tag\n");
    exit(EXIT_FAILURE);
  }
}

VMValue *new_VMValueWithValue(SexpObject *obj) {
  return new_VMValue(VValue, obj);
}

VMValue *new_VMValueWithFunc(VMFunction *vmf) {
  return new_VMValue(VFunc, vmf);
}

sds show_VMValue(VMValue *val) {
  switch (val->ty) {
  case VValue: {
    switch (val->val->ty) {
    case float_ty:
    case bool_ty:
    case symbol_ty:
    case string_ty:
      return sdscatprintf(sdsempty(), "%s",
                          show_sexp_object_impl(val->val, false));
    case list_ty: {
      sds ret = sdsnew("(");
      Vector *v = val->val->list_val;

      for (size_t i = 0; i < v->len; i++) {
        if (i == 0) {
          ret = sdscatprintf(ret, "%s", show_VMValue(v->data[i]));
        } else {
          ret = sdscatprintf(ret, " %s", show_VMValue(v->data[i]));
        }
      }

      ret = sdscatprintf(ret, ")");
      return ret;
    }
    case object_ty:
    case quote_ty:
      fprintf(stderr, "unimplemented!\n");
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Should not reach here\n");
    exit(EXIT_FAILURE);
  }
  case VFunc:
    return sdscatprintf(sdsempty(), "func<%s>", val->func->name);
  default:
    return NULL;
  }
}

int cmp_VMValue(VMValue *v1, VMValue *v2) {
  if (v1->ty != v2->ty) {
    return -2;
  }
  switch (v1->ty) {
  case VValue: {
    switch (v1->val->ty) {
    case float_ty: {
      double l = v1->val->float_val;
      double r = v2->val->float_val;
      if (l == r) {
        return 0;
      }
      if (l < r) {
        return -1;
      }
      return 1;
    }
    case bool_ty: {
      bool l = v1->val->bool_val;
      bool r = v2->val->bool_val;
      if (l == r) {
        return 0;
      }
      return -2;
    }
    case symbol_ty:
      return varcmp((void *)v1->val->symbol_val, (void *)v2->val->symbol_val);
    case string_ty:
      return varcmp((void *)v1->val->string_val, (void *)v2->val->string_val);
    case list_ty:
    case object_ty:
    case quote_ty:
      fprintf(stderr, "unimplemented!\n");
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Should not reach here\n");
    exit(EXIT_FAILURE);
  }
  case VFunc:
  default:
    fprintf(stderr, "Should not reach here\n");
    exit(EXIT_FAILURE);
  }
}

static inline SexpObject *get_SexpObject_VMValue(VMValue *vmv) {
  assert(vmv->ty == VValue);
  return vmv->val;
}

static inline VMFunction *get_func_VMValue(VMValue *vmv) {
  assert(vmv->ty == VFunc);
  return vmv->func;
}

static inline SexpObject *pop_SexpObject_from_stack(Stack *stack) {
  VMValue *v = (VMValue *)pop_Stack(stack);
  assert(v->ty == VValue);
  return v->val;
}

static inline VMFunction *pop_func_from_stack(Stack *stack) {
  VMValue *v = (VMValue *)pop_Stack(stack);
  assert(v->ty == VFunc);
  return v->func;
}

Registers *new_Registers(void) {
  Registers *registers = xmalloc(sizeof(Registers));
  registers->pc = 0;
  return registers;
}

void free_Registers(Registers **registers) { xfree(registers); }

Env *new_Env(void) {
  Env *env = xmalloc(sizeof(Env));
  env->vars = new_AVLTree(&varcmp);
  env->parent = NULL;
  env->copied = false;
  for (size_t i = 0; i < FUNC_CACHE_LEN; i++) {
    env->cached_funcs[i] = NULL;
  }
  return env;
}

static inline Vector *keys_Env(Env *env) { return avl_keys(env->vars); }

static inline VMValue *check_func_cache(Env *env, sds name) {
  VMValue *ret = NULL;

  for (size_t i = 0; i < FUNC_CACHE_LEN; i++) {
    VMValue *cached = env->cached_funcs[i];
    if (cached != NULL && cached->ty == VFunc &&
        strcmp(cached->func->name, name) == 0) {
      return cached;
    }
  }

  return ret;
}

static inline VMValue *get_Env_impl(Env *env, sds name) {
  if (env->parent == NULL) {
    return avl_find(env->vars, name);
  } else {
    if (env->copied == false) {
      for (Env *e = env->parent; e != NULL; e = e->parent) {
        VMValue *r = avl_find(e->vars, name);
        if (r) {
          return r;
        }
      }
      return NULL;
    } else {
      return avl_find(env->vars, name);
    }
  }
}

static inline VMValue *get_Env(Env *env, sds name) {
  VMValue *ret = check_func_cache(env, name);
  if (ret == NULL) {
    return get_Env_impl(env, name);
  } else {
    return ret;
  }
}

static inline void insert_Env_impl(Env *env, sds name, VMValue *val) {
  if (env->parent != NULL && env->copied == false) {
    Vector *keys = keys_Env(env->parent);
    for (size_t i = 0; i < keys->len; i++) {
      sds key = keys->data[i];
      avl_insert(env->vars, key, dup_VMValue(get_Env(env->parent, key)));
    }
    env->copied = true;
  }

  avl_insert(env->vars, name, val);
}

static inline void update_func_cache(Env *env, sds name, VMValue *val) {
  size_t i = 0;
  for (; i < FUNC_CACHE_LEN && env->cached_funcs[i] != NULL; i++) {
    VMValue *cached = env->cached_funcs[i];
    if (cached != NULL && cached->ty == VFunc &&
        strcmp(cached->func->name, name) == 0) {
      env->cached_funcs[i] = val;
      return;
    }
  }

  env->cached_funcs[i == FUNC_CACHE_LEN ? 0 : i] = val;
}

static inline void insert_Env(Env *env, sds name, VMValue *val) {
  if (val->ty == VFunc) {
    update_func_cache(env, name, val);
  }
  return insert_Env_impl(env, name, val);
}

Env *dup_Env(Env *env) {
  Env *new_env = new_Env();
  new_env->parent = env;

  for (size_t i = 0; i < FUNC_CACHE_LEN; i++) {
    new_env->cached_funcs[i] = env->cached_funcs[i];
  }

  return new_env;
}

// TODO: impl free AVL
void free_Env(Env **env) { xfree(env); }

Frame *new_Frame(void) {
  Frame *frame = xmalloc(sizeof(Frame));
  frame->registers = new_Registers();
  frame->env = new_Env();
  frame->args = new_vec();
  frame->parent = NULL;
  frame->lvars = NULL;
  frame->v_ins = NULL;
  return frame;
}

void free_Frame(Frame **frame) {
  free_Registers(&(*frame)->registers);
  free_Env(&(*frame)->env);
  // TODO: Free Vector
  xfree(&(*frame)->args);
  xfree(frame);
}

VMFunction *new_VMFunction(sds name, Vector *code, Vector *arg_names) {
  VMFunction *func = xmalloc(sizeof(VMFunction));
  func->name = name;
  func->code = code;
  func->arg_names = arg_names;
#ifdef __ENABLE_DIRECT_THREADED_CODE__
  func->ops_ptr = NULL;
#endif
  return func;
}

void free_VMFunction(VMFunction **vmf_ptr) {
  sdsfree((*vmf_ptr)->name);
  // TODO: Vector destructor
  xfree(vmf_ptr);
}

static double dmod(double x, double y) { return x - ((x / y) * y); }

static inline void push_Stack_VValue(Stack *stack, SexpObject *val) {
  push_Stack(stack, new_VMValueWithValue(val));
}

static inline void push_Stack_VFunc(Stack *stack, VMFunction *vmf) {
  push_Stack(stack, new_VMValueWithFunc(vmf));
}

void dump_stack(Stack *stack) {
  Vector *v = stack->data;
  for (size_t i = 0; i < v->len; i++) {
    void *e = v->data[i];
    printf("stack[%ld] %p : \n", i, e);
  }
}

static sds ADD_STR;
static sds SUB_STR;
static sds MUL_STR;
static sds DIV_STR;
static sds MOD_STR;
static sds EQ_STR;
static sds NEQ_STR;
static sds LT_STR;
static sds LEQ_STR;
static sds GT_STR;
static sds GEQ_STR;
static sds DEF_VAR_STR;
static sds DEF_FUN_STR;
static sds IF_STR;
static sds BEGIN_STR;
static sds WHILE_STR;
static sds SET_STR;

Vector *vm_compile_SexpObject(SexpObject *obj);

static inline void vm_compile_binary_fun(Opcode op, Vector *v, Vector *ret) {
  assert(v->len == 3);
  SexpObject *e1 = (SexpObject *)v->data[1];
  SexpObject *e2 = (SexpObject *)v->data[2];
  vec_append(ret, vm_compile_SexpObject(e1));
  vec_append(ret, vm_compile_SexpObject(e2));
  vec_pushi(ret, op);
}

static ssize_t check_var_name_in_arg_names(sds var_name, Vector *arg_names) {
  ssize_t found = -1;
  for (size_t i = 0; i < arg_names->len; i++) {
    sds arg_name = (sds)arg_names->data[arg_names->len - i - 1];
    if (strcmp(var_name, arg_name) == 0) {
      found = i;
      break;
    }
  }
  return found;
}

Vector *vm_compile_SexpObject(SexpObject *obj) {
  Vector *ret = new_vec();

  switch (obj->ty) {
  case float_ty:
  case bool_ty:
  case string_ty:
    vec_pushi(ret, OpPush);
    vec_push(ret, new_VMValueWithValue(obj));
    break;
  case symbol_ty:
    vec_pushi(ret, OpGetVar);
    vec_push(ret, obj->symbol_val);
    break;
  case list_ty: {
    Vector *v = obj->list_val;
    assert(v->len > 0);
    SexpObject *e1 = (SexpObject *)v->data[0];
    assert(e1->ty == symbol_ty);
    sds func_name = e1->symbol_val;

    if (sdscmp(func_name, ADD_STR) == 0) { // Add
      vm_compile_binary_fun(OpAdd, v, ret);
    } else if (sdscmp(func_name, SUB_STR) == 0) { // Sub
      vm_compile_binary_fun(OpSub, v, ret);
    } else if (sdscmp(func_name, MUL_STR) == 0) { // Mul
      vm_compile_binary_fun(OpMul, v, ret);
    } else if (sdscmp(func_name, DIV_STR) == 0) { // Div
      vm_compile_binary_fun(OpDiv, v, ret);
    } else if (sdscmp(func_name, MOD_STR) == 0) { // Mod
      vm_compile_binary_fun(OpMod, v, ret);
    } else if (sdscmp(func_name, EQ_STR) == 0) { // Eq
      vm_compile_binary_fun(OpEq, v, ret);
    } else if (sdscmp(func_name, NEQ_STR) == 0) { // Neq
      vm_compile_binary_fun(OpNeq, v, ret);
    } else if (sdscmp(func_name, LT_STR) == 0) { // Lt
      vm_compile_binary_fun(OpLt, v, ret);
    } else if (sdscmp(func_name, LEQ_STR) == 0) { // Leq
      vm_compile_binary_fun(OpLeq, v, ret);
    } else if (sdscmp(func_name, GT_STR) == 0) { // Gt
      vm_compile_binary_fun(OpGt, v, ret);
    } else if (sdscmp(func_name, GEQ_STR) == 0) { // Geq
      vm_compile_binary_fun(OpGeq, v, ret);
    } else if (sdscmp(func_name, DEF_VAR_STR) == 0) { // def-var
      assert(v->len == 3);
      SexpObject *e1 = (SexpObject *)v->data[1];
      SexpObject *e2 = (SexpObject *)v->data[2];
      assert(e1->ty == symbol_ty);
      vec_append(ret, vm_compile_SexpObject(e2));
      vec_pushi(ret, OpVarDef);
      vec_push(ret, e1->symbol_val);
    } else if (sdscmp(func_name, DEF_FUN_STR) == 0) { // def-fun
      assert(v->len >= 4);
      SexpObject *e1 = (SexpObject *)v->data[1];
      SexpObject *e2 = (SexpObject *)v->data[2];
      assert(e1->ty == symbol_ty); // func_name
      assert(e2->ty == list_ty);   // args

      Vector *arg_names = NULL;
      Vector *args = e2->list_val;
      for (size_t i = 0; i < args->len; i++) {
        if (i == 0) {
          arg_names = new_vec();
        }
        SexpObject *arg = (SexpObject *)args->data[i];
        assert(arg->ty == symbol_ty);
        vec_push(arg_names, arg->symbol_val);
      }

      Vector *func_body = new_vec();

      for (size_t i = 3; i < v->len; i++) {
        SexpObject *e = (SexpObject *)v->data[i];
        vec_append(func_body, vm_compile_SexpObject(e));
      }

      Vector *func_body_opt = new_vec();

      Vector *lvars = new_vec();

      for (size_t i = 0; i < func_body->len;) {
        Opcode op = (Opcode)func_body->data[i++];
        const int op_width = op_width_table[op] - 1;

        switch (op) {
        default: {
          vec_pushi(func_body_opt, op);
          for (int j = 0; j < op_width; j++) {
            vec_push(func_body_opt, func_body->data[i++]);
          }
          break;
        }
        case OpVarDef:
        case OpGetVar:
        case OpSetVar: {
          sds var_name = (sds)func_body->data[i++];

          ssize_t found = -1;
          size_t arg_names_len = 0;
          if (arg_names != NULL) {
            found = check_var_name_in_arg_names(var_name, arg_names);
            arg_names_len = arg_names->len;
          }
          if (found == -1) {
            if (op == OpVarDef || op == OpSetVar) {
              ssize_t lvar_idx_1 = -1;
              if (op == OpVarDef) {
                vec_push(lvars, var_name);
                lvar_idx_1 = lvars->len - 1;
              } else {
                for (size_t j = 0; j < lvars->len; j++) {
                  sds lvar_name = (sds)lvars->data[j];
                  if (strcmp(var_name, lvar_name) == 0) {
                    lvar_idx_1 = j;
                    break;
                  }
                }
              }

              if (lvar_idx_1 == -1) {
                vec_pushi(func_body_opt, op);
                vec_push(func_body_opt, var_name);
              } else {
                size_t lvar_idx = arg_names_len + lvar_idx_1;
                vec_pushi(func_body_opt, OpSetLocal);
                vec_pushi(func_body_opt, lvar_idx);
              }
            } else {
              ssize_t found_lvar = -1;
              for (size_t j = 0; j < lvars->len; j++) {
                sds lvar_name = (sds)lvars->data[j];
                if (strcmp(var_name, lvar_name) == 0) {
                  found_lvar = j;
                  break;
                }
              }
              if (found_lvar == -1) {
                // if not found, raise exception by OpGetVar
                vec_pushi(func_body_opt, op);
                vec_push(func_body_opt, var_name);
              } else {
                ssize_t lvar_idx = arg_names_len + found_lvar;
                vec_pushi(func_body_opt, OpGetLocal);
                vec_pushi(func_body_opt, lvar_idx);
              }
            }
          } else {
            vec_pushi(func_body_opt, op == OpVarDef ? OpSetLocal : OpGetLocal);
            vec_pushi(func_body_opt, found);
          }

          break;
        }
        }
      }
      xfree(&func_body);

      func_body = new_vec();

      size_t alloca_size = 0;

      if (arg_names != NULL) {
        alloca_size += arg_names->len;
      }
      alloca_size += lvars->len;

      vec_pushi(func_body, OpAllocLvars);
      vec_pushi(func_body, alloca_size);

      if (arg_names != NULL) {
        for (size_t i = 0; i < arg_names->len; i++) {
          vec_pushi(func_body, OpSetArgLocal);
          vec_pushi(func_body, i);
        }
      }

      vec_append(func_body, func_body_opt);

      if (alloca_size) {
        vec_pushi(func_body, OpFreeLvars);
      }
      vec_pushi(func_body, OpReturn);

      VMFunction *vmf = new_VMFunction(e1->symbol_val, func_body, arg_names);
      vec_pushi(ret, OpFuncDef);
      vec_push(ret, new_VMValueWithFunc(vmf));

      xfree(&lvars);
    } else if (sdscmp(func_name, IF_STR) == 0) { // IF
      assert(v->len >= 3);
      SexpObject *cond = v->data[1];
      SexpObject *tBlock = v->data[2];

      vec_append(ret, vm_compile_SexpObject(cond));

      Vector *tBlock_ins = vm_compile_SexpObject(tBlock);

      long long int tBlock_len = tBlock_ins->len;
      vec_pushi(ret, OpBranch);
      vec_pushi(ret, tBlock_len + (v->len == 4 ? 2 : 0)); // skip fBlock

      vec_append(ret, tBlock_ins);

      if (v->len == 4) { // exist fBlock
        Vector *fBlock_ins = vm_compile_SexpObject(v->data[3]);
        long long int fBlock_len = fBlock_ins->len;
        vec_pushi(ret, OpJumpRel);
        vec_pushi(ret, fBlock_len);

        vec_append(ret, fBlock_ins);
      }
    } else if (sdscmp(func_name, BEGIN_STR) == 0) { // begin
      assert(v->len >= 2);
      for (size_t i = 1; i < v->len; i++) {
        vec_append(ret, vm_compile_SexpObject(v->data[i]));
      }
    } else if (sdscmp(func_name, WHILE_STR) == 0) { // while
      assert(v->len == 3);
      SexpObject *cond = v->data[1];
      SexpObject *expr = v->data[2];

      Vector *cond_ins = vm_compile_SexpObject(cond);
      Vector *expr_ins = vm_compile_SexpObject(expr);
      vec_pushi(expr_ins, OpJumpRel);
      int joffset =
          -(expr_ins->len + 1 /* this jump */ + cond_ins->len + 2 /* branch */);
      vec_pushi(expr_ins, joffset);

      vec_append(ret, cond_ins);
      vec_pushi(ret, OpBranch);
      vec_pushi(ret, expr_ins->len);
      vec_append(ret, expr_ins);
    } else if (sdscmp(func_name, SET_STR) == 0) {
      assert(v->len == 3);
      SexpObject *var_name = v->data[1];
      assert(var_name->ty == symbol_ty);
      SexpObject *expr = v->data[2];
      Vector *expr_ins = vm_compile_SexpObject(expr);

      vec_append(ret, expr_ins);
      vec_pushi(ret, OpSetVar);
      vec_push(ret, var_name->symbol_val);
    } else {
      size_t i = 0;

      for (; i < v->len - 1; i++) {
        SexpObject *e = (SexpObject *)v->data[i + 1];
        vec_append(ret, vm_compile_SexpObject(e));
      }

      vec_pushi(ret, OpCall);
      vec_push(ret, func_name);
      vec_pushi(ret, i);
    }
    break;
  }
  case object_ty:
    fprintf(stderr, "unimplemented!\n");
    exit(EXIT_FAILURE);
  case quote_ty: {
    SexpObject *iobj = obj->quote_val;

    switch (iobj->ty) {
    case list_ty: {
      Vector *v = iobj->list_val;
      size_t i = 0;

      for (; i < v->len; i++) {
        SexpObject *e = (SexpObject *)v->data[i];
        vec_append(ret, vm_compile_SexpObject(e));
      }

      vec_pushi(ret, OpMakeList);
      vec_pushi(ret, i);
      break;
    }
    case float_ty:
    case bool_ty:
    case string_ty:
    case symbol_ty:
    case object_ty:
      fprintf(stderr, "unimplemented!\n");
      exit(EXIT_FAILURE);
    }
  }
  }
  return ret;
}

Vector *vm_compile(Vector *parsed) {
  Vector *ret = new_vec();

  for (size_t i = 0; i < parsed->len; i++) {
    SexpObject *obj = (SexpObject *)parsed->data[i];
    vec_append(ret, vm_compile_SexpObject(obj));
  }

  return ret;
}

static AVLTree *builtin_functions;
static bool vm_initialized;

void vm_init(void) {
  if (vm_initialized) {
    return;
  }
  vm_initialized = true;

#ifdef __ENABLE_DIRECT_THREADED_CODE__
  printf("[INTERNAL VM INFO] Direct Threaded Code : Enabled\n");
#else
  printf("[INTERNAL VM INFO] Direct Threaded Code : Disabled\n");
#endif

  builtin_functions = new_AVLTree(&varcmp);

  avl_insert(builtin_functions, sdsnew("print"), (void *)(intptr_t)OpPrint);
  avl_insert(builtin_functions, sdsnew("println"), (void *)(intptr_t)OpPrintln);

  ADD_STR = sdsnew("+");
  SUB_STR = sdsnew("-");
  MUL_STR = sdsnew("*");
  DIV_STR = sdsnew("/");
  MOD_STR = sdsnew("%%");
  EQ_STR = sdsnew("==");
  NEQ_STR = sdsnew("!=");
  LT_STR = sdsnew("<");
  LEQ_STR = sdsnew("<=");
  GT_STR = sdsnew(">");
  GEQ_STR = sdsnew(">=");
  DEF_VAR_STR = sdsnew("def-var");
  DEF_FUN_STR = sdsnew("def-fun");
  IF_STR = sdsnew("if");
  BEGIN_STR = sdsnew("begin");
  WHILE_STR = sdsnew("while");
  SET_STR = sdsnew("set");
}

static inline int get_builtin(sds name) {
  bool e = avl_exists(builtin_functions, name);
  if (!e) {
    return -1;
  } else {
    return (int)(intptr_t)avl_find(builtin_functions, name);
  }
}

#ifdef __ENABLE_DIRECT_THREADED_CODE__
static inline void **gen_table(void **table, long long int table_len,
                               Vector *v_ins, void *L_end) {
  void **ops_ptr = xmalloc(sizeof(void *) * (v_ins->len + 1));
  for (size_t j = 0; j < v_ins->len; j++) {
    long long int idx = (long long int)v_ins->data[j];
    if (idx < table_len) {
      ops_ptr[j] = table[idx];
    }
  }
  ops_ptr[v_ins->len] = L_end;

  return ops_ptr;
}

SexpObject *vm_exec(Vector *v_ins) {
  if (!vm_initialized) {
    vm_init();
  }

  SexpObject *ret = NULL;
  register Stack *stack = new_Stack();
  register Stack *frame_stack = new_Stack();
  Frame *frame = new_Frame();
  frame->v_ins = v_ins;
  register Registers *reg = frame->registers;

  static void *table[] = {
      &&L_OpPop,      &&L_OpPush,       &&L_OpAllocLvars,  &&L_OpFreeLvars,
      &&L_OpGetLocal, &&L_OpSetLocal,   &&L_OpSetArgLocal, &&L_OpAdd,
      &&L_OpSub,      &&L_OpMul,        &&L_OpDiv,         &&L_OpMod,
      &&L_OpEq,       &&L_OpNeq,        &&L_OpLt,          &&L_OpLeq,
      &&L_OpGt,       &&L_OpGeq,        &&L_OpPrint,       &&L_OpPrintln,
      &&L_OpJumpRel,  &&L_OpFuncDef,    &&L_OpCall,        &&L_OpReturn,
      &&L_OpVarDef,   &&L_OpGetVar,     &&L_OpSetVar,      &&L_OpBranch,
      &&L_OpMakeList, &&L_OpSetArgFrom, &&L_OpDumpEnv};
  static const long long int table_len = sizeof(table) / sizeof(table[0]);
  void **ops_ptr = gen_table(table, table_len, v_ins, &&L_end);

//    printf("op: %s, reg: %p, reg->pc: %ld\n", #op_name, reg, reg->pc);
#define DTHC_CASE(op_name, proc_code)                                          \
  L_##op_name : {                                                              \
    proc_code;                                                                 \
    goto *ops_ptr[reg->pc++];                                                  \
  }

  // start
  long long int op = (long long int)(intptr_t)v_ins->data[reg->pc++];
  goto *table[op];

L_MAIN_LOOP:
  reg->pc++;
  goto *table[reg->pc];

L_OP_SELECT:
  goto *table[op];

  DTHC_CASE(OpPop, { pop_Stack(stack); });
  DTHC_CASE(OpPush, { push_Stack(stack, frame->v_ins->data[reg->pc++]); });
  DTHC_CASE(OpAllocLvars, {
    long long int vars = (long long int)frame->v_ins->data[reg->pc++];
    frame->lvars = new_vec_with(vars);
  });
  DTHC_CASE(OpFreeLvars, { xfree(&frame->lvars); });
  DTHC_CASE(OpGetLocal, {
    long long int var_idx = (long long int)frame->v_ins->data[reg->pc++];
    push_Stack(stack, frame->lvars->data[var_idx]);
  });
  DTHC_CASE(OpSetLocal, {
    long long int var_idx = (long long int)frame->v_ins->data[reg->pc++];
    frame->lvars->data[var_idx] = pop_Stack(stack);
  });
  DTHC_CASE(OpSetArgLocal, {
    long long int var_idx = (long long int)frame->v_ins->data[reg->pc++];
    frame->lvars->data[var_idx] = frame->args->data[var_idx];
  });
  DTHC_CASE(OpAdd, {
    double r = get_float_val(pop_SexpObject_from_stack(stack));
    double l = get_float_val(pop_SexpObject_from_stack(stack));
    push_Stack_VValue(stack, new_SexpObject_float(l + r));
  })
  DTHC_CASE(OpSub, {
    double r = get_float_val(pop_SexpObject_from_stack(stack));
    double l = get_float_val(pop_SexpObject_from_stack(stack));
    push_Stack_VValue(stack, new_SexpObject_float(l - r));
  });
  DTHC_CASE(OpMul, {
    double r = get_float_val(pop_SexpObject_from_stack(stack));
    double l = get_float_val(pop_SexpObject_from_stack(stack));
    push_Stack_VValue(stack, new_SexpObject_float(l * r));
  });
  DTHC_CASE(OpDiv, {
    double r = get_float_val(pop_SexpObject_from_stack(stack));
    double l = get_float_val(pop_SexpObject_from_stack(stack));
    push_Stack_VValue(stack, new_SexpObject_float(l / r));
  });
  DTHC_CASE(OpMod, {
    double r = get_float_val(pop_SexpObject_from_stack(stack));
    double l = get_float_val(pop_SexpObject_from_stack(stack));
    push_Stack_VValue(stack, new_SexpObject_float(dmod(l, r)));
  });
  DTHC_CASE(OpEq, {
    VMValue *r = pop_Stack(stack);
    VMValue *l = pop_Stack(stack);
    push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) == 0));
  });
  DTHC_CASE(OpNeq, {
    VMValue *r = pop_Stack(stack);
    VMValue *l = pop_Stack(stack);
    push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) != 0));
  });
  DTHC_CASE(OpLt, {
    VMValue *r = pop_Stack(stack);
    VMValue *l = pop_Stack(stack);
    push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) < 0));
  });
  DTHC_CASE(OpLeq, {
    VMValue *r = pop_Stack(stack);
    VMValue *l = pop_Stack(stack);
    push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) <= 0));
  });
  DTHC_CASE(OpGt, {
    VMValue *r = pop_Stack(stack);
    VMValue *l = pop_Stack(stack);
    push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) > 0));
  });
  DTHC_CASE(OpGeq, {
    VMValue *r = pop_Stack(stack);
    VMValue *l = pop_Stack(stack);
    push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) >= 0));
  });
  DTHC_CASE(OpPrint, {
    VMValue *val = pop_Stack(stack);
    printf("%s", show_VMValue(val));
  });
  DTHC_CASE(OpPrintln, {
    VMValue *val = pop_Stack(stack);
    printf("%s\n", show_VMValue(val));
  });
  DTHC_CASE(OpJumpRel, {
    long long int lv = (long long int)(intptr_t)frame->v_ins->data[reg->pc++];
    reg->pc += lv;
  });
  DTHC_CASE(OpFuncDef, {
    VMValue *vfptr = frame->v_ins->data[reg->pc++];
    VMFunction *vmf = get_func_VMValue(vfptr);
    insert_Env(frame->env, vmf->name, vfptr);
  });
  DTHC_CASE(OpCall, {
    sds func_name = (sds)frame->v_ins->data[reg->pc++];
    int bop = get_builtin(func_name);
    if (bop != -1) {
      op = bop;
      reg->pc++; // skip argc
      goto L_OP_SELECT;
    }

    VMValue *v = get_Env(frame->env, func_name);

    if (v == NULL) {
      fprintf(stderr, "No such a function : %s\n", func_name);
      exit(EXIT_FAILURE);
    }
    VMFunction *vmf = get_func_VMValue(v);

    size_t argc = (size_t)frame->v_ins->data[reg->pc++];
    Frame *new_frame = new_Frame();
    new_frame->env = dup_Env(frame->env);
    new_frame->args = new_vec();
    new_frame->parent = frame;

    for (size_t i = 0; i < argc; i++) {
      vec_push(new_frame->args, pop_Stack(stack));
    }

    new_frame->v_ins = vmf->code;

    push_Stack(frame_stack, (void **)ops_ptr);
    if (vmf->ops_ptr == NULL) {
      vmf->ops_ptr = gen_table(table, table_len, vmf->code, &&L_end);
    }
    ops_ptr = vmf->ops_ptr;

    push_Stack(frame_stack, frame); // 戻る先のフレーム
    frame = new_frame;
    reg = new_frame->registers;
  });
  DTHC_CASE(OpReturn, {
    free_Frame(&frame);
    frame = pop_Stack(frame_stack); // フレームを復元
    reg = frame->registers;

    ops_ptr = pop_Stack(frame_stack);
  });
  DTHC_CASE(OpVarDef, {
    sds var_name = (sds)frame->v_ins->data[reg->pc++];
    VMValue *v = pop_Stack(stack);
    insert_Env(frame->env, var_name, v);
    // printf("def! name: %s, val: %s\n", var_name, show_VMValue(v));
  });
  DTHC_CASE(OpGetVar, {
    sds var_name = (sds)frame->v_ins->data[reg->pc++];
    VMValue *v = get_Env(frame->env, var_name);
    // printf("get! name: %s, val: %s\n", var_name, show_VMValue(v));
    push_Stack(stack, v);
  });
  DTHC_CASE(OpSetVar, {
    sds var_name = (sds)frame->v_ins->data[reg->pc++];
    VMValue *v = pop_Stack(stack);
    insert_Env(frame->env, var_name, v);
  });
  DTHC_CASE(OpBranch, {
    long long int tBlock_len =
        (long long int)(intptr_t)frame->v_ins->data[reg->pc++];
    SexpObject *cond_result = pop_SexpObject_from_stack(stack);
    assert(cond_result->ty == bool_ty);

    if (!cond_result->bool_val) {
      reg->pc += tBlock_len;
    }
  });
  DTHC_CASE(OpMakeList, {
    long long int list_len =
        (long long int)(intptr_t)frame->v_ins->data[reg->pc++];
    Vector *list = new_vec_with(list_len);
    list->len = list_len;

    for (long long int i = list_len - 1; i >= 0; i--) {
      list->data[i] = pop_Stack(stack);
    }

    push_Stack_VValue(stack, new_SexpObject_list(list));
  });
  DTHC_CASE(OpSetArgFrom, {
    sds arg_name = (sds)frame->v_ins->data[reg->pc++];
    size_t arg_idx = (size_t)(intptr_t)frame->v_ins->data[reg->pc++];
    assert(arg_idx < frame->args->len);

    insert_Env(frame->env, arg_name, frame->args->data[arg_idx]);
  });
  DTHC_CASE(OpDumpEnv, {
    Vector *keys = avl_keys(frame->env->vars);
    for (size_t i = 0; i < keys->len; i++) {
      sds key = (sds)keys->data[i];
      VMValue *v = get_Env(frame->env, key);
      printf("%s - %s\n", key, v->ty == VValue ? "VValue" : "VFunc");
    }
  });

L_end:
  // 戻るべきフレームが存在する．
  if (frame->parent != NULL) {
    free_Frame(&frame);
    frame = pop_Stack(frame_stack); // フレームを復元
    reg = frame->registers;
    ops_ptr = (void **)pop_Stack(frame_stack);
    goto L_MAIN_LOOP;
  }

  if (!isempty_Stack(stack)) {
    ret = (SexpObject *)pop_Stack(stack);
  }

  return ret;
}

#else

SexpObject *vm_exec(Vector *v_ins) {
  if (!vm_initialized) {
    vm_init();
  }

  SexpObject *ret = NULL;
  Stack *stack = new_Stack();
  Stack *frame_stack = new_Stack();
  Frame *frame = new_Frame();
  frame->v_ins = v_ins;
  Registers *reg = frame->registers;

MAIN_LOOP:
  for (; reg->pc < frame->v_ins->len;) {
    Opcode op = (Opcode)frame->v_ins->data[reg->pc++];
    // printf("op: %lld, reg: %p, reg->pc: %lld\n", op, reg, reg->pc);
  OP_SELECT:
    switch (op) {
    case OpPop:
      pop_Stack(stack);
      break;
    case OpPush:
      push_Stack(stack, frame->v_ins->data[reg->pc++]);
      break;

    case OpAllocLvars: {
      long long int vars = (long long int)frame->v_ins->data[reg->pc++];
      frame->lvars = new_vec_with(vars);
      break;
    }
    case OpFreeLvars: {
      xfree(&frame->lvars);
      break;
    }
    case OpGetLocal: {
      long long int var_idx = (long long int)frame->v_ins->data[reg->pc++];
      push_Stack(stack, frame->lvars->data[var_idx]);
      break;
    }
    case OpSetLocal: {
      long long int var_idx = (long long int)frame->v_ins->data[reg->pc++];
      frame->lvars->data[var_idx] = pop_Stack(stack);
      break;
    }
    case OpSetArgLocal: {
      long long int var_idx = (long long int)frame->v_ins->data[reg->pc++];
      frame->lvars->data[var_idx] = frame->args->data[var_idx];
      break;
    }
    case OpAdd: {
      double r = get_float_val(pop_SexpObject_from_stack(stack));
      double l = get_float_val(pop_SexpObject_from_stack(stack));
      push_Stack_VValue(stack, new_SexpObject_float(l + r));
      break;
    }
    case OpSub: {
      double r = get_float_val(pop_SexpObject_from_stack(stack));
      double l = get_float_val(pop_SexpObject_from_stack(stack));
      push_Stack_VValue(stack, new_SexpObject_float(l - r));
      break;
    }
    case OpMul: {
      double r = get_float_val(pop_SexpObject_from_stack(stack));
      double l = get_float_val(pop_SexpObject_from_stack(stack));
      push_Stack_VValue(stack, new_SexpObject_float(l * r));
      break;
    }
    case OpDiv: {
      double r = get_float_val(pop_SexpObject_from_stack(stack));
      double l = get_float_val(pop_SexpObject_from_stack(stack));
      push_Stack_VValue(stack, new_SexpObject_float(l / r));
      break;
    }
    case OpMod: {
      double r = get_float_val(pop_SexpObject_from_stack(stack));
      double l = get_float_val(pop_SexpObject_from_stack(stack));
      push_Stack_VValue(stack, new_SexpObject_float(dmod(l, r)));
      break;
    }
    case OpEq: {
      VMValue *r = pop_Stack(stack);
      VMValue *l = pop_Stack(stack);
      push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) == 0));
      break;
    }
    case OpNeq: {
      VMValue *r = pop_Stack(stack);
      VMValue *l = pop_Stack(stack);
      push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) != 0));
      break;
    }
    case OpLt: {
      VMValue *r = pop_Stack(stack);
      VMValue *l = pop_Stack(stack);
      push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) < 0));
      break;
    }
    case OpLeq: {
      VMValue *r = pop_Stack(stack);
      VMValue *l = pop_Stack(stack);
      push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) <= 0));
      break;
    }
    case OpGt: {
      VMValue *r = pop_Stack(stack);
      VMValue *l = pop_Stack(stack);
      push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) > 0));
      break;
    }
    case OpGeq: {
      VMValue *r = pop_Stack(stack);
      VMValue *l = pop_Stack(stack);
      push_Stack_VValue(stack, new_SexpObject_bool(cmp_VMValue(l, r) >= 0));
      break;
    }
    case OpPrint: {
      VMValue *val = pop_Stack(stack);
      printf("%s", show_VMValue(val));
      break;
    }
    case OpPrintln: {
      VMValue *val = pop_Stack(stack);
      printf("%s\n", show_VMValue(val));
      break;
    }
    case OpJumpRel: {
      long long int lv = (long long int)(intptr_t)frame->v_ins->data[reg->pc++];
      reg->pc += lv;
      break;
    }
    case OpFuncDef: {
      VMValue *vfptr = frame->v_ins->data[reg->pc++];
      VMFunction *vmf = get_func_VMValue(vfptr);
      insert_Env(frame->env, vmf->name, vfptr);
      break;
    }
    case OpCall: {
      sds func_name = (sds)frame->v_ins->data[reg->pc++];
      int bop = get_builtin(func_name);
      if (bop != -1) {
        op = bop;
        reg->pc++; // skip argc
        goto OP_SELECT;
      }

      VMValue *v = get_Env(frame->env, func_name);

      if (v == NULL) {
        fprintf(stderr, "No such a function : %s\n", func_name);
        exit(EXIT_FAILURE);
      }
      VMFunction *vmf = get_func_VMValue(v);

      size_t argc = (size_t)frame->v_ins->data[reg->pc++];
      Frame *new_frame = new_Frame();
      new_frame->env = dup_Env(frame->env);
      new_frame->args = new_vec();
      new_frame->parent = frame;

      for (size_t i = 0; i < argc; i++) {
        vec_push(new_frame->args, pop_Stack(stack));
      }

      new_frame->v_ins = vmf->code;

      push_Stack(frame_stack, frame); // 戻る先のフレーム
      frame = new_frame;
      reg = new_frame->registers;
      break;
    }
    case OpReturn: {
      free_Frame(&frame);
      frame = pop_Stack(frame_stack); // フレームを復元
      reg = frame->registers;
      break;
    }
    case OpVarDef: {
      sds var_name = (sds)frame->v_ins->data[reg->pc++];
      VMValue *v = pop_Stack(stack);
      insert_Env(frame->env, var_name, v);
      // printf("def! name: %s, val: %s\n", var_name, show_VMValue(v));
      break;
    }
    case OpGetVar: {
      sds var_name = (sds)frame->v_ins->data[reg->pc++];
      VMValue *v = get_Env(frame->env, var_name);
      // printf("get! name: %s, val: %s\n", var_name, show_VMValue(v));
      push_Stack(stack, v);
      break;
    }
    case OpSetVar: {
      sds var_name = (sds)frame->v_ins->data[reg->pc++];
      VMValue *v = pop_Stack(stack);
      insert_Env(frame->env, var_name, v);
      break;
    }
    case OpBranch: {
      long long int tBlock_len =
          (long long int)(intptr_t)frame->v_ins->data[reg->pc++];
      SexpObject *cond_result = pop_SexpObject_from_stack(stack);
      assert(cond_result->ty == bool_ty);

      if (!cond_result->bool_val) {
        reg->pc += tBlock_len;
      }
      break;
    }
    case OpMakeList: {
      long long int list_len =
          (long long int)(intptr_t)frame->v_ins->data[reg->pc++];
      Vector *list = new_vec_with(list_len);
      list->len = list_len;

      for (long long int i = list_len - 1; i >= 0; i--) {
        list->data[i] = pop_Stack(stack);
      }

      push_Stack_VValue(stack, new_SexpObject_list(list));
      break;
    }
    case OpSetArgFrom: {
      sds arg_name = (sds)frame->v_ins->data[reg->pc++];
      size_t arg_idx = (size_t)(intptr_t)frame->v_ins->data[reg->pc++];
      assert(arg_idx < frame->args->len);

      // printf("OpSetArgFrom! name: %s, data: %s\n", arg_name,
      // show_VMValue(frame->args->data[arg_idx]));
      insert_Env(frame->env, arg_name, frame->args->data[arg_idx]);
      break;
    }
    case OpDumpEnv: {
      Vector *keys = avl_keys(frame->env->vars);
      for (size_t i = 0; i < keys->len; i++) {
        sds key = (sds)keys->data[i];
        VMValue *v = get_Env(frame->env, key);
        printf("%s - %s\n", key, v->ty == VValue ? "VValue" : "VFunc");
      }
      break;
    }
    default:
      fprintf(stderr, "Unkown op given. op: %lld\n", op);
      exit(EXIT_FAILURE);
      break;
    }
  }

  // 戻るべきフレームが存在する．
  if (frame->parent != NULL) {
    free_Frame(&frame);
    frame = pop_Stack(frame_stack); // フレームを復元
    reg = frame->registers;
    goto MAIN_LOOP;
  }

  if (!isempty_Stack(stack)) {
    ret = (SexpObject *)pop_Stack(stack);
  }

  return ret;
}
#endif

void vm_ins_dump_impl(Vector *v_ins, size_t depth) {
  sds sp = sdsempty();

  for (size_t i = 0; i < depth; i++) {
    sp = sdscatprintf(sp, "  ");
  }

  for (size_t i = 0; i < v_ins->len;) {
    printf("%s[ins: %ld] ", sp, i);
    Opcode op = (Opcode)v_ins->data[i++];
    switch (op) {
    case OpPop:
      printf("OpPop\n");
      break;
    case OpPush:
      printf("OpPush %s\n", show_VMValue(v_ins->data[i++]));
      break;
    case OpAllocLvars:
      printf("OpAllocLvars %lld\n", (long long int)v_ins->data[i++]);
      break;
    case OpFreeLvars:
      printf("OpFreeLvars\n");
      break;
    case OpGetLocal:
      printf("OpGetLocal %lld\n", (long long int)v_ins->data[i++]);
      break;
    case OpSetLocal:
      printf("OpSetLocal %lld\n", (long long int)v_ins->data[i++]);
      break;
    case OpSetArgLocal:
      printf("OpSetArgLocal %lld\n", (long long int)v_ins->data[i++]);
      break;
    case OpAdd: {
      printf("OpAdd\n");
      break;
    }
    case OpSub: {
      printf("OpSub\n");
      break;
    }
    case OpMul: {
      printf("OpMul\n");
      break;
    }
    case OpDiv: {
      printf("OpDiv\n");
      break;
    }
    case OpMod: {
      printf("OpMod\n");
      break;
    }
    case OpEq: {
      printf("OpEq\n");
      break;
    }
    case OpNeq: {
      printf("OpNeq\n");
      break;
    }
    case OpLt: {
      printf("OpLt\n");
      break;
    }
    case OpLeq: {
      printf("OpLeq\n");
      break;
    }
    case OpGt: {
      printf("OpGt\n");
      break;
    }
    case OpGeq: {
      printf("OpGeq\n");
      break;
    }
    case OpPrint: {
      printf("OpPrint\n");
      break;
    }
    case OpPrintln: {
      printf("OpPrintln\n");
      break;
    }
    case OpJumpRel: {
      long long int offset = (long long int)(intptr_t)v_ins->data[i++];
      printf("OpJumpRel %lld\n", offset);
      break;
    }
    case OpFuncDef: {
      VMValue *vfptr = v_ins->data[i++];
      VMFunction *vmf = get_func_VMValue(vfptr);
      printf("OpFuncDef %s\n", vmf->name);
      vm_ins_dump_impl(vmf->code, depth + 1);
      break;
    }
    case OpCall: {
      sds func_name = (sds)v_ins->data[i++];
      size_t argc = (size_t)v_ins->data[i++];
      printf("OpCall %s, %ld\n", func_name, argc);
      break;
    }
    case OpReturn: {
      printf("OpReturn\n");
      break;
    }
    case OpVarDef: {
      sds var_name = (sds)v_ins->data[i++];
      printf("OpVarDef %s\n", var_name);
      break;
    }
    case OpGetVar: {
      sds var_name = (sds)v_ins->data[i++];
      printf("OpGetVar %s\n", var_name);
      break;
    }
    case OpSetVar: {
      sds var_name = (sds)v_ins->data[i++];
      printf("OpSetVar %s\n", var_name);
      break;
    }
    case OpBranch: {
      long long int tBlock_len = (long long int)(intptr_t)v_ins->data[i++];
      printf("OpBranch %lld\n", tBlock_len);
      break;
    }
    case OpDumpEnv: {
      printf("OpDumpEnv\n");
      break;
    }
    case OpMakeList: {
      size_t list_len = (intptr_t)v_ins->data[i++];
      printf("OpMakeList %ld\n", list_len);
      break;
    }
    case OpSetArgFrom: {
      sds arg_name = (sds)v_ins->data[i++];
      size_t arg_idx = (size_t)(intptr_t)v_ins->data[i++];
      printf("OpSetArgFrom %s %ld\n", arg_name, arg_idx);
      break;
    }
    default:
      if (0 <= op && op < (long long int)sizeof(op_to_string)) {
        fprintf(stderr, "Unkown op given. op: %s\n", op_to_string[op]);
      } else {
        fprintf(stderr, "Unkown op given. op: %lld\n", op);
      }
      exit(EXIT_FAILURE);
      break;
    }
  }
}

void vm_ins_dump(Vector *v_ins) { vm_ins_dump_impl(v_ins, 0); }

static inline void serialize_string(Vector *ret, sds str) {
  size_t str_len = strlen(str);
  vec_pushlli(ret, str_len);
  for (size_t i = 0; i < str_len; i++) {
    char c = str[i];
    vec_pushi(ret, c);
  }
}

static Vector *binarize_SexpObject(VMValue *value) {
  Vector *ret = new_vec();

  vec_pushi(ret, value->ty);
  switch (value->ty) {
  case VValue: {
    SexpObject *obj = value->val;
    vec_pushi(ret, obj->ty);

    switch (obj->ty) {
    case float_ty: {
      double fv = obj->float_val;
      long long int *ptr = (long long int *)&fv;
      vec_pushlli(ret, *ptr);
      break;
    }
    case bool_ty: {
      bool bv = obj->bool_val;
      vec_pushi(ret, bv);
      break;
    }
    case string_ty:
    case symbol_ty: {
      sds str;
      if (obj->ty == string_ty) {
        str = obj->string_val;
      } else {
        str = obj->symbol_val;
      }

      serialize_string(ret, str);

      break;
    }
    case list_ty:
      unimplemented();
      break;
    case object_ty:
      unimplemented();
      break;
    case quote_ty:
      unimplemented();
      break;
    default:
      break;
    }

    break;
  }
  case VFunc: {
    VMFunction *vmf = value->func;
    serialize_string(ret, vmf->name);

    Vector *code = vm_binarize(vmf->code);
    vec_pushlli(ret, code->len);
    vec_append(ret, code);

    Vector *arg_names = vmf->arg_names;
    if (arg_names == NULL) {
      vec_pushlli(ret, 0);
    } else {
      vec_pushlli(ret, arg_names->len);
      for (size_t i = 0; i < arg_names->len; i++) {
        sds arg_name = (sds)arg_names->data[i];
        serialize_string(ret, arg_name);
      }
    }
    break;
  }
  }

  return ret;
}

typedef struct {
  Vector *ret;
} SerializeContext;

SerializeContext *new_SerializeContext(void) {
  SerializeContext *ctx = xnew(SerializeContext);
  ctx->ret = new_vec();
  return ctx;
}

static inline void emit_op(SerializeContext *ctx, Opcode op) {
  vec_pushlli(ctx->ret, op);
}

static inline void emit_op_1arg_vmv(SerializeContext *ctx, Opcode op,
                                    VMValue *vmvalue) {
  emit_op(ctx, op);
  vec_append(ctx->ret, binarize_SexpObject(vmvalue));
}

static inline void emit_op_1arg_llv(SerializeContext *ctx, Opcode op,
                                    long long int llv) {
  emit_op(ctx, op);
  vec_pushlli(ctx->ret, llv);
}

static inline void emit_op_1arg_sds(SerializeContext *ctx, Opcode op, sds str) {
  emit_op(ctx, op);
  serialize_string(ctx->ret, str);
}

static inline void emit_op_2arg_sds_size(SerializeContext *ctx, Opcode op,
                                         sds str, size_t size) {
  emit_op_1arg_sds(ctx, op, str);
  vec_pushlli(ctx->ret, size);
}

Vector *vm_binarize(Vector *v_ins) {
  SerializeContext *ctx = new_SerializeContext();

  for (size_t i = 0; i < v_ins->len;) {
    Opcode op = (Opcode)v_ins->data[i++];

    switch (op) {
    case OpPop:
      emit_op(ctx, op);
      break;
    case OpPush:
      emit_op_1arg_vmv(ctx, op, (VMValue *)v_ins->data[i++]);
      break;
    case OpAllocLvars:
    case OpGetLocal:
    case OpSetLocal:
    case OpSetArgLocal:
      emit_op_1arg_llv(ctx, op, (long long int)v_ins->data[i++]);
      break;
    case OpFreeLvars:
    case OpAdd:
    case OpSub:
    case OpMul:
    case OpDiv:
    case OpMod:
    case OpEq:
    case OpNeq:
    case OpLt:
    case OpLeq:
    case OpGt:
    case OpGeq:
    case OpPrint:
    case OpPrintln:
      emit_op(ctx, op);
      break;
    case OpJumpRel: {
      long long int llv = (long long int)v_ins->data[i++];
      emit_op_1arg_llv(ctx, op, llv);
      break;
    }
    case OpFuncDef: {
      VMValue *vfptr = v_ins->data[i++];
      emit_op_1arg_vmv(ctx, op, vfptr);
      break;
    }
    case OpCall: {
      sds func_name = (sds)v_ins->data[i++];
      size_t argc = (size_t)v_ins->data[i++];
      emit_op_2arg_sds_size(ctx, op, func_name, argc);
      break;
    }
    case OpReturn: {
      emit_op(ctx, op);
      break;
    }
    case OpVarDef: {
      sds var_name = (sds)v_ins->data[i++];
      emit_op_1arg_sds(ctx, op, var_name);
      break;
    }
    case OpGetVar: {
      sds var_name = (sds)v_ins->data[i++];
      emit_op_1arg_sds(ctx, op, var_name);
      break;
    }
    case OpBranch: {
      long long int tBlock_len = (long long int)(intptr_t)v_ins->data[i++];
      emit_op_1arg_llv(ctx, op, tBlock_len);
      break;
    }
    case OpMakeList: {
      long long int list_len = (long long int)(intptr_t)v_ins->data[i++];
      emit_op_1arg_llv(ctx, op, list_len);
      break;
    }
    case OpSetArgFrom: {
      sds arg_name = (sds)v_ins->data[i++];
      size_t arg_idx = (size_t)(intptr_t)v_ins->data[i++];
      emit_op_2arg_sds_size(ctx, op, arg_name, arg_idx);
      break;
    }
    case OpDumpEnv: {
      emit_op(ctx, op);
      break;
    }
    default:
      if (0 <= op && op < (long long int)sizeof(op_to_string)) {
        fprintf(stderr, "Unkown op given. op: %s\n", op_to_string[op]);
      } else {
        fprintf(stderr, "Unkown op given. op: %lld\n", op);
      }
      exit(EXIT_FAILURE);
      break;
    }
  }

  return ctx->ret;
}

typedef struct {
  Vector *serialized;
  size_t read_len;
} DeserializeResult;

typedef struct {
  sds str;
  size_t read_len;
} DeserializeStringResult;

static inline DeserializeStringResult deserialize_string(Vector *serialized,
                                                         size_t first_idx) {
  size_t idx = first_idx;
  size_t str_len = (size_t)serialized->data[idx++];

  char *buf = xmalloc(str_len + 1);
  for (size_t i = 0; i < str_len; i++) {
    char c = (char)serialized->data[idx++];
    buf[i] = c;
  }
  buf[str_len] = '\0';

  DeserializeStringResult dsr = {.str = sdsnew(buf),
                                 .read_len = idx - first_idx};
  return dsr;
}

DeserializeResult deserialize_vmv(Vector *serialized, size_t first_idx) {
  Vector *ret = new_vec();
  size_t idx = first_idx;

  int ty = (intptr_t)serialized->data[idx++];
  switch (ty) {
  case VValue: {
    int obj_ty = (intptr_t)serialized->data[idx++];

    switch (obj_ty) {
    case float_ty: {
      long long int lv = (long long int)serialized->data[idx++];
      double dv;
      memcpy(&dv, &lv, sizeof(double));
      vec_push(ret, new_VMValueWithValue(new_SexpObject_float(dv)));
      break;
    }
    case bool_ty: {
      unimplemented();
      break;
    }
    case string_ty:
    case symbol_ty: {
      DeserializeStringResult dsr = deserialize_string(serialized, idx);
      idx += dsr.read_len;

      if (obj_ty == string_ty) {
        vec_push(ret, new_VMValueWithValue(new_SexpObject_string(dsr.str)));
      } else {
        vec_push(ret, new_VMValueWithValue(new_SexpObject_symbol(dsr.str)));
      }

      break;
    }
    case list_ty:
      unimplemented();
      break;
    case object_ty:
      unimplemented();
      break;
    case quote_ty:
      unimplemented();
      break;
    default:
      break;
    }

    break;
  }
  case VFunc: {
    DeserializeStringResult dsr = deserialize_string(serialized, idx);
    idx += dsr.read_len;
    sds func_name = dsr.str;

    Vector *func_body_serialized = new_vec();
    long long int code_len = (long long int)serialized->data[idx++];
    for (long long int i = 0; i < code_len; i++) {
      vec_push(func_body_serialized, serialized->data[idx++]);
    }
    Vector *func_body = vm_deserialize(func_body_serialized);

    size_t arg_count = (size_t)serialized->data[idx++];
    Vector *args = NULL;
    for (size_t i = 0; i < arg_count; i++) {
      if (i == 0) {
        args = new_vec();
      }

      DeserializeStringResult arg_dsr = deserialize_string(serialized, idx);
      idx += arg_dsr.read_len;

      vec_push(args, arg_dsr.str);
    }

    VMFunction *vmf = new_VMFunction(func_name, func_body, args);
    vec_push(ret, new_VMValueWithFunc(vmf));
    break;
  }
  default: {
    fprintf(stderr, "unkown type given : %d\n", ty);
    exit(EXIT_FAILURE);
    break;
  }
  }

  DeserializeResult result = {.serialized = ret, .read_len = idx - first_idx};
  return result;
}

DeserializeResult deserialize_sds_size(Vector *serialized, size_t first_idx) {
  Vector *ret = new_vec();
  size_t idx = first_idx;

  DeserializeStringResult dsr = deserialize_string(serialized, idx);
  idx += dsr.read_len;
  vec_push(ret, dsr.str);
  vec_pushi(ret, (size_t)serialized->data[idx++]);

  DeserializeResult result = {.serialized = ret, .read_len = idx - first_idx};
  return result;
}

DeserializeResult *new_DeserializeResult(Vector *serialized) {
  DeserializeResult *ctx = xnew(DeserializeResult);
  ctx->serialized = serialized;
  ctx->read_len = 0;
  return ctx;
}

Vector *vm_deserialize(Vector *serialized) {
  Vector *code = new_vec();

  for (size_t i = 0; i < serialized->len;) {
    Opcode op = (Opcode)serialized->data[i++];

    switch (op) {
    case OpPop:
      vec_pushi(code, op);
      break;
    case OpPush: {
      vec_pushi(code, OpPush);
      DeserializeResult result = deserialize_vmv(serialized, i);
      vec_append(code, result.serialized);
      i += result.read_len;
      break;
    }
    case OpAllocLvars:
    case OpGetLocal:
    case OpSetLocal:
    case OpSetArgLocal: {
      vec_pushi(code, op);
      long long int lv = (long long int)serialized->data[i++];
      vec_pushlli(code, lv);
      break;
    }
    case OpFreeLvars:
    case OpAdd:
    case OpSub:
    case OpMul:
    case OpDiv:
    case OpMod:
    case OpEq:
    case OpNeq:
    case OpLt:
    case OpLeq:
    case OpGt:
    case OpGeq:
    case OpPrint:
    case OpPrintln:
      vec_pushi(code, op);
      break;
    case OpJumpRel: {
      vec_pushi(code, op);
      long long int lv = (long long int)serialized->data[i++];
      vec_pushlli(code, lv);
      break;
    }
    case OpFuncDef: {
      vec_pushi(code, op);
      DeserializeResult result = deserialize_vmv(serialized, i);
      vec_append(code, result.serialized);
      i += result.read_len;
      break;
    }
    case OpCall: {
      vec_pushi(code, op);
      DeserializeResult result = deserialize_sds_size(serialized, i);
      vec_append(code, result.serialized);
      i += result.read_len;
      break;
    }
    case OpReturn: {
      vec_pushi(code, op);
      break;
    }
    case OpVarDef:
    case OpGetVar: {
      vec_pushi(code, op);
      DeserializeStringResult dsr = deserialize_string(serialized, i);
      vec_push(code, dsr.str);
      i += dsr.read_len;
      break;
    }
    case OpBranch:
    case OpMakeList: {
      vec_pushi(code, op);
      long long int lv = (long long int)serialized->data[i++];
      vec_pushlli(code, lv);
      break;
    }
    case OpSetArgFrom: {
      vec_pushi(code, op);
      DeserializeResult result = deserialize_sds_size(serialized, i);
      vec_append(code, result.serialized);
      i += result.read_len;
      break;
    }
    case OpDumpEnv: {
      vec_pushi(code, op);
      break;
    }
    default:
      if (0 <= op && op < (long long int)sizeof(op_to_string)) {
        fprintf(stderr, "Unkown op given. op: %s\n", op_to_string[op]);
      } else {
        fprintf(stderr, "Unkown op given. op: %lld\n", op);
      }
      exit(EXIT_FAILURE);
      break;
    }
  }

  return code;
}
