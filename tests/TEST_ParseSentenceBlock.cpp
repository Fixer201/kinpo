/*!
* \file TEST_ParseSentenceBlock.cpp
* \brief DDT-тесты для функции parseSentenceBlock.
*
* Проверяет корректность парсинга одного блока строк CoNLL-U:
*  - валидные блоки с токенами и MWT;
*  - отсутствие обязательных метаданных (# sent_id, # text);
*  - ошибки формата строк (комментарии, количество колонок, ID, HEAD, UPOS, FORM, порядок ID, MWT).
*/

#include <QtTest>
#include <QObject>
#include <variant>

#include "TEST_ParseSentenceBlock.h"
#include "datamodel.h"
#include "inputmodule.h"
#include "auxiliaryfunctionsfortesting.h"

TEST_ParseSentenceBlock::TEST_ParseSentenceBlock() {}
TEST_ParseSentenceBlock::~TEST_ParseSentenceBlock() {}

void TEST_ParseSentenceBlock::TestParseSentenceBlock_data()
{
    // Колонки DDT-таблицы
    QTest::addColumn<QStringList>("block");               ///< Строки CoNLL-U блока
    QTest::addColumn<int>("firstLineNumber");             ///< Номер первой строки во входном файле
    QTest::addColumn<QString>("expectedSentId");          ///< Ожидаемый sentId (или пусто при ошибке)
    QTest::addColumn<int>("expectedTokenCount");        ///< Ожидаемое число токенов
    QTest::addColumn<QString>("expectedText");            ///< Ожидаемый текст предложения
    QTest::addColumn<QString>("expectedFirstTokenForm");///< FORM первого токена
    QTest::addColumn<QString>("expectedLastTokenUpos"); ///< UPOS последнего токена
    QTest::addColumn<int>("expectedMwtCount");          ///< Число MWT-записей
    QTest::addColumn<QString>("expectedErrorKind");     ///< Тип ошибки (или пусто при успехе)
    QTest::addColumn<QString>("expectedErrorMessage");  ///< Сообщение об ошибке (или пусто)

    // ========================================================================
    // Тест 3.1 – Корректный блок
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = test-01")
              << makeComment("text = The cat sits.")
              << makeTokenLine(1, "The", "the", "DET", 3, "det")
              << makeTokenLine(2, "cat", "cat", "NOUN", 3, "nsubj")
              << makeTokenLine(3, "sits", "sit", "VERB", 0, "root")
              << makeTokenLine(4, ".", ".", "PUNCT", 3, "punct");
        QTest::addRow("3.1_valid") << block << 1
            << "test-01" << 4 << "The cat sits." << "The" << "PUNCT"
            << 0 << QString() << QString();
    }

    // ========================================================================
    // Тест 3.2 – Отсутствует sent_id
    // ========================================================================
    {
        QStringList block;
        block << makeComment("text = The cat sits.")
              << makeTokenLine(1, "cat", "cat", "NOUN", 2, "nsubj")
              << makeTokenLine(2, "sits", "sit", "VERB", 0, "root");
        QTest::addRow("3.2_missing_sent_id") << block << 1
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Отсутствует '# sent_id'";
    }

    // ========================================================================
    // Тест 3.3 – Отсутствует text
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = test-03")
              << makeTokenLine(1, "cat", "cat", "NOUN", 2, "nsubj")
              << makeTokenLine(2, "sits", "sit", "VERB", 0, "root");
        QTest::addRow("3.3_missing_text") << block << 1
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Отсутствует '# text'";
    }

    // ========================================================================
    // Тест 3.4 – Ошибка формата комментария (# без пробела)
    // ========================================================================
    {
        QStringList block;
        block << QStringLiteral("#sent_id = test")
              << makeTokenLine(1, "a", "a", "DET", 2, "det")
              << makeTokenLine(2, "cat", "cat", "NOUN", 0, "root");
        QTest::addRow("3.4_bad_comment") << block << 1
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Ошибка формата комментария";
    }

    // ========================================================================
    // Тест 3.5 – 9 колонок
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = t")
              << QStringLiteral("1\tcat\tcat\tNOUN\tNN\t_\t2\tnsubj\t_");
        QTest::addRow("3.5_nine_columns") << block << 2
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Ожидается 10 колонок, найдено 9";
    }

    // ========================================================================
    // Тест 3.6 – 11 колонок
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = t")
              << QStringLiteral("1\tcat\tcat\tNOUN\tNN\t_\t2\tnsubj\t_\t_\textra");
        QTest::addRow("3.6_eleven_columns") << block << 2
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Ожидается 10 колонок, найдено 11";
    }

    // ========================================================================
    // Тест 3.7 – Некорректный ID (abc)
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = t")
              << QStringLiteral("abc\tcat\tcat\tNOUN\tNN\t_\t2\tnsubj\t_\t_");
        QTest::addRow("3.7_bad_id") << block << 2
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Некорректный ID";
    }

    // ========================================================================
    // Тест 3.8 – Некорректный HEAD (-1)
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = t")
              << makeTokenLine(1, "cat", "cat", "NOUN", -1, "nsubj");
        QTest::addRow("3.8_bad_head") << block << 2
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Некорректный HEAD";
    }

    // ========================================================================
    // Тест 3.9 – Некорректный UPOS
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = t")
              << makeTokenLine(1, "cat", "cat", "INVALID", 0, "root");
        QTest::addRow("3.9_invalid_upos") << block << 2
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Некорректный UPOS";
    }

    // ========================================================================
    // Тест 3.10 – Пустая FORM
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = t")
              << makeTokenLine(1, "", "cat", "NOUN", 2, "nsubj");
        QTest::addRow("3.10_empty_form") << block << 2
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Колонка FORM не может быть пустой";
    }

    // ========================================================================
    // Тест 3.11 – Нарушен порядок ID
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = t")
              << makeTokenLine(1, "a", "a", "DET", 3, "det")
              << makeTokenLine(3, "cat", "cat", "NOUN", 3, "nsubj")
              << makeTokenLine(2, "sits", "sit", "VERB", 0, "root");
        QTest::addRow("3.11_wrong_id_order") << block << 2
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Нарушен порядок ID";
    }

    // ========================================================================
    // Тест 3.13 – MWT с непустыми колонками
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = t")
              << makeComment("text = don't")
              << QStringLiteral("1-2\tdon't\t_\t_\tNOUN\t_\t_\t_\t_\t_")
              << makeTokenLine(1, "do", "do", "AUX", 3, "aux")
              << makeTokenLine(2, "n't", "not", "PART", 3, "advmod");
        QTest::addRow("3.13_mwt_nonempty") << block << 2
            << QString() << 0 << QString() << QString() << QString()
            << 0 << "InputFormatError" << "Составной токен требует '_' в колонке";
    }

    // ========================================================================
    // Тест 3.14 – Корректный MWT (don't)
    // ========================================================================
    {
        QStringList block;
        block << makeComment("sent_id = mwt-01")
              << makeComment("text = don't")
              << makeMwtLine("1-2", "don't")
              << makeTokenLine(1, "do", "do", "AUX", 3, "aux")
              << makeTokenLine(2, "n't", "not", "PART", 3, "advmod")
              << makeTokenLine(3, "go", "go", "VERB", 0, "root");
        QTest::addRow("mwt_ok") << block << 1
            << "mwt-01" << 3 << "don't" << "do" << "VERB"
            << 1 << QString() << QString();
    }
}

