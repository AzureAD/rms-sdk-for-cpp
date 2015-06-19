/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef GUID_H
#define GUID_H

#include "types.h"

namespace rmsauth {

class IGuid
{
public:
    virtual String toString() const = 0;
    virtual bool empty() const = 0;
};

class Guid : public IGuid
{
public:
    Guid();
    Guid(const String& str);
    virtual String toString() const override;
    virtual bool empty() const override;

    static Guid newGuid();

private:
    ptr<IGuid> pImpl;
    Guid(ptr<IGuid> guid);
};

} // namespace rmsauth {

#endif // GUID_H
