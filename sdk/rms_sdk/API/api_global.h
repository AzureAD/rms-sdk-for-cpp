#ifndef API_GLOBAL_H
#define API_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(API_LIBRARY)
#  define APISHARED_EXPORT Q_DECL_EXPORT
#else
#  define APISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // API_GLOBAL_H
