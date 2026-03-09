#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 *
 * It's a template-based library (like templates in `C++`, but implemented through `C` macros) and it
 * gives users the tools to create and manage `Result` types similar to those in `Rust`, including
 * pattern matching.
 * 
 * The library works with every standard starting from C89 with some caveats:
 * In C89, some features are unavailable because they depend on language features introduced in
 * later standards. The library still compiles, and through `conditional guards` these features are 
 * excluded from compilation.
 * 
 * > [!WARNING]
 * > This library uses some `GCC Extensions` so check your compiler for that.
 * 
 * ```c
 * #include <stdio.h>
 * 
 * #define CRESULT_NO_PREFIX
 * #include "Cresult.h"
 * 
 * typedef TEMPLATE(int, char*) CResult_int_char;
 * typedef TEMPLATE(int, struct H{int a;}) CResult_int_custom_c;
 * 
 * #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
 * static RETURN_TYPE(CResult_int_char) foo(int i){
 *   #define OK(val) T_OK(CResult_int_char, val)
 * 
 *   if (i==2) {
 *     return OK(1);
 *   }else{
 *     return T_ERR(CResult_int_char,"invalid value");
 *   }
 * }
 * #else
 * static RETURN_TYPE(CResult_int_char) foo(int i){
 *   CResult_int_char res = {0};
 * 
 *   if (i==2) {
 *     SET_OK(res, 1);
 *   }else{
 *     SET_ERR(res, "invalid value");
 *   }
 * 
 *   return res;
 * }
 * #endif
 * 
 * RETURN_TYPE(CResult_int_char) try_f(int j){
 *   CResult_int_char res = {0};
 * 
 *   if (j==1) SET_OK(res, 2);
 *   else SET_ERR(res, "try failed");
 * 
 *   return res;
 * }
 * 
 * RETURN_TYPE(CResult_int_char) bar(int i){
 *   CResult_int_char res = {0};
 *   int z = TRY(try_f(i));
 *   (void)z;
 * 
 *   SET_OK(res,1);
 * 
 *   return res;
 * }
 * 
 * int main(void)
 * {
 *   CResult_int_char res;
 * 
 *   printf("foo(2)\n");
 *   res= foo(2);
 * 
 *   printf("manual\n");
 *   if (IS_OK(res)){
 *     printf("res: %d\n", OK_VAL(res));
 *   }else{
 *     printf("res: %s\n", ERR_VAL(res));
 *   }
 * 
 *   #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
 *   printf("with full match\n");
 *   FULL_MATCH(res, res_val, printf("res: %d\n", res_val), printf("res: %s\n", res_val));
 * 
 *   printf("with ok match\n");
 *   OK_MATCH(res, ok_val, printf("res: %d\n", ok_val));
 * 
 *   printf("with err match\n");
 *   ERR_MATCH(res, err_val, printf("res: %s\n", err_val));
 *   #endif
 * 
 * 
 *   printf("--------------------------\n");
 *   printf("foo(5)\n");
 *   res = foo(5);
 * 
 *   printf("manual\n");
 *   if (IS_OK(res)){
 *     printf("res: %d\n", OK_VAL(res));
 *   }else{
 *     printf("res: %s\n", ERR_VAL(res));
 *   }
 * 
 *   printf("with full match\n");
 *   FULL_MATCH(res, res_val, printf("res: %d\n", res_val), printf("res: %s\n", res_val));
 * 
 *   printf("with ok match\n");
 *   OK_MATCH(res, ok_val, printf("res: %d\n", ok_val));
 * 
 *   printf("with err match\n");
 *   ERR_MATCH(res, err_val, printf("res: %s\n", err_val));
 * 
 *   printf("call to bar(5)\n");
 *   FULL_MATCH(bar(5), val, printf("ok val: %d\n", val), printf("error: %s\n",val));
 *   printf("call to bar(1)\n");
 *   FULL_MATCH(bar(1), val, printf("ok val: %d\n", val), printf("error: %s\n",val));
 * 
 *   return 0;
 * }
 * 
 * ```
 * 
 * ## Usage
 * 
 * ### Defining a Result
 * 
 * > To make the code less verbose, a namespace system has been defined.
 * > By defining `CRESULT_NO_PREFIX` before the include, you remove the prefix `CRESULT_`. 
 * > It works in the same way as `using namespace X` in `C++`.
 * 
 * To use a Result, first you need to define one. To do that, you specialize the template
 * (`#define CRESULT_TEMPLATE(T_OK, T_ERR)`) in the following way:
 * 
 * ```c
 * typedef TEMPLATE(int, char*) CResult_int_char;
 * typedef TEMPLATE(int, struct H{int a;}) CResult_int_custom_c;
 * ```
 * 
 * - T_OK : It's the type for the result in case of success
 * - T_ERR : It's the type for the result in case of failure
 * 
 * There is no real constraint on the type: they can be anything from pointers, to structs,
 * to primitive types.
 * 
 * After a Result has been designed, it can be used freely in the code.
 * 
 * ### Returning a Result
 * 
 * To use it in a function you just make the function return a Result type in this way:
 * ```c
 * #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
 * RETURN(CResult_int_char) foo(int i){
 *   if (i == 2) {
 *     return NEW_OK(CResult_int_char,1);
 *   }else{
 *     return NEW_ERR(CResult_int_char, "invalid value");
 *   }
 * }
 * #else
 * CResult_int_char foo(int i){
 *   CResult_int_char res = {0};
 * 
 *   if (i == 2) {
 *     SET_OK(res, 1);
 *   }else{
 *     SET_ERR(res, "invalid value");
 *   }
 * 
 *   return res;
 * }
 * #endif
 * ```
 * 
 * In this snippet of code, the function foo() returns a `CResult_int_char` with `Ok(1)` if i == 2 and
 * `Err("invalid value")` otherwise. 
 * The `RETURN` macro enforces that the Result value cannot be ignored by applying the
 * `warn_unused_result` attribute to the function's return type. (Like in Rust)
 * 
 * > [!IMPORTANT]
 * > `NEW_OK` and `NEW_ERR` require at least C99. In case this is not possible, you can do that
 * > manually by using `SET_OK` and `SET_ERR` as in the example above.
 * 
 * ### Consuming a Result
 * 
 * To inspect and use a Result type there are different approaches. They can be split into two
 * categories:
 * 
 * - Manual
 * - Automatic
 * 
 * #### Manual
 * 
 * In the manual approach, the user is responsible for determining whether the Result value is an
 * `Ok()` or an `Err()` by using the command `IS_OK(self)`.
 * After that the user can access the data by using the following commands:
 * 
 * - for Ok values: `OK_VAL(self)`
 * - for Err values: `ERR_VAL(self)`
 * 
 * #### Automatic
 * 
 * In the automatic approach, the user can use a set of commands to pattern match the appropriate
 * value and execute different expressions based on the context. Example below:
 * 
 * ```c
 *   printf("with full match\n");
 *   FULL_MATCH(res, res_val, printf("res: %d\n", res_val), printf("res: %s\n", res_val));
 * 
 *   printf("with ok match\n");
 *   OK_MATCH(res, ok_val, printf("res: %d\n", ok_val));
 * 
 *   printf("with err match\n");
 *   ERR_MATCH(res, err_val, printf("res: %s\n", err_val));
 * 
 *   RETURN_TYPE(CResult_int_char) bar(int i){
 *       CResult_int_char res = {0};
 *       int z = TRY(try_f(i));
 *   }
 * ```
 * 
 * The `TRY` command allow the evaluation of a function and if it returned a `Err(x)` than that error
 * is returned by the function itself else it is stored in a lvalue like. 
 * This is similar to the `?` of Rust.
 *
 */

