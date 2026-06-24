/*!
* \file outputmodule.cpp
* \brief Реализация слоя вывода: формирование исправления и запись результата.
*
* buildCorrection применяет edits к displayTokenIds для формирования строки
* «Исправление». writeOutput форматирует все ошибки в табличный вывод
* согласно внешней спецификации (раздел 2.4.3).
*/

#include "outputmodule.h"
#include "wordlists.h"
#include <QFile>
#include <QTextStream>
#include <algorithm>
#include <climits>

/*!
* \brief Элемент при построении строки исправления.
*
* Каждый элемент хранит ID оригинального токена (0 для вставленных)
* и текст. Список таких элементов последовательно модифицируется правками.
*/
struct CorrectionElem {
    int originalId = 0;
    QString text;
};

/*!
* \brief Найти displayTokenIds, отсортированные по возрастанию.
*/
static QList<int> sortedDisplayIds(const CandidateError& ce)
{
    QList<int> ids = ce.displayTokenIds;
    std::sort(ids.begin(), ids.end());
    return ids;
}

// ---------------------------------------------------------------------------
// buildCorrection
// ---------------------------------------------------------------------------

/*!
* \brief Применить edits к displayTokenIds и сформировать строку исправления.
*
* Алгоритм:
*   1. Построить начальный список элементов из оригинальных токенов.
*   2. Для каждой правки модифицировать список (удаление, замена, вставка).
*   3. Склеить финальный список через пробел.
*/
QString buildCorrection(const CandidateError& ce,
                         const SentenceModel& sentence)
{
    QList<int> ids = sortedDisplayIds(ce);
    if (ids.isEmpty())
        return QString();

    // 1. Начальный список элементов
    QList<CorrectionElem> elems;
    for (int id : ids) {
        auto it = sentence.tokensById.find(id);
        QString form = (it != sentence.tokensById.end()) ? (*it)->form : QString();
        elems.append({id, form});
    }

    // 2. Применяем правки
    for (const AtomicEdit& edit : ce.edits) {
        switch (edit.type) {
        case AtomicEditType::DeleteTokens: {
            QSet<int> toDelete(edit.targetTokenIds.begin(), edit.targetTokenIds.end());
            QList<CorrectionElem> kept;
            for (const CorrectionElem& e : elems)
                if (!toDelete.contains(e.originalId))
                    kept.append(e);
            elems = kept;
            break;
        }
        case AtomicEditType::ReplaceTokens: {
            QSet<int> toReplace(edit.targetTokenIds.begin(), edit.targetTokenIds.end());
            QString replacement = edit.newTokens.isEmpty() ? QString() : edit.newTokens.first();
            for (CorrectionElem& e : elems)
                if (toReplace.contains(e.originalId))
                    e.text = replacement;
            break;
        }
        case AtomicEditType::InsertBefore: {
            int idx = -1;
            for (int i = 0; i < elems.size(); ++i)
                if (elems[i].originalId == edit.referenceTokenId) { idx = i; break; }
            if (idx >= 0)
                for (int j = edit.newTokens.size() - 1; j >= 0; --j)
                    elems.insert(idx, {0, edit.newTokens[j]});
            break;
        }
        case AtomicEditType::InsertAfter: {
            int idx = -1;
            for (int i = 0; i < elems.size(); ++i)
                if (elems[i].originalId == edit.referenceTokenId) { idx = i; break; }
            if (idx >= 0)
                for (int j = 0; j < edit.newTokens.size(); ++j)
                    elems.insert(idx + 1 + j, {0, edit.newTokens[j]});
            break;
        }
        }
    }

    // 3. Склеиваем, пропуская пустые элементы
    QStringList result;
    for (const CorrectionElem& e : elems)
        if (!e.text.isEmpty())
            result.append(e.text);
    return result.join(QStringLiteral(" "));
}

// ---------------------------------------------------------------------------
// writeOutput — вспомогательные функции
// ---------------------------------------------------------------------------

/*!
* \brief Проверить, являются ли ID смежным диапазоном.
*/
static bool isContiguousRange(const QList<int>& ids)
{
    if (ids.size() < 2)
        return false;
    for (int i = 1; i < ids.size(); ++i)
        if (ids[i] != ids[i - 1] + 1)
            return false;
    return true;
}

/*!
* \brief Строка позиции: "4", "3-5" или "2, 4, 6".
*/
static QString formatPosition(const QList<int>& ids)
{
    if (ids.isEmpty())
        return QString();
    if (ids.size() == 1)
        return QString::number(ids.first());
    if (isContiguousRange(ids))
        return QString::number(ids.first()) + QStringLiteral("-") + QString::number(ids.last());
    QStringList parts;
    for (int id : ids)
        parts.append(QString::number(id));
    return parts.join(QStringLiteral(", "));
}

/*!
* \brief Фрагмент: конкатенация FORM токенов через пробел.
*/
static QString formatFragment(const QList<int>& ids, const SentenceModel& sentence)
{
    QStringList forms;
    for (int id : ids) {
        auto it = sentence.tokensById.find(id);
        if (it != sentence.tokensById.end())
            forms.append((*it)->form);
    }
    return forms.join(QStringLiteral(" "));
}

/*!
* \brief Заменить плейсхолдеры {KEY} в шаблоне.
*/
static QString fillTemplate(const QString& templateStr,
                            const QHash<QString, QString>& params)
{
    QString result = templateStr;
    for (auto it = params.begin(); it != params.end(); ++it)
        result.replace(QStringLiteral("{") + it.key() + QStringLiteral("}"), it.value());
    return result;
}

