/* This file was automatically generated by CasADi.
   The CasADi copyright holders make no ownership claim of its contents. */
#ifdef __cplusplus
extern "C" {
#endif

/* How to prefix internal symbols */
#ifdef CODEGEN_PREFIX
  #define NAMESPACE_CONCAT(NS, ID) _NAMESPACE_CONCAT(NS, ID)
  #define _NAMESPACE_CONCAT(NS, ID) NS ## ID
  #define CASADI_PREFIX(ID) NAMESPACE_CONCAT(CODEGEN_PREFIX, ID)
#else
  #define CASADI_PREFIX(ID) engine_ls_cost_ ## ID
#endif

#include <math.h>

#ifndef casadi_real
#define casadi_real double
#endif

#ifndef casadi_int
#define casadi_int int
#endif

/* Add prefix to internal symbols */
#define casadi_copy CASADI_PREFIX(copy)
#define casadi_f0 CASADI_PREFIX(f0)
#define casadi_fill CASADI_PREFIX(fill)
#define casadi_s0 CASADI_PREFIX(s0)
#define casadi_s1 CASADI_PREFIX(s1)
#define casadi_s2 CASADI_PREFIX(s2)
#define casadi_s3 CASADI_PREFIX(s3)

/* Symbol visibility in DLLs */
#ifndef CASADI_SYMBOL_EXPORT
  #if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
    #if defined(STATIC_LINKED)
      #define CASADI_SYMBOL_EXPORT
    #else
      #define CASADI_SYMBOL_EXPORT __declspec(dllexport)
    #endif
  #elif defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
    #define CASADI_SYMBOL_EXPORT __attribute__ ((visibility ("default")))
  #else
    #define CASADI_SYMBOL_EXPORT
  #endif
#endif

static const casadi_int casadi_s0[6] = {0, 2, 3, 4, 6, 7};
static const casadi_int casadi_s1[10] = {6, 1, 0, 6, 0, 1, 2, 3, 4, 5};
static const casadi_int casadi_s2[11] = {7, 1, 0, 7, 0, 1, 2, 3, 4, 5, 6};
static const casadi_int casadi_s3[18] = {6, 7, 0, 2, 3, 4, 5, 6, 7, 8, 4, 5, 2, 3, 4, 5, 0, 1};

void casadi_copy(const casadi_real* x, casadi_int n, casadi_real* y) {
  casadi_int i;
  if (y) {
    if (x) {
      for (i=0; i<n; ++i) *y++ = *x++;
    } else {
      for (i=0; i<n; ++i) *y++ = 0.;
    }
  }
}

void casadi_fill(casadi_real* x, casadi_int n, casadi_real alpha) {
  casadi_int i;
  if (x) {
    for (i=0; i<n; ++i) *x++ = alpha;
  }
}

/* engine_ls_cost:(i0[6])->(o0[7],o1[6x7,8nz]) */
static int casadi_f0(const casadi_real** arg, casadi_real** res, casadi_int* iw, casadi_real* w, void* mem) {
  casadi_int i;
  casadi_real *rr, *ss;
  const casadi_int *cii;
  const casadi_real *cs;
  casadi_real w0, *w1=w+1, w2, *w3=w+6, *w4=w+8, *w5=w+16, *w6=w+21, w7, *w8=w+25, *w9=w+31, *w11=w+37;
  /* #0: @0 = 1000 */
  w0 = 1000.;
  /* #1: @1 = input[0][1] */
  casadi_copy(arg[0] ? arg[0]+2 : 0, 4, w1);
  /* #2: @2 = @1[2] */
  for (rr=(&w2), ss=w1+2; ss!=w1+3; ss+=1) *rr++ = *ss;
  /* #3: @0 = (@0*@2) */
  w0 *= w2;
  /* #4: @2 = @1[3] */
  for (rr=(&w2), ss=w1+3; ss!=w1+4; ss+=1) *rr++ = *ss;
  /* #5: @0 = (@0*@2) */
  w0 *= w2;
  /* #6: output[0][0] = @0 */
  if (res[0]) res[0][0] = w0;
  /* #7: output[0][1] = @1 */
  if (res[0]) casadi_copy(w1, 4, res[0]+1);
  /* #8: @3 = input[0][0] */
  casadi_copy(arg[0], 2, w3);
  /* #9: output[0][2] = @3 */
  if (res[0]) casadi_copy(w3, 2, res[0]+5);
  /* #10: @4 = zeros(6x7,8nz) */
  casadi_fill(w4, 8, 0.);
  /* #11: @0 = @1[3] */
  for (rr=(&w0), ss=w1+3; ss!=w1+4; ss+=1) *rr++ = *ss;
  /* #12: @2 = 1000 */
  w2 = 1000.;
  /* #13: @5 = ones(6x1,5nz) */
  casadi_fill(w5, 5, 1.);
  /* #14: {@3, @6} = vertsplit(@5) */
  casadi_copy(w5, 2, w3);
  casadi_copy(w5+2, 3, w6);
  /* #15: @7 = @6[2] */
  for (rr=(&w7), ss=w6+2; ss!=w6+3; ss+=1) *rr++ = *ss;
  /* #16: @7 = (@2*@7) */
  w7  = (w2*w7);
  /* #17: @0 = (@0*@7) */
  w0 *= w7;
  /* #18: @8 = vertcat(@0, @6, @3) */
  rr=w8;
  *rr++ = w0;
  for (i=0, cs=w6; i<3; ++i) *rr++ = *cs++;
  for (i=0, cs=w3; i<2; ++i) *rr++ = *cs++;
  /* #19: @9 = @8[:6] */
  for (rr=w9, ss=w8+0; ss!=w8+6; ss+=1) *rr++ = *ss;
  /* #20: (@4[0, 2, 3, 4, 6, 7] = @9) */
  for (cii=casadi_s0, rr=w4, ss=w9; cii!=casadi_s0+6; ++cii, ++ss) if (*cii>=0) rr[*cii] = *ss;
  /* #21: @0 = @1[2] */
  for (rr=(&w0), ss=w1+2; ss!=w1+3; ss+=1) *rr++ = *ss;
  /* #22: @2 = (@2*@0) */
  w2 *= w0;
  /* #23: @0 = ones(6x1,1nz) */
  w0 = 1.;
  /* #24: {NULL, @7} = vertsplit(@0) */
  w7 = w0;
  /* #25: @0 = @7[0] */
  for (rr=(&w0), ss=(&w7)+0; ss!=(&w7)+1; ss+=1) *rr++ = *ss;
  /* #26: @2 = (@2*@0) */
  w2 *= w0;
  /* #27: @10 = zeros(2x1,0nz) */
  /* #28: @3 = vertcat(@2, @7, @10) */
  rr=w3;
  *rr++ = w2;
  *rr++ = w7;
  /* #29: @11 = @3[:2] */
  for (rr=w11, ss=w3+0; ss!=w3+2; ss+=1) *rr++ = *ss;
  /* #30: (@4[1:9:4] = @11) */
  for (rr=w4+1, ss=w11; rr!=w4+9; rr+=4) *rr = *ss++;
  /* #31: output[1][0] = @4 */
  casadi_copy(w4, 8, res[1]);
  return 0;
}

CASADI_SYMBOL_EXPORT int engine_ls_cost(const casadi_real** arg, casadi_real** res, casadi_int* iw, casadi_real* w, void* mem){
  return casadi_f0(arg, res, iw, w, mem);
}

CASADI_SYMBOL_EXPORT void engine_ls_cost_incref(void) {
}

CASADI_SYMBOL_EXPORT void engine_ls_cost_decref(void) {
}

CASADI_SYMBOL_EXPORT casadi_int engine_ls_cost_n_in(void) { return 1;}

CASADI_SYMBOL_EXPORT casadi_int engine_ls_cost_n_out(void) { return 2;}

CASADI_SYMBOL_EXPORT const char* engine_ls_cost_name_in(casadi_int i){
  switch (i) {
    case 0: return "i0";
    default: return 0;
  }
}

CASADI_SYMBOL_EXPORT const char* engine_ls_cost_name_out(casadi_int i){
  switch (i) {
    case 0: return "o0";
    case 1: return "o1";
    default: return 0;
  }
}

CASADI_SYMBOL_EXPORT const casadi_int* engine_ls_cost_sparsity_in(casadi_int i) {
  switch (i) {
    case 0: return casadi_s1;
    default: return 0;
  }
}

CASADI_SYMBOL_EXPORT const casadi_int* engine_ls_cost_sparsity_out(casadi_int i) {
  switch (i) {
    case 0: return casadi_s2;
    case 1: return casadi_s3;
    default: return 0;
  }
}

CASADI_SYMBOL_EXPORT int engine_ls_cost_work(casadi_int *sz_arg, casadi_int* sz_res, casadi_int *sz_iw, casadi_int *sz_w) {
  if (sz_arg) *sz_arg = 4;
  if (sz_res) *sz_res = 4;
  if (sz_iw) *sz_iw = 0;
  if (sz_w) *sz_w = 39;
  return 0;
}


#ifdef __cplusplus
} /* extern "C" */
#endif