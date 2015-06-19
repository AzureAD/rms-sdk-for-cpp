/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef GUIDQT_H
#define GUIDQT_H

#include <Guid.h>
#include <QUuid>

namespace rmsauth {

class GuidQt : public IGuid
{
public:
    GuidQt();
    explicit GuidQt(const QString& str);
    explicit GuidQt(const QUuid&);

private:
    QUuid uuid_;

    virtual String toString() const override;
    virtual bool empty() const override;

};

} // namespace rmsauth {

#endif // GUIDQT_H