void TEST_ParseSentenceBlock::TestParseSentenceBlock()
{
    QFETCH(QStringList, block);
    QFETCH(int, firstLineNumber);
    QFETCH(QString, expectedSentId);
    QFETCH(int, expectedTokenCount);
    QFETCH(QString, expectedText);
    QFETCH(QString, expectedFirstTokenForm);
    QFETCH(QString, expectedLastTokenUpos);
    QFETCH(int, expectedMwtCount);
    QFETCH(QString, expectedErrorKind);
    QFETCH(QString, expectedErrorMessage);

    // Вызов тестируемой функции
    auto result = parseSentenceBlock(block, firstLineNumber);
    const QString tag = QString(QTest::currentDataTag());

    if (expectedErrorKind.isEmpty()) {
        // Ожидаем успешный разбор
        if (std::holds_alternative<Diagnostic>(result)) {
            // Если пришла ошибка вместо RawSentence выводим её
            const Diagnostic& d = std::get<Diagnostic>(result);
            qDebug() << "[TEST] " << tag << " - неожиданная диагностическая ошибка:" << d.message;
        }
        // Проверяем, что функция действительно вернула RawSentence,
        // а не Diagnostic с ошибкой. Если тут падает, значит
        // входные данные в тесте сломаны, либо в реализации parseSentenceBlock баг
        QVERIFY(std::holds_alternative<RawSentence>(result));

        // Получаем ссылку на RawSentence
        const RawSentence& rs = std::get<RawSentence>(result);

        // Сравниваем sentId, text, количество токенов, FORM первого токена
        // и UPOS последнего токена с ожидаемыми значениями из DDT-таблицы.
        compareRawSentence(tag, rs,
                           expectedSentId, expectedText,
                           expectedTokenCount,
                           expectedFirstTokenForm, expectedLastTokenUpos);

        // Проверяем количество MWT-записей
        // expectedMwtCount = 0 для блоков без MWT, > 0 для блоков с MWT.
        QCOMPARE(rs.mwtRecords.size(), expectedMwtCount);
    } else {
        // Проверяем, что функция вернула Diagnostic (а не RawSentence).
        // Если тут падает, значит тест неправильно ожидает ошибку:
        // либо входные данные валидны, либо баг в тесте.
        QVERIFY(std::holds_alternative<Diagnostic>(result));

        // Получаем ссылку на Diagnostic
        const Diagnostic& d = std::get<Diagnostic>(result);

        // Сравниваем категорию ошибки (InputFormatError) и текст сообщения
        // с ожидаемыми значениями из DDT-таблицы.
        compareDiagnostic(tag, d, expectedErrorKind, expectedErrorMessage);
    }
}
