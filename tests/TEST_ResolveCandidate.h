/*!
* \file TEST_ResolveCandidate.h
* \brief Объявление тестового класса для функции resolveCandidate.
*
* Проверяет инкрементальное разрешение конфликтов приоритетов
* между кандидатами в одной зоне conflictTokenIds.
*/

#ifndef TEST_RESOLVECANDIDATE_H
#define TEST_RESOLVECANDIDATE_H

#include <QObject>

/*!
* \class TEST_ResolveCandidate
* \brief Тестовый класс для resolveCandidate (раздел 5 тестов_v3.md).
*/
class TEST_ResolveCandidate : public QObject
{
    Q_OBJECT
public:
    TEST_ResolveCandidate();
    ~TEST_ResolveCandidate();

private slots:
    void TestResolveCandidate_data(); ///\u003c DDT данные раздела 5
    void TestResolveCandidate();      ///\u003c Выполнение проверок
};

#endif // TEST_RESOLVECANDIDATE_H
