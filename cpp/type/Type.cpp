#include "Type.h"

struct Type g_NoneT;
struct IntT g_IntT;
struct StrT g_StrT;
struct NilT g_NilT;
struct VoidT g_VoidT;

struct Type *NoneT = &g_NoneT;
struct IntT *IntT  = &g_IntT;
struct StrT *StrT  = &g_StrT;
struct NilT *NilT  = &g_NilT;
struct VoidT *VoidT  = &g_VoidT;

//RecordT *RecordT::head;
