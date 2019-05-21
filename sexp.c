#include "cplayground.h"
#include "sds/sds.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

GenSexpObjectConstructorWithName(double, float);
GenSexpObjectConstructorWithName(bool, bool);
GenSexpObjectConstructorWithName(sds, string);
GenSexpObjectConstructorWithName(sds, symbol);
GenSexpObjectConstructorWithName(Vector *, list);
GenSexpObjectConstructorWithName(SexpObject *, object);
GenSexpObjectConstructorWithName(SexpObject *, quote);

#define GenGetterOfSexpObjectWithName(T, Name)                                 \
  T get_##Name##_val(SexpObject *obj) {                                        \
    assert(obj->ty == Name##_ty);                                              \
    return obj->Name##_val;                                                    \
  }

#define GenGetterOfSexpObject(T) GenGetterOfSexpObjectWithName(T, T)

GenGetterOfSexpObjectWithName(double, float);
GenGetterOfSexpObjectWithName(bool, bool);
GenGetterOfSexpObjectWithName(sds, string);
GenGetterOfSexpObjectWithName(sds, symbol);
GenGetterOfSexpObjectWithName(Vector *, list);
GenGetterOfSexpObjectWithName(SexpObject *, object);
GenGetterOfSexpObjectWithName(SexpObject *, quote);

bool equal_SexpObjects(SexpObject *lhs, SexpObject *rhs) {
  if (lhs->ty != rhs->ty) {
    return false;
  }

  switch (lhs->ty) {
  case float_ty:
    return lhs->float_val == rhs->float_val;
  case bool_ty:
    return lhs->bool_val == rhs->bool_val;
  case string_ty:
    return strcmp(lhs->string_val, rhs->string_val) == 0;
  case symbol_ty:
    return strcmp(lhs->symbol_val, rhs->symbol_val) == 0;
  case list_ty: {
    Vector *lv = lhs->list_val;
    Vector *rv = rhs->list_val;

    if (lv->len != rv->len) {
      return false;
    }

    for (size_t i = 0; i < lv->len; i++) {
      SexpObject *le = lv->data[i];
      SexpObject *re = rv->data[i];

      if (!equal_SexpObjects(le, re)) {
        return false;
      }
    }

    return true;
  }
  case object_ty: {
    SexpObject *li = lhs->object_val;
    SexpObject *ri = rhs->object_val;
    return equal_SexpObjects(li, ri);
  }
  case quote_ty: {
    SexpObject *li = lhs->quote_val;
    SexpObject *ri = rhs->quote_val;
    return equal_SexpObjects(li, ri);
  }
  default:
    fprintf(stderr, "Unknown type was given.\n");
    exit(EXIT_FAILURE);
  }
}

static size_t next_bracket(sds code, size_t left_offset) {
  size_t index = 0, left_count = left_offset, right_count = 0;

  while (left_count != right_count) {
    if (code[index] == '(') {
      left_count++;
    }
    if (code[index] == ')') {
      right_count++;
    }
    ++index;
  }

  return index;
}

static ParseResult new_ParseResult(void) {
  return (ParseResult){.parse_result = NULL, .read_len = 0};
}

ParseResult parse_list(sds str) {
  ParseResult result = new_ParseResult();
  Vector *list = new_vec();
  size_t i = 1; // skip first paren '('
  size_t next_bracket_idx = next_bracket(&str[1], 1);

  sds contents = sdsempty();
  sdscpylen(contents, &str[1], next_bracket_idx - 1);

  size_t j = 0;
  ParseResult tmp_result = {.parse_result = NULL};
  for (; j < sdslen(contents);) {
    tmp_result = sexp_parseExpr(&contents[j]);
    if (tmp_result.parse_result != NULL) {
      vec_push(list, tmp_result.parse_result);
      tmp_result.parse_result = NULL;
    }
    j += tmp_result.read_len;
  }
  i += j;
  assert(str[i] == ')');
  i++; // skip final ')'

  sdsfree(contents);

  result.parse_result = new_SexpObject_list(list);
  result.read_len = i;

  return result;
}

ParseResult skip_line(sds str) {
  size_t str_len = strlen(str);
  size_t i = 0;
  for (; i < str_len && str[i] != '\n'; i++)
    ;

  return (ParseResult){.read_len = i};
}

#define DOT_NEXT_IS_NUMBER(str, str_len, i)                                    \
  (str[i] == '.' && i + 1 < str_len && isdigit(str[i + 1]))

