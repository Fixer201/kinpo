#ifndef TEST_BUILDSENTENCEMODEL_H
#define TEST_BUILDSENTENCEMODEL_H

#include <QObject>

/*!
* \class TEST_BuildSentenceModel
* \brief Тестовый класс для buildSentenceModel.
*
* Единственная тестовая функция TestBuildSentenceModel с DDT-данными.
* Структура ожиданий BuildExpect позволяет задавать точечные проверки
* для каждого теста: заполняются только те поля, что указаны в спецификации.
*/
class TEST_BuildSentenceModel : public QObject
{
    Q_OBJECT
public:
    TEST_BuildSentenceModel();
    ~TEST_BuildSentenceModel();

private slots:
    void TestBuildSentenceModel_data();
    void TestBuildSentenceModel();
};

#endif // TEST_BUILDSENTENCEMODEL_H