#if !defined(__GNUC__) && !defined(__clang__)
#error unsupported compiler
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define CRESULT_C99
#endif

#if defined(CRESULT_C99)
#include <stdbool.h>
#else
#define _Bool unsigned char
#define true 1
#define false 0
#endif

#define CRESULT_APPEND_RAW(x, y) x ## y
#define CRESULT_APPEND_2(x, y) CRESULT_APPEND_RAW(x,y)

#define _CRESULT_VERSION_MAJOR 02
#define _CRESULT_VERSION_MINOR 00
#define _CRESULT_VERSION_ CRESULT_APPEND_2(_CRESULT_VERSION_MAJOR,_CRESULT_VERSION_MINOR)

/*
 * Template definition of a CResult type.
 * @param T_OK: type of the data in case of success
 * @param T_ERR: type of the data in case of failure
 *
 * example usage:
 * typedef TEMPLATE(int, char*) CResult_int_char;
 * typedef TEMPLATE(int, struct H{int a;}) CResult_int_custom_c;
 *
 */
#define CRESULT_TEMPLATE(T_OK, T_ERR)                                                             \
struct{                                                                                           \
  _Bool _ok;                                                                                      \
  union{                                                                                          \
    T_OK _ok_val;                                                                                 \
    T_ERR _err_val;                                                                               \
  }value;                                                                                         \
}

