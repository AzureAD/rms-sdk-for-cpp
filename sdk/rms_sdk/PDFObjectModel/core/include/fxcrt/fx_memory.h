#ifndef _FX_MEMORY_H_
#define _FX_MEMORY_H_
#ifndef _FX_SYSTEM_H_
#include "fx_system.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif
#define FX_Alloc(type, size)						(type*)malloc(sizeof(type) * (size))
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
#define FX_AlignedAlloc(type, size, align)		(type*)_aligned_malloc(sizeof(type) * (size), align)
#else
#define FX_AlignedAlloc(type, size, align)		(type*)memalign(align, sizeof(type) * (size))
#endif
#define FX_Realloc(type, ptr, size)					(type*)realloc(ptr, sizeof(type) * (size))
#define FX_AllocNL(type, size)						FX_Alloc(type, size)
#define FX_ReallocNL(type, ptr, size)				FX_Realloc(type, ptr, size)
#define FX_Free(ptr)								free(ptr)
void*	FXMEM_DefaultAlloc(size_t byte_size, int flags);
void*	FXMEM_DefaultRealloc(void* pointer, size_t new_size, int flags);
void	FXMEM_DefaultFree(void* pointer, int flags);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
class CFX_Object
{
public:
    void*			operator new (size_t size, FX_LPCSTR file, int line)
    {
        FX_UNREFERENCED_PARAMETER(file);
        FX_UNREFERENCED_PARAMETER(line);
        return malloc(size);
    }
    void			operator delete (void* p, FX_LPCSTR file, int line)
    {
        FX_UNREFERENCED_PARAMETER(file);
        FX_UNREFERENCED_PARAMETER(line);
        free(p);
    }
    void*			operator new (size_t size)
    {
        return malloc(size);
    }
    void			operator delete (void* p)
    {
        free(p);
    }
    void*			operator new[] (size_t size, FX_LPCSTR file, int line)
    {
        FX_UNREFERENCED_PARAMETER(file);
        FX_UNREFERENCED_PARAMETER(line);
        return malloc(size);
    }
    void			operator delete[] (void* p, FX_LPCSTR file, int line)
    {
        FX_UNREFERENCED_PARAMETER(file);
        FX_UNREFERENCED_PARAMETER(line);
        free(p);
    }
    void*			operator new[] (size_t size)
    {
        return malloc(size);
    }
    void			operator delete[] (void* p)
    {
        free(p);
    }
    void*			operator new (size_t, void* buf)
    {
        return buf;
    }
    void			operator delete (void*, void*)							{}
};
#endif
#ifdef __cplusplus
#if defined(_DEBUG)
#define FX_NEW new(__FILE__, __LINE__)
#else

#define FX_NEW new
#endif
#define FX_NEW_VECTOR(Pointer, Class, Count) \
    { \
        Pointer = FX_Alloc(Class, Count); \
        if (Pointer) { \
            for (int i = 0; i < (Count); i ++) new (Pointer + i) Class; \
        } \
    }
#define FX_DELETE_VECTOR(Pointer, Class, Count) \
    { \
        for (int i = 0; i < (Count); i ++) Pointer[i].~Class(); \
        FX_Free(Pointer); \
    }
class CFX_DestructObject : public CFX_Object
{
public:

    virtual ~CFX_DestructObject() {}
};
#endif
#endif
