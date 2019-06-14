#include "cplayground.h"
#include "sds/sds.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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
  return env;
}

void insert_Env(Env *env, sds name, VMValue *val) {
  avl_insert(env->vars, name, val);
}

static inline VMValue *get_Env(Env *env, sds name) {
  return avl_find(env->vars, name);
}

static inline Vector *keys_Env(Env *env) { return avl_keys(env->vars); }

Env *dup_Env(Env *env) {
  Env *new_env = new_Env();

  Vector *keys = keys_Env(env);
  for (size_t i = 0; i < keys->len; i++) {
    sds key = keys->data[i];
    insert_Env(new_env, key, dup_VMValue(get_Env(env, key)));
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
    printf("stack[%lld] %p : \n", i, e);
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

Vector *vm_compile_SexpObject(SexpObject *obj);

static inline void vm_compile_binary_fun(Opcode op, Vector *v, Vector *ret) {
  assert(v->len == 3);
  SexpObject *e1 = (SexpObject *)v->data[1];
  SexpObject *e2 = (SexpObject *)v->data[2];
  vec_append(ret, vm_compile_SexpObject(e1));
  vec_append(ret, vm_compile_SexpObject(e2));
  vec_pushi(ret, op);
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
      assert(e1->ty == symbol_ty);
      assert(e2->ty == list_ty);

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

      if (arg_names != NULL) {
        for (size_t i = 0; i < arg_names->len; i++) {
          vec_pushi(func_body, OpSetArgFrom);
          vec_push(func_body, arg_names->data[i]);
          vec_pushi(func_body, i);
        }
      }

      for (size_t i = 3; i < v->len; i++) {
        SexpObject *e = (SexpObject *)v->data[i];
        vec_append(func_body, vm_compile_SexpObject(e));
      }
      vec_pushi(func_body, OpReturn);

      VMFunction *vmf = new_VMFunction(e1->symbol_val, func_body, arg_names);
      vec_pushi(ret, OpFuncDef);
      vec_push(ret, new_VMValueWithFunc(vmf));
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
}

static inline int get_builtin(sds name) {
  bool e = avl_exists(builtin_functions, name);
  if (!e) {
    return -1;
  } else {
    return (int)(intptr_t)avl_find(builtin_functions, name);
  }
}

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
    printf("op: %lld, reg: %p, reg->pc: %lld\n", op, reg, reg->pc);
  OP_SELECT:
    switch (op) {
    case OpPop:
      pop_Stack(stack);
      break;
    case OpPush:
      push_Stack(stack, frame->v_ins->data[reg->pc++]);
      break;
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
      double l = get_float_val((SexpObject *)pop_Stack(stack));
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
      printf("def! name: %s, val: %s\n", var_name, show_VMValue(v));
      break;
    }
    case OpGetVar: {
      sds var_name = (sds)frame->v_ins->data[reg->pc++];
      VMValue *v = get_Env(frame->env, var_name);
      push_Stack(stack, v);
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
      insert_Env(frame->env, arg_name, frame->args->data[arg_idx]);
      break;
    }
    case opDumpEnv: {
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
      printf("OpLt");
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
    case opDumpEnv: {
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
      fprintf(stderr, "Unkown op given. op: %lld\n", op);
      exit(EXIT_FAILURE);
      break;
    }
  }
}

void vm_ins_dump(Vector *v_ins) { vm_ins_dump_impl(v_ins, 0); }