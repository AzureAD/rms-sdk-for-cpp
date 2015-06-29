/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <QApplication>
#include <QCommandLineParser>
#include "NonInteractiveTests.h"
#include "InteractiveTests.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();

    // A boolean option with multiple names (-i, --interactive)
    QCommandLineOption interactiveOption(QStringList() << "i" << "interactive", "Run interactive test cases.");
    parser.addOption(interactiveOption);

    // Process the actual command line arguments given by the user
    parser.process(app);

    bool interactive = parser.isSet(interactiveOption);

    qDebug() << "interactiveOption: " << interactive;

    int res = 0;

    if(interactive)
    {
        res += QTest::qExec(new InteractiveTests());
    }

    res += QTest::qExec(new NonInteractiveTests());

    return res;
}