#if defined(CRESULT_C99)
/*
 * Return a CResult type ,already defined, in case of success:
 * @param T: Specialization Type of the CResult
 * @param ok_val: value of the result
 */
#define CRESULT_T_OK(T, ok_val)       ((const T) {._ok = true, .value._ok_val = ok_val})

/*
 * Return a CResult type ,already defined, in case of failure:
 * @param T: Specialization Type of the CResult
 * @param err_val: value of the error
 */
#define CRESULT_T_ERR(T, err_val)     ((const T) {._ok = false, .value._err_val= err_val})
#endif

/*
 * Complete pattern match of an already defined specialized CResult:
 * @param self: instance of the specialized CResult
 * @param res_val: name of the result/error that can be used in the user expressions
 * @param ok_exp: expression executed if self represents a successful result
 * @param err_exp: expression executed if self represents a failure error
 */
#define CRESULT_FULL_MATCH(self, res_val, ok_exp, err_exp)                                        \
  do{                                                                                             \
    const __typeof__((self)) c_self = (self);                                                     \
    if(c_self._ok){                                                                               \
      __typeof__(c_self.value._ok_val) res_val = c_self.value._ok_val;                            \
      do{ok_exp;}while(0);                                                                      \
    }else{                                                                                        \
      __typeof__(c_self.value._err_val) res_val = c_self.value._err_val;                          \
      do{err_exp;}while(0);                                                                     \
    }                                                                                             \
  }while(0)


/*
 * Partial pattern match that executes if the specialized CResult represents a successful value
 * @param self: instance of the specialized CResult
 * @param ok_val: name of the result that can be used in the user expressions
 * @param ok_exp: expression executed if self represents a result
 */
#define CRESULT_OK_MATCH(self, ok_val, ok_exp)                                                    \
  do{                                                                                             \
    const __typeof__((self)) c_self = (self);                                                     \
    if(c_self._ok){                                                                               \
      __typeof__(c_self.value._ok_val) ok_val = c_self.value._ok_val;                             \
      do{ok_exp;}while(0);                                                                      \
    }                                                                                             \
  }while(0)

/*
 * Partial pattern match that executes if the specialized CResult represents an error
 * @param self: instance of the specialized CResult
 * @param err_val: name of the error that can be used in the user expressions
 * @param err_exp: expression executed if self represents an error 
 */
#define CRESULT_ERR_MATCH(self, err_val, err_exp)                                                 \
  do{                                                                                             \
    const __typeof__((self)) c_self = (self);                                                     \
    if(!c_self._ok){                                                                              \
      __typeof__(c_self.value._err_val) err_val = c_self.value._err_val;                          \
      do{err_exp;}while(0);                                                                     \
    }                                                                                             \
  }while(0)

/*
 * Set an existing specialized CResult as successful with a specific result
 * @param self: instance of the specialized CResult
 * @param ok_val: user value to set
 */
#define CRESULT_SET_OK(self, ok_val)                                                              \
  do{                                                                                             \
    __typeof__((self)) *p_self = &(self);                                                         \
    p_self->_ok=true;                                                                             \
    p_self->value._ok_val = (ok_val);                                                             \
  }while(0)

