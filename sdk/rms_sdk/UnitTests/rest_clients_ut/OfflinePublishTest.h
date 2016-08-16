#ifndef OFFLINEPUBLISHTEST_H
#define OFFLINEPUBLISHTEST_H

#include <QObject>
#include <QTest>
namespace UnitTests {
namespace RestClientsUT {
class OfflinePublishTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testCreatePLWithTemplate();
    void testCreatePLCustom();
private:
    static std::string generateMockCLC(std::string email, std::string &outCLC);
};

} //UnitTests
} //RestClientsUT
#endif // OFFLINEPUBLISHTEST_H