/*!
* \brief Строка вывода одной ошибки.
*
* Формат: [sent_id] | [ruleId] | [position] | [fragment] | [correction] | [description]
*/
static QString formatErrorLine(const CandidateError& ce,
                                const SentenceModel& sentence,
                                const Rule* rule)
{
    QList<int> ids = sortedDisplayIds(ce);

    QStringList parts;
    parts << ce.sentId
          << ce.ruleId
          << formatPosition(ids)
          << formatFragment(ids, sentence)
          << buildCorrection(ce, sentence);

    QString description;
    if (rule) {
        QString tmpl = rule->descriptionTemplate();
        if (!tmpl.isEmpty())
            description = fillTemplate(tmpl, ce.messageParams);
    }
    parts << description;

    return parts.join(QStringLiteral(" | "));
}

/*!
* \brief Сортировка ошибок.
*
* Порядок: по порядку предложений, затем по первому ID токена,
* затем лексикографически по ruleId.
*/
static QList<CandidateError> sortErrors(const QSet<CandidateError>& errors,
                                         const DocumentModel& document)
{
    QHash<QString, int> sentOrder;
    for (int i = 0; i < static_cast<int>(document.sentences.size()); ++i)
        sentOrder[document.sentences[i]->sentId] = i;

    QList<CandidateError> sorted(errors.begin(), errors.end());
    std::sort(sorted.begin(), sorted.end(),
              [&sentOrder](const CandidateError& a, const CandidateError& b) {
                  int orderA = sentOrder.value(a.sentId, INT_MAX);
                  int orderB = sentOrder.value(b.sentId, INT_MAX);
                  if (orderA != orderB) return orderA < orderB;
                  int firstA = a.displayTokenIds.isEmpty() ? 0
                              : *std::min_element(a.displayTokenIds.begin(), a.displayTokenIds.end());
                  int firstB = b.displayTokenIds.isEmpty() ? 0
                              : *std::min_element(b.displayTokenIds.begin(), b.displayTokenIds.end());
                  if (firstA != firstB) return firstA < firstB;
                  return a.ruleId < b.ruleId;
              });
    return sorted;
}

/*!
* \brief Найти правило по ruleId в dispatch.
*/
static const Rule* findRule(const QString& ruleId, const CheckerRuntime& runtime)
{
    for (auto it = runtime.dispatch.begin(); it != runtime.dispatch.end(); ++it)
        for (const Rule* rule : it.value())
            if (rule->ruleId() == ruleId)
                return rule;
    return nullptr;
}

/*!
* \brief Список форматированных строк вывода.
*/
static QStringList formatErrors(const QSet<CandidateError>& errors,
                                 const DocumentModel& document,
                                 const CheckerRuntime& runtime)
{
    if (errors.isEmpty())
        return {QStringLiteral("NO ERRORS FOUND")};

    QList<CandidateError> sorted = sortErrors(errors, document);
    QStringList lines;
    for (const CandidateError& ce : sorted) {
        auto sentIt = document.sentById.find(ce.sentId);
        const SentenceModel* sentence = (sentIt != document.sentById.end())
                                        ? sentIt.value() : nullptr;

        if (!sentence) {
            lines.append(ce.sentId + QStringLiteral(" | ") + ce.ruleId
                         + QStringLiteral(" | ???"));
            continue;
        }

        const Rule* rule = findRule(ce.ruleId, runtime);
        lines.append(formatErrorLine(ce, *sentence, rule));
    }
    return lines;
}

/*!
* \brief Записать отформатированные строки в выходной файл.
* \return std::nullopt при успехе, Diagnostic{OutputWriteError} при ошибке.
*/
std::optional<Diagnostic> writeOutput(const QSet<CandidateError>& errors,
                                      const DocumentModel& document,
                                      const CheckerRuntime& runtime)
{
    QStringList lines = formatErrors(errors, document, runtime);

    // Атомарная запись: пишем во временный файл, затем переименовываем.
    const QString& outputPath = runtime.config.outputPath;
    QString tmpPath = outputPath + QStringLiteral(".tmp");

    QFile tmpFile(tmpPath);
    if (!tmpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Diagnostic d;
        d.kind = DiagnosticKind::OutputWriteError;
        d.message = QStringLiteral("Невозможно создать выходной файл: %1")
                     .arg(outputPath);
        d.code = -1;
        return d;
    }

    QTextStream out(&tmpFile);
    setUtf8Encoding(out);

    for (const QString& line : lines)
        out << line << Qt::endl;

    out.flush();
    tmpFile.close();

    // Удаляем существующий целевой файл, если он есть, иначе rename может провалиться на некоторых платформах.
    if (QFile::exists(outputPath)) {
        if (!QFile::remove(outputPath)) {
            QFile::remove(tmpPath);
            Diagnostic d;
            d.kind = DiagnosticKind::OutputWriteError;
            d.message = QStringLiteral("Невозможно заменить выходной файл: %1")
                         .arg(outputPath);
            d.code = -1;
            return d;
        }
    }

    if (!QFile::rename(tmpPath, outputPath)) {
        QFile::remove(tmpPath);
        Diagnostic d;
        d.kind = DiagnosticKind::OutputWriteError;
        d.message = QStringLiteral("Невозможно завершить запись выходного файла: %1")
                     .arg(outputPath);
        d.code = -1;
        return d;
    }

    return std::nullopt;
}
