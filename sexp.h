#ifndef __SEXP_HEADER_INCLUDED__
#define __SEXP_HEADER_INCLUDED__

#include "cplayground.h"

enum {
  float_ty,
  bool_ty,
  string_ty,
  symbol_ty,
  list_ty,
  object_ty,
  quote_ty,
};

typedef struct SexpObject {
  int ty;

  union {
    double float_val;
    bool bool_val;
    sds string_val;
    sds symbol_val;
    Vector *list_val;
    struct SexpObject *object_val;
    struct SexpObject *quote_val;
  };
} SexpObject;

#define GenSexpObjectConstructorProtWithName(T, Name)                          \
  SexpObject *new_SexpObject_##Name(T val);

SexpObject *new_SexpObject(void);

GenSexpObjectConstructorProtWithName(double, float);
GenSexpObjectConstructorProtWithName(bool, bool);
GenSexpObjectConstructorProtWithName(sds, string);
GenSexpObjectConstructorProtWithName(sds, symbol);
GenSexpObjectConstructorProtWithName(Vector *, list);
GenSexpObjectConstructorProtWithName(SexpObject *, object);
GenSexpObjectConstructorProtWithName(SexpObject *, quote);

#define GenGetterOfSexpObjectProtWithName(T, Name)                             \
  T get_##Name##_val(SexpObject *obj);

GenGetterOfSexpObjectProtWithName(double, float);
GenGetterOfSexpObjectProtWithName(bool, bool);
GenGetterOfSexpObjectProtWithName(sds, string);
GenGetterOfSexpObjectProtWithName(sds, symbol);
GenGetterOfSexpObjectProtWithName(Vector *, list);
GenGetterOfSexpObjectProtWithName(SexpObject *, object);
GenGetterOfSexpObjectProtWithName(SexpObject *, quote);

bool equal_SexpObjects(SexpObject *lhs, SexpObject *rhs);

typedef struct {
  SexpObject *parse_result;
  size_t read_len;
} ParseResult;

ParseResult sexp_parseExpr(sds code);
Vector *sexp_parse(sds code);

sds show_sexp_object(SexpObject *obj);
#endif
