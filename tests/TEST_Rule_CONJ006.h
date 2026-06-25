/*!
* \file TEST_Rule_CONJ006.h
* \brief Объявление тестового класса для правила CONJ-006.
*
* Проверяет правило «Избыточный союз при подчинительном».
*/

#ifndef TEST_RULE_CONJ_006_H
#define TEST_RULE_CONJ_006_H

#include <QObject>

/*!
* \class TEST_Rule_CONJ006
* \brief Тестовый класс для CONJ-006 (раздел 6.75–6.76 тестов_v3.md).
*/
class TEST_Rule_CONJ006 : public QObject
{
    Q_OBJECT
public:
    TEST_Rule_CONJ006();
    ~TEST_Rule_CONJ006();

private slots:
    void TestRule_data(); ///< DDT данные CONJ-006
    void TestRule();      ///< Выполнение проверок
};

#endif // TEST_RULE_CONJ_006_H
