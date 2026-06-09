/*!
* \file TEST_ValidateSentenceStructure.cpp
* \brief DDT-тесты для функции validateSentenceStructure.
*
* Проверяет структурную валидацию предложения после парсинга:
*  - корректные предложения (валидное дерево, MWT, лимит токенов);
*  - ошибки дерева зависимостей (self-loop, циклы, два корня, нет корня);
*  - ошибки HEAD (вне диапазона, отрицательный);
*  - ошибки MWT (некорректный диапазон);
*  - превышение лимита токенов (200).
*/

#include <QtTest>
#include <QObject>
#include <optional>

#include "TEST_ValidateSentenceStructure.h"
#include "datamodel.h"
#include "auxiliaryfunctionsfortesting.h"

std::optional<Diagnostic> validateSentenceStructure(const RawSentence& sentence);

TEST_ValidateSentenceStructure::TEST_ValidateSentenceStructure() {}
TEST_ValidateSentenceStructure::~TEST_ValidateSentenceStructure() {}

void TEST_ValidateSentenceStructure::TestValidateSentenceStructure_data()
{
    // Колонки DDT-таблицы
    QTest::addColumn<RawSentence>("sentence");        ///< Входное предложение (создано вручную)
    QTest::addColumn<bool>("expectValid");            ///< true если ожидается std::nullopt
    QTest::addColumn<QString>("expectedMessage");     ///< Сообщение об ошибке (или пусто при успехе)

    // Тесты используют makeRawToken для создания токенов вручную.

    // ========================================================================
    // Тест 2.1 – Корректное предложение (The cat sits.)
    // ========================================================================
    {
        // Создаём предложение с деревом: sits(root) → The(det→cat), cat(nsubj→sits), .(punct→sits)
        RawSentence s = makeRawSentence(1, "test", "The cat sits.");
        addToken(s, makeRawToken(1, 1, "The", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "cat", "NOUN", 3, "nsubj"));
        addToken(s, makeRawToken(3, 3, "sits", "VERB", 0, "root"));
        addToken(s, makeRawToken(4, 4, ".", "PUNCT", 3, "punct"));
        QTest::addRow("2.1_valid") << s << true << QString();
    }

    // ========================================================================
    // Тест 2.2 – Self-loop (HEAD=1 для ID=1)
    // ========================================================================
    {
        // Токен 1 ссылается сам на себя — петля в дереве.
        RawSentence s = makeRawSentence(1, "test", "The cat sits.");
        addToken(s, makeRawToken(1, 1, "The", "DET", 1, "det")); // HEAD=1, ID=1 — петля
        addToken(s, makeRawToken(2, 2, "cat", "NOUN", 3, "nsubj"));
        addToken(s, makeRawToken(3, 3, "sits", "VERB", 0, "root"));
        QTest::addRow("2.2_self_loop") << s << false << "HEAD образует петлю";
    }

    // ========================================================================
    // Тест 2.3 – HEAD вне диапазона (HEAD=999)
    // ========================================================================
    {
        // Токен 4 ссылается на HEAD=999, но в предложении всего 4 токена.
        RawSentence s = makeRawSentence(1, "test", "The cat sits here.");
        addToken(s, makeRawToken(1, 1, "The", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "cat", "NOUN", 3, "nsubj"));
        addToken(s, makeRawToken(3, 3, "sits", "VERB", 0, "root"));
        addToken(s, makeRawToken(4, 4, "here", "ADV", 999, "advmod"));
        QTest::addRow("2.3_head_out_of_range") << s << false << "HEAD 999 ссылается на несуществующий токен";
    }

    // ========================================================================
    // Тест 2.4 – HEAD отрицательный (-1)
    // ========================================================================
    {
        // HEAD=-1 недопустим (допустимы только 0..maxId).
        RawSentence s = makeRawSentence(1, "test", "The cat sits.");
        addToken(s, makeRawToken(1, 1, "The", "DET", -1, "det"));
        addToken(s, makeRawToken(2, 2, "cat", "NOUN", 3, "nsubj"));
        addToken(s, makeRawToken(3, 3, "sits", "VERB", 0, "root"));
        QTest::addRow("2.4_head_negative") << s << false << "Некорректный HEAD";
    }

    // ========================================================================
    // Тест 2.5 – Цикл длины 3 (1→2→3→1)
    // ========================================================================
    {
        // Токены образуют цикл: A→B→C→A, плюс D с HEAD=0 (корень).
        RawSentence s = makeRawSentence(1, "test", "A B C D");
        addToken(s, makeRawToken(1, 1, "A", "NOUN", 2, "conj"));
        addToken(s, makeRawToken(2, 2, "B", "NOUN", 3, "conj"));
        addToken(s, makeRawToken(3, 3, "C", "NOUN", 1, "conj")); // замыкает цикл
        addToken(s, makeRawToken(4, 4, "D", "NOUN", 0, "root"));
        QTest::addRow("2.5_cycle_3") << s << false << "HEAD образует петлю";
    }

    // ========================================================================
    // Тест 2.6 – Два корня (HEAD=0 у двух токенов)
    // ========================================================================
    {
        // В дереве зависимостей может быть только один корень (HEAD=0).
        RawSentence s = makeRawSentence(1, "test", "cat sits");
        addToken(s, makeRawToken(1, 1, "cat", "NOUN", 0, "root"));
        addToken(s, makeRawToken(2, 2, "sits", "VERB", 0, "root"));
        QTest::addRow("2.6_two_roots") << s << false << "Несколько корней";
    }

    // ========================================================================
    // Тест 2.7 – Нет корня (ни у кого HEAD≠0)
    // ========================================================================
    {
        // Все токены ссылаются друг на друга, корневой токен (HEAD=0) отсутствует.
        RawSentence s = makeRawSentence(1, "test", "The cat");
        addToken(s, makeRawToken(1, 1, "The", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "cat", "NOUN", 1, "nsubj"));
        QTest::addRow("2.7_no_root") << s << false << "Не найден корневой токен";
    }

    // ========================================================================
    // Тест 2.8 – MWT корректный (don't)
    // ========================================================================
    {
        // MWT 1-2 охватывает токены 1 и 2, все токены присутствуют.
        // Исправлено: do — корень (HEAD=0), n't — зависимый от do (HEAD=1).
        RawSentence s = makeRawSentence(1, "test", "don't");
        addMwt(s, 1, 1, 2, "don't");
        addToken(s, makeRawToken(2, 1, "do", "AUX", 0, "root"));
        addToken(s, makeRawToken(3, 2, "n't", "PART", 1, "advmod"));
        QTest::addRow("2.8_mwt_valid") << s << true << QString();
    }

    // ========================================================================
    // Тест 2.9 – MWT некорректный диапазон (1-3, токен 3 пропущен)
    // ========================================================================
    {
        // MWT заявляет диапазон 1-3, но в предложении нет токена 3.
        // Должна быть ошибка: не все ID из диапазона присутствуют.
        RawSentence s = makeRawSentence(1, "test", "cannot");
        addMwt(s, 1, 1, 3, "cannot"); // диапазон 1-3
        addToken(s, makeRawToken(2, 1, "can", "AUX", 0, "root"));
        // токен 2 и 3 отсутствуют
        QTest::addRow("2.9_mwt_bad_range") << s << false << "Некорректный диапазон MWT";
    }

    // ========================================================================
    // Тест 2.10 – Максимальное количество токенов (200)
    // ========================================================================
    {
        // Предложение ровно из 200 токенов — верхняя граница допустимого.
        // Токен 1 — корень (HEAD=0), остальные — цепочка conj.
        RawSentence s = makeRawSentence(1, "test", "...");
        for (int i = 1; i <= 200; ++i) {
            int head = (i == 1) ? 0 : i - 1;
            addToken(s, makeRawToken(i, i, "word", "NOUN", head, "conj"));
        }
        QTest::addRow("2.10_max_200") << s << true << QString();
    }

    // ========================================================================
    // Тест 2.11 – Превышение максимума (201 токен)
    // ========================================================================
    {
        // Предложение из 201 токена — превышает лимит в 200.
        RawSentence s = makeRawSentence(1, "test", "...");
        for (int i = 1; i <= 201; ++i) {
            int head = (i == 1) ? 0 : i - 1;
            addToken(s, makeRawToken(i, i, "word", "NOUN", head, "conj"));
        }
        QTest::addRow("2.11_over_200") << s << false << "Превышен максимум в 200 токенов";
    }
}

void TEST_ValidateSentenceStructure::TestValidateSentenceStructure()
{
    QFETCH(RawSentence, sentence);
    QFETCH(bool, expectValid);
    QFETCH(QString, expectedMessage);

    // Вызов тестируемой функции
    auto result = ::validateSentenceStructure(sentence);
    const QString tag = QString(QTest::currentDataTag());

    // Сравниваем результат с ожидаемым через хелпер.
    // При ожидании успеха, но получении ошибки — хелпер выведет сообщение в qDebug.
    compareOptionalDiagnostic(tag, result, expectValid, expectedMessage);
}
