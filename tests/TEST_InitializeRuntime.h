#ifndef TEST_INITIALIZERUNTIME_H
#define TEST_INITIALIZERUNTIME_H

#include <QObject>

class TEST_InitializeRuntime : public QObject
{
    Q_OBJECT
public:
    TEST_InitializeRuntime();
    ~TEST_InitializeRuntime();

private slots:
    void TestInitializeRuntime_data();
    void TestInitializeRuntime();
};

#endif // TEST_INITIALIZERUNTIME_H