ParseResult parse_number(sds str) {
  ParseResult result = new_ParseResult();
  size_t i = 0;
  size_t str_len = strlen(str);
  size_t first = 0;

  if (str[0] == '-') {
    i++;
    first = 1;
  }
  for (;
       i < str_len && (isdigit(str[i]) || DOT_NEXT_IS_NUMBER(str, str_len, i));
       i++)
    ;

  sds tmp = sdsempty();
  sdscpylen(tmp, &str[first], i - first);
  double val = parseDouble(tmp);
  sdsfree(tmp);

  result.parse_result = new_SexpObject_float(val);
  result.read_len = i;

  return result;
}

const char symbol_chars[] = "~!@#$%^&*-_=+:/?<>";

ParseResult parse_symbol(sds str) {
  ParseResult result = new_ParseResult();
  size_t str_len = strlen(str);
  size_t i = 0;

  for (; i < str_len && (isalpha(str[i]) || strchr(symbol_chars, str[i])); i++)
    ;

  sds tmp = sdsempty();
  sdscpylen(tmp, str, i);
  result.parse_result = new_SexpObject_symbol(tmp);
  result.read_len = i;

  return result;
}

ParseResult parse_string(sds str) {
  ParseResult result = new_ParseResult();
  size_t str_len = strlen(str);
  size_t i = 1;

  for (; i < str_len && str[i] != '\"'; i++)
    ;

  sds tmp = sdsempty();
  sdscpylen(tmp, &str[1], i - 1);
  result.parse_result = new_SexpObject_string(tmp);
  result.read_len = i + 1;

  return result;
}

ParseResult parse_quote(sds str) {
  ParseResult result = new_ParseResult();

  ParseResult expr = sexp_parseExpr(&str[1]);
  result.parse_result = new_SexpObject_quote(expr.parse_result);
  result.read_len = 1 + expr.read_len;

  return result;
}

ParseResult sexp_parseExpr(sds code) {
  size_t code_len = strlen(code);
  ParseResult result = new_ParseResult();
  size_t i = 0;
  for (; i < code_len;) {
    char c = code[i];

    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
      i++;
      continue;
    }

    if (c == ';') {
      i += skip_line(&code[i]).read_len;
      continue;
    }

    if (isdigit(c)) {
      result = parse_number(&code[i]);
      result.read_len += i;
      return result;
    }

    if (c == '-' && i + 1 < code_len && isdigit(code[i + 1])) {
      result = parse_number(&code[i]);
      result.read_len += i;
      return result;
    }

    if (isalpha(c) || strchr(symbol_chars, c)) {
      result = parse_symbol(&code[i]);
      result.read_len += i;
      return result;
    }

    if (c == '\"') {
      result = parse_string(&code[i]);
      result.read_len += i;
      return result;
    }

    if (c == '(') {
      result = parse_list(&code[i]);
      result.read_len += i;
      return result;
    }

    if (c == '\'') {
      result = parse_quote(&code[i]);
      result.read_len += i;
      return result;
    }
  }

  result.read_len = i;
  return result;
}

Vector *sexp_parse(sds code) {
  Vector *ret = new_vec();

  for (size_t i = 0; i < sdslen(code);) {
    ParseResult result = sexp_parseExpr(&code[i]);
    vec_push(ret, result.parse_result);
    i += result.read_len;
  }

  return ret;
}

sds show_sexp_object(SexpObject *obj) {
  sds ret = sdsempty();

  switch (obj->ty) {
  case float_ty:
    ret = sdscatprintf(ret, "%f", obj->float_val);
    break;
  case bool_ty:
    ret = sdscatprintf(ret, "%s", obj->bool_val ? "true" : "false");
    break;
  case string_ty:
    ret = sdscatprintf(ret, "\"%s\"", obj->string_val);
    break;
  case symbol_ty:
    ret = sdscatprintf(ret, "%s", obj->string_val);
    break;
  case list_ty: {
    Vector *elems = obj->list_val;
    Vector *elems_str = new_vec();
    for (size_t i = 0; i < elems->len; i++) {
      vec_push(elems_str, show_sexp_object((SexpObject *)elems->data[i]));
    }
    ret = sdscatprintf(ret, "(%s)", vecstrjoin(elems_str, " "));
    break;
  }
  case object_ty: {
    SexpObject *iobj = obj->object_val;
    ret = sdscatprintf(ret, "(%s)", show_sexp_object(iobj));
    break;
  }
  case quote_ty: {
    SexpObject *iobj = obj->quote_val;
    ret = sdscatprintf(ret, "'%s", show_sexp_object(iobj));
    break;
  }
  default:
    fprintf(stderr, "[ERROR] invalid type SexpObject was given\n");
    exit(EXIT_FAILURE);
  }

  return ret;
}