/*
 * Set an existing specialized CResult as failed with a specific error
 * @param self: instance of the specialized CResult
 * @param err_val: user error to set
 */
#define CRESULT_SET_ERR(self, err_val)                                                            \
  do{                                                                                             \
    __typeof__((self)) *p_self = &(self);                                                         \
    p_self->_ok=false;                                                                            \
    p_self->value._err_val = (err_val);                                                           \
  }while(0)

/*
 * Check if an existing specialized CResult has a value
 * @param self: instance of the specialized CResult
 * RETURN_TYPE: true if self has a value, false otherwise
 */
#define CRESULT_IS_OK(self)             ((self)._ok)

/*
 * Check if an existing specialized CResult has an error
 * @param self: instance of the specialized CResult
 * RETURN_TYPE: true if self has an error, false otherwise
 */
#define CRESULT_IS_ERR(self)            (!(self)._ok)

/*
 * UNCHECKED RETURN_TYPE the value of an existing specialized CResult.
 * @param self: instance of the specialized CResult
 * IF self has an error, this results in UNDEFINED BEHAVIOUR
 */
#define CRESULT_OK_VAL(self)            ((self).value._ok_val)

/*
 * UNCHECKED RETURN_TYPE the error of an existing specialized CResult.
 * @param self: instance of the specialized CResult
 * IF self has a value, this results in UNDEFINED BEHAVIOUR
 */
#define CRESULT_ERR_VAL(self)           ((self).value._err_val)

/*
 * TRY the function and checks its return value by evaluating the function once.
 * If the return value is Specialization of CResult it will compile otherwise it won't.
 * If the return value is Ok(x) then the value in the Result is returned to the user.
 * If the return value is Err(x) then Err(x) is returned;
 *
 * @param exp: function to evaluate
 */
#define CRESULT_TRY(fun)                                                                          \
     ({                                                                                           \
      __typeof__ ((fun)) __tmp_res__ = (fun);                                                     \
      if(!CRESULT_IS_OK(__tmp_res__)){                                                            \
        return __tmp_res__;                                                                       \
      }                                                                                           \
      CRESULT_OK_VAL(__tmp_res__);                                                                \
     })

/*
 * Set the return type of a function to an already defined specialized CResult.
 * When set, the return value of the function cannot be ignored.
 */
#define CRESULT_RETURN(self)            __attribute__((warn_unused_result)) self

/*
 * Remove the namespace guard of the library
 */

#ifdef CRESULT_NO_PREFIX
#define TEMPLATE(T_OK, T_ERR)                           CRESULT_TEMPLATE(T_OK, T_ERR)

#define SET_OK(self, ok_val)                            CRESULT_SET_OK(self, ok_val)
#define SET_ERR(self, err_val)                          CRESULT_SET_ERR(self, err_val)

#define IS_OK(self)                                     CRESULT_IS_OK(self)
#define IS_ERR(self)                                    CRESULT_IS_ERR(self)

#define OK_VAL(self)                                    CRESULT_OK_VAL(self)
#define ERR_VAL(self)                                   CRESULT_ERR_VAL(self)

#define TRY(exp)                                        CRESULT_TRY(exp)
#define RETURN_TYPE(self)                                    CRESULT_RETURN(self)

#define FULL_MATCH(self, res_val, ok_exp, err_exp)      CRESULT_FULL_MATCH(self, res_val, ok_exp, err_exp)

#define OK_MATCH(self, ok_val, ok_exp)                  CRESULT_OK_MATCH(self, ok_val, ok_exp)
#define ERR_MATCH(self, err_val, err_exp)               CRESULT_ERR_MATCH(self, err_val, err_exp)

#if defined(CRESULT_C99)
#define T_OK(T, ok_val)                                 CRESULT_T_OK(T, ok_val)
#define T_ERR(T, err_val)                               CRESULT_T_ERR(T, err_val)
#endif

#endif /* CRESULT_NO_PREFIX */
