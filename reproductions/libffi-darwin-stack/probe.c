#include <ffi.h>

#include <stdio.h>
#include <string.h>

#if !defined(__APPLE__) || (!defined(__arm64__) && !defined(__aarch64__))
int
main(void)
{
  fprintf(stderr, "unsupported: this probe requires native macOS arm64\n");
  return 2;
}
#else

typedef struct
{
  float f1;
  float f2;
  int i;
} mixed_struct;

enum { EXPECTED_RESULT = 1, FAILURE_RESULT = 0 };

static ffi_cif cif;
static ffi_type mixed_type;
static ffi_type *mixed_elements[4];
static ffi_type *arg_types[12];
static int native_arguments_match;
static int closure_arguments_match;

static int
matches(long a1, long a2, long a3, long a4, long a5, long a6, long a7,
        long a8, int before, const mixed_struct *value, int after)
{
  return a1 == 1 && a2 == 2 && a3 == 3 && a4 == 4 && a5 == 5 &&
    a6 == 6 && a7 == 7 && a8 == 8 && before == 9 &&
    value->f1 == 1.5f && value->f2 == 2.5f && value->i == 17 && after == 11;
}

static void
print_arguments(const char *label, long a1, long a2, long a3, long a4,
                long a5, long a6, long a7, long a8, int before,
                const mixed_struct *value, int after)
{
  printf("%s observed: longs=[%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld], "
         "before=%d, mixed={f1=%a,f2=%a,i=%d}, after=%d\n",
         label, a1, a2, a3, a4, a5, a6, a7, a8, before,
         (double) value->f1, (double) value->f2, value->i, after);
}

static void
print_mismatches(const char *label, long a1, long a2, long a3, long a4,
                 long a5, long a6, long a7, long a8, int before,
                 const mixed_struct *value, int after)
{
  long observed_longs[8] = { a1, a2, a3, a4, a5, a6, a7, a8 };
  int i;

  for (i = 0; i < 8; ++i)
    if (observed_longs[i] != i + 1)
      printf("%s mismatch: a%d expected=%d observed=%ld\n",
             label, i + 1, i + 1, observed_longs[i]);
  if (before != 9)
    printf("%s mismatch: before expected=9 observed=%d\n", label, before);
  if (value->f1 != 1.5f)
    printf("%s mismatch: mixed.f1 expected=%a observed=%a\n",
           label, (double) 1.5f, (double) value->f1);
  if (value->f2 != 2.5f)
    printf("%s mismatch: mixed.f2 expected=%a observed=%a\n",
           label, (double) 2.5f, (double) value->f2);
  if (value->i != 17)
    printf("%s mismatch: mixed.i expected=17 observed=%d\n",
           label, value->i);
  if (after != 11)
    printf("%s mismatch: after expected=11 observed=%d\n", label, after);
}

static int __attribute__((noinline))
target(long a1, long a2, long a3, long a4, long a5, long a6, long a7,
       long a8, int before, mixed_struct value, int after)
{
  print_arguments("native target", a1, a2, a3, a4, a5, a6, a7, a8,
                  before, &value, after);
  print_mismatches("native target", a1, a2, a3, a4, a5, a6, a7, a8,
                   before, &value, after);
  native_arguments_match = matches(a1, a2, a3, a4, a5, a6, a7, a8,
                                   before, &value, after);
  return native_arguments_match ? EXPECTED_RESULT : FAILURE_RESULT;
}

static void
closure_handler(ffi_cif *unused_cif, void *result, void **args,
                void *unused_userdata)
{
  mixed_struct *value = (mixed_struct *) args[9];
  (void) unused_cif;
  (void) unused_userdata;

  print_arguments("closure callback", *(long *) args[0], *(long *) args[1],
                  *(long *) args[2], *(long *) args[3], *(long *) args[4],
                  *(long *) args[5], *(long *) args[6], *(long *) args[7],
                  *(int *) args[8], value, *(int *) args[10]);
  print_mismatches("closure callback", *(long *) args[0], *(long *) args[1],
                   *(long *) args[2], *(long *) args[3], *(long *) args[4],
                   *(long *) args[5], *(long *) args[6], *(long *) args[7],
                   *(int *) args[8], value, *(int *) args[10]);
  closure_arguments_match = matches(*(long *) args[0], *(long *) args[1],
                                     *(long *) args[2], *(long *) args[3],
                                     *(long *) args[4], *(long *) args[5],
                                     *(long *) args[6], *(long *) args[7],
                                     *(int *) args[8], value,
                                     *(int *) args[10]);
  *(ffi_arg *) result = (ffi_arg) (closure_arguments_match
                                  ? EXPECTED_RESULT : FAILURE_RESULT);
}

