#include "exlib/cout_buffer.h"

namespace ex {

EXLIB_API ThreadSafeOStream thread_safe_cout(std::cout);

EXLIB_API EndlT endl;

EXLIB_API FlushT flush;

EXLIB_API FlushT &commit = flush;

EXLIB_API RollbackT rollback;

}