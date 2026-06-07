/*!
* \file auxiliaryfunctionsfortesting.cpp
* \brief Реализация хелперов для тестовых данных и сравнения.
*/

#include "auxiliaryfunctionsfortesting.h"
#include <QTest>

// =======================================================================
// Хелперы создания данных
// =======================================================================

QString makeTokenLine(int id,
                      const QString& form,
                      const QString& lemma,
                      const QString& upos,
                      int head,
                      const QString& deprel,
                      const QString& xpos,
                      const QString& feats,
                      const QString& deps,
                      const QString& misc)
{
    // Собирает строку CoNLL-U с ровно 10 колонками через табуляцию.
    // Порядок полей строго соответствует спецификации CoNLL-U:
    // ID, FORM, LEMMA, UPOS, XPOS, FEATS, HEAD, DEPREL, DEPS, MISC.
    return QStringLiteral("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10")
        .arg(id).arg(form).arg(lemma).arg(upos).arg(xpos)
        .arg(feats).arg(head).arg(deprel).arg(deps).arg(misc);
}

QString makeMwtLine(const QString& rangeId, const QString& form)
{
    // Собирает строку Multi-Word Token (MWT) CoNLL-U.
    // MWT имеет ID вида "N-M" (например, "2-3"), FORM - исходная форма,
    // все остальные колонки должны содержать символ "_".
    return QStringLiteral("%1\t%2\t_\t_\t_\t_\t_\t_\t_\t_")
        .arg(rangeId).arg(form);
}

QString makeComment(const QString& content)
{
    // Формирует строку комментария CoNLL-U.
    // По стандарту комментарий начинается с "# " (решётка и пробел),
    // затем идёт содержимое (например, "sent_id = test-01").
    return QStringLiteral("# %1").arg(content);
}

// =======================================================================
// Хелперы сравнения (с дополнительным логированием несовпадений)
// =======================================================================

void compareRawSentence(const QString& /*testName*/,
                        const RawSentence& actual,
                        const QString& expectedSentId,
                        const QString& expectedText,
                        int expectedTokenCount,
                        const QString& expectedFirstTokenForm,
                        const QString& expectedLastTokenUpos)
{
    // Проверяет sentId: должен точно совпадать с ожидаемым значением.
    QCOMPARE(actual.sentId, expectedSentId);

    // Проверяет text: должен точно совпадать с ожидаемым значением.
    QCOMPARE(actual.text, expectedText);

    // Проверяет общее количество токенов (обычных + без MWT).
    QCOMPARE(actual.tokens.size(), expectedTokenCount);

    // Если токены есть — дополнительно проверяет FORM первого
    // и UPOS последнего токена (чтобы убедиться, что парсинг
    // прочитал все поля правильно и не потерял порядок).
    if (!actual.tokens.isEmpty()) {
        QCOMPARE(actual.tokens.first().form, expectedFirstTokenForm);
        QCOMPARE(actual.tokens.last().upos, expectedLastTokenUpos);
    }
}

void compareDiagnostic(const QString& /*testName*/,
                       const Diagnostic& actual,
                       const QString& expectedKind,
                       const QString& expectedMessage)
{
    // Преобразует вид ошибки в строку (например, InputFormatError)
    // и сравнивает с ожидаемым.
    QCOMPARE(diagnosticKindToString(actual.kind), expectedKind);

    // Проверяет текст сообщения об ошибке: должно полностью совпадать.
    QCOMPARE(actual.message, expectedMessage);
}