typedef int (*closure_fn)(long, long, long, long, long, long, long, long,
                          int, mixed_struct, int);

static int
prepare_cif(void)
{
  mixed_type.size = 0;
  mixed_type.alignment = 0;
  mixed_type.type = FFI_TYPE_STRUCT;
  mixed_type.elements = mixed_elements;
  mixed_elements[0] = &ffi_type_float;
  mixed_elements[1] = &ffi_type_float;
  mixed_elements[2] = &ffi_type_sint;
  mixed_elements[3] = NULL;

  arg_types[0] = &ffi_type_slong;
  arg_types[1] = &ffi_type_slong;
  arg_types[2] = &ffi_type_slong;
  arg_types[3] = &ffi_type_slong;
  arg_types[4] = &ffi_type_slong;
  arg_types[5] = &ffi_type_slong;
  arg_types[6] = &ffi_type_slong;
  arg_types[7] = &ffi_type_slong;
  arg_types[8] = &ffi_type_sint;
  arg_types[9] = &mixed_type;
  arg_types[10] = &ffi_type_sint;
  arg_types[11] = NULL;
  return ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 11, &ffi_type_sint,
                      arg_types) == FFI_OK;
}

int
main(int argc, char **argv)
{
  long integers[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  int before = 9;
  mixed_struct value = { 1.5f, 2.5f, 17 };
  int after = 11;
  void *args[11];
  int direct_result;
  int i;

  if (argc != 2 || (strcmp(argv[1], "--call") != 0 &&
                    strcmp(argv[1], "--closure") != 0))
    {
      fprintf(stderr, "usage: %s --call|--closure\n", argv[0]);
      return 2;
    }

  if (!prepare_cif())
    {
      fprintf(stderr, "setup failed: ffi_prep_cif returned an error\n");
      return 2;
    }

  for (i = 0; i < 8; ++i)
    args[i] = &integers[i];
  args[8] = &before;
  args[9] = &value;
  args[10] = &after;

  direct_result = target(1, 2, 3, 4, 5, 6, 7, 8, 9, value, 11);
  printf("native control result: expected=%d observed=%d (%s)\n",
         EXPECTED_RESULT, direct_result,
         native_arguments_match && direct_result == EXPECTED_RESULT
         ? "pass" : "FAIL");
  if (!native_arguments_match || direct_result != EXPECTED_RESULT)
    return 1;

  if (strcmp(argv[1], "--call") == 0)
    {
      ffi_arg result = 0;
      native_arguments_match = 0;
      ffi_call(&cif, FFI_FN(target), &result, args);
      printf("ffi_call result: expected=%d observed=%d (%s)\n",
             EXPECTED_RESULT, (int) result,
             native_arguments_match && (int) result == EXPECTED_RESULT
             ? "pass" : "FAIL");
      return native_arguments_match && (int) result == EXPECTED_RESULT
        ? 0 : 1;
    }
  else
    {
      void *code = NULL;
      ffi_closure *closure = ffi_closure_alloc(sizeof(ffi_closure), &code);
      int result;

      if (closure == NULL)
        {
          fprintf(stderr, "setup unsupported: ffi_closure_alloc failed\n");
          return 2;
        }
      if (ffi_prep_closure_loc(closure, &cif, closure_handler, NULL, code)
          != FFI_OK)
        {
          fprintf(stderr, "setup unsupported: ffi_prep_closure_loc failed\n");
          ffi_closure_free(closure);
          return 2;
        }

      closure_arguments_match = 0;
      result = ((closure_fn) code)(1, 2, 3, 4, 5, 6, 7, 8,
                                   9, value, 11);
      printf("closure result: expected=%d observed=%d (%s)\n",
             EXPECTED_RESULT, result,
             closure_arguments_match && result == EXPECTED_RESULT
             ? "pass" : "FAIL");
      ffi_closure_free(closure);
      return closure_arguments_match && result == EXPECTED_RESULT ? 0 : 1;
    }
}

#endif
