#ifndef TEST_RULE_OTHER006_H
#define TEST_RULE_OTHER006_H

#include <QObject>

class TEST_Rule_OTHER006 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_OTHER006();
    ~TEST_Rule_OTHER006();

private slots:
    void TestRule_data();
    void TestRule();
};

#endif // TEST_RULE_OTHER006_H