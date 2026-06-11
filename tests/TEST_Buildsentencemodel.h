#ifndef TEST_BUILDSENTENCEMODEL_H
#define TEST_BUILDSENTENCEMODEL_H

#include <QObject>

/*!
 * \class TEST_BuildSentenceModel
 * \brief Тестовый класс для buildSentenceModel.
 */
class TEST_BuildSentenceModel : public QObject
{
    Q_OBJECT
public:
    TEST_BuildSentenceModel();
    ~TEST_BuildSentenceModel();

private slots:
    void TestStructure_data();
    void TestStructure();

    void TestNextNonPunct_data();
    void TestNextNonPunct();

    void TestFeatsNumber_data();
    void TestFeatsNumber();

    void TestFeatsVerbForm_data();
    void TestFeatsVerbForm();

    void TestFeatsDegree_data();
    void TestFeatsDegree();

    void TestFeatsCase_data();
    void TestFeatsCase();

    void TestFeatsBool_data();
    void TestFeatsBool();

    void TestUposEnum_data();
    void TestUposEnum();

    void TestDeprelEnum_data();
    void TestDeprelEnum();

    void TestMwtFlag_data();
    void TestMwtFlag();
};

#endif // TEST_BUILDSENTENCEMODEL_H
