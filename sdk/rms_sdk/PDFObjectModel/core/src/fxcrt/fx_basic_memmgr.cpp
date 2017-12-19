#include "../../include/fxcrt/fx_basic.h"
#ifdef __cplusplus
extern "C" {
#endif
void*	FXMEM_DefaultAlloc(size_t byte_size, int flags)
{
  FX_UNREFERENCED_PARAMETER(flags);
    return (void*)malloc(byte_size);
}
void*	FXMEM_DefaultRealloc(void* pointer, size_t new_size, int flags)
{
  FX_UNREFERENCED_PARAMETER(flags);
    return realloc(pointer, new_size);
}
void	FXMEM_DefaultFree(void* pointer, int flags)
{
  FX_UNREFERENCED_PARAMETER(flags);
    free(pointer);
}
#ifdef __cplusplus
}
#endif
