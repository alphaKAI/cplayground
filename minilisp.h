#ifndef __MINILISP_HEADER_INCLUDED__
#define __MINILISP_HEADER_INCLUDED__

#include "cplayground.h"

#define __ENABLE_DIRECT_THREADED_CODE__

typedef struct {
  size_t pc;
} Registers;

enum {
  OpPop,
  OpPush,
  OpAllocLvars,
  OpFreeLvars,
  OpGetLocal,
  OpSetLocal,
  OpSetArgLocal,
  OpAdd,
  OpSub,
  OpMul,
  OpDiv,
  OpMod,
  OpEq,
  OpNeq,
  OpLt,
  OpLeq,
  OpGt,
  OpGeq,
  OpPrint,
  OpPrintln,
  OpJumpRel,
  OpFuncDef,
  OpCall,
  OpReturn,
  OpVarDef,
  OpGetVar,
  OpSetVar,
  OpBranch,
  OpMakeList,
  OpSetArgFrom,
  OpDumpEnv
};

#define FUNC_CACHE_LEN 5

struct VMValue;

typedef struct Env {
  AVLTree *vars;
  struct Env *parent;
  bool copied;
  struct VMValue *cached_funcs[FUNC_CACHE_LEN];
} Env;

static inline int varcmp(void *lhs, void *rhs) {
  char *ls = (char *)lhs;
  char *rs = (char *)rhs;
  int ret = sdscmp(ls, rs);

  if (ret < 0) {
    return -1;
  }
  if (ret > 0) {
    return 1;
  }
  return 0;
}

typedef struct Frame {
  Registers *registers;
  Env *env;
  Vector *args;
  struct Frame *parent;
  Vector *lvars;
  Vector *v_ins;
} Frame;

typedef struct {
  sds name;
  Vector *code;
  Vector *arg_names;
#ifdef __ENABLE_DIRECT_THREADED_CODE__
  void **ops_ptr;
#endif
} VMFunction;

VMFunction *new_VMFunction(sds name, Vector *code, Vector *arg_names);
void free_VMFunction(VMFunction **vmf_ptr);

enum { VValue, VFunc };

typedef struct {
  int ty;
  union {
    SexpObject *val;
    VMFunction *func;
  };
} VMValue;

VMValue *new_VMValue(int ty, void *e);
void free_VMValue(VMValue **v_ptr);
VMValue *dup_VMValue(VMValue *value);
VMValue *new_VMValueWithValue(SexpObject *obj);
VMValue *new_VMValueWithFunc(VMFunction *vmf);

typedef long long int Opcode;

void vm_init(void);
Vector *vm_compile(Vector *parsed);
SexpObject *vm_exec(Vector *);
void vm_ins_dump_impl(Vector *v_ins, size_t depth);
void vm_ins_dump(Vector *v_ins);
Vector *vm_binarize(Vector *v_ins);
Vector *vm_deserialize(Vector *serialized);
#endif
