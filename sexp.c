#include "cplayground.h"
#include "sds/sds.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static size_t nextBracket(sds code) {
  size_t index, leftCount = 1, rightCount;

  while (leftCount != rightCount) {
    if (code[index] == '(') {
      leftCount++;
    }
    if (code[index] == ')') {
      rightCount++;
    }
    ++index;
  }

  return index;
}

SexpObject *new_SexpObject(void) {
  SexpObject *obj = xmalloc(sizeof(SexpObject));
  return obj;
}

#define GenSexpObjectConstructorWithName(T, Name)                              \
  SexpObject *new_SexpObject_##Name(T val) {                                   \
    SexpObject *obj = new_SexpObject();                                        \
    obj->ty = Name##_ty;                                                       \
    obj->Name##_val = val;                                                     \
    return obj;                                                                \
  }

#define GenSexpObjectConstructor(T) GenSexpObjectConstructorWithName(T, T)

GenSexpObjectConstructor(int);
GenSexpObjectConstructorWithName(double, float);
GenSexpObjectConstructorWithName(bool, bool);
GenSexpObjectConstructorWithName(sds, string);
GenSexpObjectConstructorWithName(Vector *, array);
GenSexpObjectConstructorWithName(SexpObject *, object);

#define GenGetterOfSexpObjectWithName(T, Name)                                 \
  T get_##Name##_val(SexpObject *obj) {                                        \
    assert(obj->ty == Name##_ty);                                              \
    return obj->Name##_val;                                                    \
  }

#define GenGetterOfSexpObject(T) GenGetterOfSexpObjectWithName(T, T)

GenGetterOfSexpObject(int);

GenGetterOfSexpObjectWithName(double, float);
GenGetterOfSexpObjectWithName(bool, bool);
GenGetterOfSexpObjectWithName(sds, string);
GenGetterOfSexpObjectWithName(Vector *, array);
GenGetterOfSexpObjectWithName(SexpObject *, object);

Vector *sexp_parse(sds code) {
  Vector *_out = new_vec();

  for (size_t i = 0; i < sdslen(code); ++i) {
    char ch = code[i];

    if (ch == ' ') {
      continue;
    } else {
      if (ch == '(') {
        size_t j = nextBracket(&code[i + 1]);
        sds tmp;
        sdscpylen(tmp, &code[i + 1], j - 1);
        vec_push(_out, new_SexpObject_array(sexp_parse(tmp)));

        i += j;
      } else if (ch == ')') {
        return _out;
      } else {
        if (isdigit(ch) ||
            (i + 1 < sdslen(code) && ch == '-' && isdigit(code[i + 1]))) {
          sds tmp = sdsempty();
          size_t j = i;

          do {
            tmp = sdscatprintf(tmp, "%c", code[j]);
            ++j;
          } while (j < sdslen(code) &&
                   ((code[j] != ' ' && isdigit(code[j])) ||
                    (code[j] == '.' && j + 1 < sdslen(code) &&
                     isdigit(code[j + 1]))));

          vec_push(_out, new_SexpObject_float(parse_double(tmp)));

          i = j - 1;
        } else if (ch == '\"' || ch == '\'') {
          if (ch == '\'' && i + 1 < sdslen(code) && code[i + 1] == '(') {
            size_t j = nextBracket(&code[i + 2]) + 1;
            sds tmp;
            sdscpylen(tmp, &code[i + 2], j - 2);

            vec_push(_out, new_SexpObject_array(sexp_parse(tmp)));

            i += j;
          } else {
            sds tmp = sdsempty();
            size_t j = i + 1;

            while (j < sdslen(code) && code[j] != ch) {
              if (j < sdslen(code)) {
                tmp = sdscatprintf(tmp, "%c", code[j]);
              } else {
                fprintf(stderr, "Syntax Error\n");
                exit(EXIT_FAILURE);
              }

              ++j;
            }

            vec_push(_out, new_SexpObject_string(tmp));
            i = j;
          }
        } else {
          sds tmp = sdsempty();
          size_t j = i;

          while (j < sdslen(code) && code[j] != '\"' && code[j] != '\'' &&
                 code[j] != '(' && code[j] != ')' && code[j] != ' ') {
            tmp = sdscatprintf(tmp, "%c", code[j]);
            ++j;
          }

          if (sdscmp(tmp, "true") == 0) {
            vec_push(_out, new_SexpObject_bool(true));
          } else if (sdscmp(tmp, "false") == 0) {
            vec_push(_out, new_SexpObject_bool(false));
          } else {
            vec_push(_out, new_SexpObject_string(tmp));
          }

          i = j;
        }
      }
    }
  }

  return _out;
}

sds show_sexp_object(SexpObject *obj) {
  sds ret = sdsempty();

  switch (obj->ty) {
  case int_ty:
    ret = sdscatprintf(ret, "%d", obj->int_val);
    break;
  case float_ty:
  case bool_ty:
  case string_ty:
  case array_ty:
  case object_ty:
  default:
    fprintf(stderr, "[ERROR] invalid type SexpObject was given\n");
    exit(EXIT_FAILURE);
  }

  return ret;
}
