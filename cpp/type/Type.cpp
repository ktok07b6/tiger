#include "Type.h"

class Type g_NoneT;
class IntT g_IntT;
class StrT g_StrT;
class NilT g_NilT;
class VoidT g_VoidT;

class Type *NoneT = &g_NoneT;
class IntT *IntT  = &g_IntT;
class StrT *StrT  = &g_StrT;
class NilT *NilT  = &g_NilT;
class VoidT *VoidT  = &g_VoidT;

//RecordT *RecordT::head;
