/*!
* \file auxiliaryfunctionsfortesting.cpp
* \brief Реализация хелперов для тестовых данных и сравнения.
*/

#include "auxiliaryfunctionsfortesting.h"
#include <QTest>

// =====================================================================
// Вспомогательные функции создания строк CoNLL-U
// =====================================================================

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
    // MWT имеет ID вида "N-M" (например, "2-3"), FORM — исходная форма,
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

// =====================================================================
// Вспомогательные функции создания RawSentence вручную (изолированно от парсера)
// =====================================================================

RawSentence makeRawSentence(int firstLine, const QString& sentId, const QString& text)
{
    // Создаёт пустой RawSentence с заданными метаданными.
    // Списки tokens и mwtRecords инициализируются пустыми.
    RawSentence s;
    s.firstLineNumber = firstLine;
    s.sentId = sentId;
    s.text = text;
    return s;
}

RawToken makeRawToken(int lineNumber, int id, const QString& form,
                      const QString& upos, int headId, const QString& deprel)
{
    // Создаёт RawToken с минимальным набором полей.
    // Поля, которые не важны для структурной валидации,
    // заполняются символом "_" (LEMMA, XPOS, FEATS, DEPS, MISC).
    RawToken t;
    t.lineNumber = lineNumber;
    t.id = id;
    t.form = form;
    t.lemma = QStringLiteral("_");
    t.upos = upos;
    t.xpos = QStringLiteral("_");
    t.featsRaw = QStringLiteral("_");
    t.headId = headId;
    t.deprel = deprel;
    t.depsRaw = QStringLiteral("_");
    t.miscRaw = QStringLiteral("_");
    return t;
}


RawToken makeRawToken(int lineNumber, int id, const QString& form,
                      const QString& upos, int headId, const QString& deprel,
                      const QString& featsRaw) {
    RawToken t = makeRawToken(lineNumber, id, form, upos, headId, deprel);

    t.featsRaw = featsRaw;

    return t;
}


void addToken(RawSentence& sentence, const RawToken& token)
{
    // Добавляет токен в конец списка tokens предложения.
    sentence.tokens.append(token);
}

void addMwt(RawSentence& sentence, int lineNumber, int rangeStart, int rangeEnd, const QString& form)
{
    // Создаёт MwtRecord и добавляет её в конец списка mwtRecords предложения.
    MwtRecord rec;
    rec.lineNumber = lineNumber;
    rec.rangeStart = rangeStart;
    rec.rangeEnd = rangeEnd;
    rec.form = form;
    sentence.mwtRecords.append(rec);
}

// =====================================================================
// Вспомогательные функции сравнения
// =====================================================================

void compareRawSentence(const QString& testName,
                        const RawSentence& actual,
                        const QString& expectedSentId,
                        const QString& expectedText,
                        int expectedTokenCount,
                        const QString& expectedFirstTokenForm,
                        const QString& expectedLastTokenUpos)
{
    // Проверяет sentId: должен точно совпадать с ожидаемым значением.
    if (actual.sentId != expectedSentId) {
        qDebug() << "[TEST FAIL]" << testName << "sendId не совпадает:"
                 << "actual =" << actual.sentId
                 << "expected =" << expectedSentId;
    }
    QCOMPARE(actual.sentId, expectedSentId);


    // Проверяет text: должен точно совпадать с ожидаемым значением.
    QCOMPARE(actual.text, expectedText);

    // Проверяет общее количество токенов (обычных + без MWT).
    QCOMPARE(actual.tokens.size(), expectedTokenCount);

    // Если токены есть дополнительно проверяет FORM первого
    // и UPOS последнего токена (чтобы убедиться, что парсинг
    // прочитал все поля правильно и не потерял порядок).
    if (!actual.tokens.isEmpty()) {
        QCOMPARE(actual.tokens.first().form, expectedFirstTokenForm);
        QCOMPARE(actual.tokens.last().upos, expectedLastTokenUpos);
    }
}

void compareDiagnostic(const QString& testName,
                       const Diagnostic& actual,
                       const QString& expectedKind,
                       const QString& expectedMessage)
{
    // Преобразует вид ошибки в строку (например, InputFormatError)
    // и сравнивает с ожидаемым.
    QString actualKind = diagnosticKindToString(actual.kind);

    if (actualKind != expectedKind) {
        qDebug() << "[TEST FAIL]" << testName << "ошибки не совпадают:"
                 << actualKind << "!=" << expectedKind;
    }
    QCOMPARE(actualKind, expectedKind);


    // Проверяет текст сообщения об ошибке: должно полностью совпадать.
    QCOMPARE(actual.message, expectedMessage);
}

