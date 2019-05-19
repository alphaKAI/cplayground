#ifndef __SEXP_HEADER_INCLUDED__
#define __SEXP_HEADER_INCLUDED__

#include "cplayground.h"

enum {
  int_ty,
  float_ty,
  bool_ty,
  string_ty,
  array_ty,
  object_ty,
};

typedef struct SexpObject {
  int ty;

  union {
    int int_val;
    double float_val;
    bool bool_val;
    sds string_val;
    Vector *array_val;
    struct SexpObject *object_val;
  };
} SexpObject;

SexpObject *new_SexpObject(void);
SexpObject *new_SexpObject_int(int);
SexpObject *new_SexpObject_float(double);
SexpObject *new_SexpObject_bool(bool);
SexpObject *new_SexpObject_string(sds);
SexpObject *new_SexpObject_array(Vector *);
SexpObject *new_SexpObject_object(SexpObject *);

Vector *sexp_parse(sds code);

int get_int_val(SexpObject *obj);
double get_float_val(SexpObject *obj);
bool get_bool_val(SexpObject *obj);
sds get_string_val(SexpObject *obj);
Vector *get_array_val(SexpObject *obj);
SexpObject *get_object_val(SexpObject *obj);

#endif
