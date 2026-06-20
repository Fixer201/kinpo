/*!
* \file TEST_Rule_ART001.h
* \brief Объявление тестового класса для правила ART-001.
*
* Проверяет срабатывание и исключения правила «Лишний артикль перед PROPN».
*/

#ifndef TEST_RULE_ART001_H
#define TEST_RULE_ART001_H

#include <QObject>

/*!
* \class TEST_Rule_ART001
* \brief Тестовый класс для ART-001 (раздел 6.1–6.6 тестов_v3.md).
*/
class TEST_Rule_ART001 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_ART001();
    ~TEST_Rule_ART001();

private slots:
    void TestRule_data(); ///\u003c DDT данные ART-001
    void TestRule();      ///\u003c Выполнение проверок
};

#endif // TEST_RULE_ART001_H
