/*!
* \file TEST_Rule_ART003.h
* \brief DDT-тесты для правила ART-003 (тесты 6.13–6.16).
*
* Все тесты из спецификации (тесты_v3.md) объединены
* в одну тестовую функцию с последовательной нумерацией.
* Каждый addRow содержит только заполненные поля структуры ожиданий,
* остальные остаются со значениями по умолчанию («не проверять»).
*/

#ifndef TEST_RULE_ART003_H
#define TEST_RULE_ART003_H

#include <QObject>

/*!
* \class TEST_Rule_ART003
* \brief DDT-тесты для правила ART-003 (тесты 6.13–6.16).
*/
class TEST_Rule_ART003 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART003();
    ~TEST_Rule_ART003();

private slots:
    void TestRule_data(); ///< Подготовка таблицы данных DDT.
    void TestRule();      ///< Выполнение проверок по таблице.
};

#endif // TEST_RULE_ART003_H