void compareOptionalDiagnostic(const QString& testName,
                                const std::optional<Diagnostic>& actual,
                                bool expectValid,
                                const QString& expectedMessage)
{
    if (expectValid) {
        if (actual.has_value()) {
            qDebug() << "[TEST FAIL] Неожиданная ошибка валидации:" << testName << actual->message;
        }
        QVERIFY(!actual.has_value());
    } else {
        QVERIFY(actual.has_value());

        if (actual->message != expectedMessage) {
            qDebug() << "[TEST FAIL]" << testName
                     << "сообщеиня не совпадают:" << actual->message
                     << "!=" << expectedMessage;
        }
        QCOMPARE(actual->message, expectedMessage);
    }
}

// =====================================================================
// Вспомогательные функции сравнения CandidateError / QSet<CandidateError>
// =====================================================================

void compareCandidateSet(const QString& testName,
                         const QSet<CandidateError>& actual,
                         const QSet<QString>& expectedRuleIds)
{
    QSet<QString> actualRuleIds;
    for (const auto& ce : actual) {
        actualRuleIds.insert(ce.ruleId);
    }
    if (actualRuleIds != expectedRuleIds) {
        qDebug() << "[TEST FAIL]" << testName
                 << "ruleIds не совпадают:" << actualRuleIds
                 << "expected:" << expectedRuleIds;
    }
    QCOMPARE(actualRuleIds, expectedRuleIds);
}

void compareConflictZones(const QString& testName,
                          const QSet<CandidateError>& actual,
                          const QList<QSet<int>>& expectedZones)
{
    QList<QSet<int>> actualZones;
    for (const auto& ce : actual) {
        actualZones.append(ce.conflictTokenIds);
    }

    int matchCount = 0;
    for (const QSet<int>& expectedZone : expectedZones) {
        bool found = false;
        for (const QSet<int>& actualZone : actualZones) {
            if (actualZone == expectedZone) {
                found = true;
                break;
            }
        }
        QVERIFY2(found, qPrintable(QString("[%1] Не найдена зона конфликта %2")
                                   .arg(testName)
                                   .arg([](const QSet<int>& s){
                                       QStringList parts;
                                       for (int v : s) parts.append(QString::number(v));
                                       return parts.isEmpty() ? QStringLiteral("∅") : parts.join(',');
                                   }(expectedZone))));
        if (found) ++matchCount;
    }

    QCOMPARE(matchCount, expectedZones.size());
}

void compareZoneCandidates(const QString& testName,
                           const ConflictZoneMap& zoneMap,
                           const QSet<int>& zoneKey,
                           int expectedCount,
                           const QSet<QString>& expectedRuleIds)
{
    auto it = zoneMap.zones.find(zoneKey);
    if (it == zoneMap.zones.end()) {
        qDebug() << "[TEST FAIL]" << testName
                 << "зона не найдена:" << [](const QSet<int>& s){
                     QStringList parts;
                     for (int v : s) parts.append(QString::number(v));
                     return parts.isEmpty() ? QStringLiteral("∅") : parts.join(',');
                 }(zoneKey);
        QVERIFY2(it != zoneMap.zones.end(), qPrintable(QString("[%1] Зона не найдена").arg(testName)));
        return;
    }

    const QSet<CandidateError>& candidates = it.value();
    if (candidates.size() != expectedCount) {
        qDebug() << "[TEST FAIL]" << testName
                 << "число кандидатов не совпадает:" << candidates.size()
                 << "expected:" << expectedCount;
    }
    QCOMPARE(candidates.size(), expectedCount);

    QSet<QString> actualRuleIds;
    for (const auto& ce : candidates) {
        actualRuleIds.insert(ce.ruleId);
    }
    if (actualRuleIds != expectedRuleIds) {
        qDebug() << "[TEST FAIL]" << testName
                 << "ruleIds не совпадают:" << actualRuleIds
                 << "expected:" << expectedRuleIds;
    }
    QCOMPARE(actualRuleIds, expectedRuleIds);
}

void compareSingleCandidate(const QString& testName,
                            const CandidateError& actual,
                            const QString& expectedRuleId,
                            const QList<int>& expectedDisplayIds,
                            const QSet<int>& expectedConflictIds)
{
    if (actual.ruleId != expectedRuleId) {
        qDebug() << "[TEST FAIL]" << testName
                 << "ruleId не совпадает:" << actual.ruleId
                 << "expected:" << expectedRuleId;
    }
    QCOMPARE(actual.ruleId, expectedRuleId);

    if (actual.displayTokenIds != expectedDisplayIds) {
        qDebug() << "[TEST FAIL]" << testName
                 << "displayTokenIds не совпадают:" << actual.displayTokenIds
                 << "expected:" << expectedDisplayIds;
    }
    QCOMPARE(actual.displayTokenIds, expectedDisplayIds);

    if (actual.conflictTokenIds != expectedConflictIds) {
        qDebug() << "[TEST FAIL]" << testName
                 << "conflictTokenIds не совпадают:" << actual.conflictTokenIds
                 << "expected:" << expectedConflictIds;
    }
    QCOMPARE(actual.conflictTokenIds, expectedConflictIds);
}
