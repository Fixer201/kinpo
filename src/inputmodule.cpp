/*!
* \file inputmodule.cpp
* \brief Реализация модуля ввода для чтения CoNLL-U блоков.
*
* Содержит parseSentenceBlock и validateSentenceStructure, а также
* вспомогательные функции parseTokenLine, parseMwtLine, buildTokenIndex,
* makeDiag для разбора строк токенов, MWT и построения индексов.
*/

#include "inputmodule.h"

#include <QSet>
#include <QStringList>

namespace {

/*!
* \var validUpos
* \brief Справочник допустимых значений UPOS согласно CoNLL-U.
*/
const QSet<QString> validUpos = {
    QStringLiteral("ADJ"), QStringLiteral("ADP"), QStringLiteral("ADV"),
    QStringLiteral("AUX"), QStringLiteral("CCONJ"), QStringLiteral("DET"),
    QStringLiteral("INTJ"), QStringLiteral("NOUN"), QStringLiteral("NUM"),
    QStringLiteral("PART"), QStringLiteral("PRON"), QStringLiteral("PROPN"),
    QStringLiteral("PUNCT"), QStringLiteral("SCONJ"), QStringLiteral("SYM"),
    QStringLiteral("VERB"), QStringLiteral("X")
};

/*!
* \brief Разобрать строку обычного токена CoNLL-U.
* \param [in] line Исходная строка (10 колонок через \t).
* \param [in] lineNumber Номер строки во входном файле (для диагностики).
* \param [in,out] expectedNextId Ожидаемый следующий ID (проверка порядка 1,2,3...).
* \return RawToken при успехе, или Diagnostic{kind=InputFormatError} при ошибке.
*
* Проверяет: ровно 10 колонок, целочисленный ID >= 1, HEAD целое >= 0,
* UPOS в справочнике, FORM не пустое, строгий порядок ID.
*/
std::variant<RawToken, Diagnostic> parseTokenLine(
    const QString& line, int lineNumber, int& expectedNextId)
{
    // Разбиваем строку по табуляциям  должно получиться ровно 10 колонок
    QStringList cols = line.split('\t');
    if (cols.size() != 10) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Ожидается 10 колонок, найдено %1").arg(cols.size()),
            -1
        };
    }

    // Парсим ID: должен быть целым числом >= 1
    bool ok = false;
    int id = cols[0].toInt(&ok);
    if (!ok || id < 1) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Некорректный ID"),
            -1
        };
    }

    // Парсим HEAD: должен быть целым числом >= 0
    int head = cols[6].toInt(&ok);
    if (!ok || head < 0) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Некорректный HEAD"),
            -1
        };
    }

    // Проверяем UPOS: должен быть в списке допустимых значений
    if (!validUpos.contains(cols[3])) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Некорректный UPOS"),
            -1
        };
    }

    // Проверяем FORM: не должен быть пустым
    if (cols[1].isEmpty()) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Колонка FORM не может быть пустой"),
            -1
        };
    }

    // Проверяем порядок ID: должен строго следовать 1, 2, 3...
    if (id != expectedNextId) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Нарушен порядок ID"),
            -1
        };
    }
    // Сдвигаем ожидаемый ID для следующего токена
    expectedNextId = id + 1;

    // Заполняем структуру RawToken из распарсенных колонок
    RawToken token;
    token.lineNumber = lineNumber;
    token.id = id;
    token.form = cols[1];
    token.lemma = cols[2];
    token.upos = cols[3];
    token.xpos = cols[4];
    token.featsRaw = cols[5];
    token.headId = head;
    token.deprel = cols[7];
    token.depsRaw = cols[8];
    token.miscRaw = cols[9];
    return token;
}

/*!
* \brief Разобрать строку MWT (Multi-Word Token) в формате CoNLL-U.
* \param [in] line Исходная строка (ID = N-M, 10 колонок).
* \param [in] lineNumber Номер строки во входном файле (для диагностики).
* \return MwtRecord при успехе, или Diagnostic{kind=InputFormatError} при ошибке.
*
* Проверяет: ровно 10 колонок, ID вида N-M с N < M,
* все колонки кроме ID и FORM содержат '_'.
*/
std::variant<MwtRecord, Diagnostic> parseMwtLine(
    const QString& line, int lineNumber)
{
    QStringList cols = line.split('\t');
    if (cols.size() != 10) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Ожидается 10 колонок, найдено %1").arg(cols.size()),
            -1
        };
    }

    QStringList parts = cols[0].split('-');
    if (parts.size() != 2) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Некорректный диапазон MWT"),
            -1
        };
    }

    bool ok1 = false, ok2 = false;
    int rangeStart = parts[0].toInt(&ok1);
    int rangeEnd = parts[1].toInt(&ok2);
    if (!ok1 || !ok2 || rangeStart >= rangeEnd) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Некорректный диапазон MWT"),
            -1
        };
    }

    for (int i = 2; i < cols.size(); ++i) {
        if (cols[i] != QStringLiteral("_")) {
            return Diagnostic{
                DiagnosticKind::InputFormatError,
                lineNumber,
                std::nullopt,
                QStringLiteral("Составной токен требует '_' в колонке"),
                -1
            };
        }
    }

    MwtRecord rec;
    rec.lineNumber = lineNumber;
    rec.rangeStart = rangeStart;
    rec.rangeEnd = rangeEnd;
    rec.form = cols[1];
    return rec;
}

/*!
* \brief Собрать Diagnostic с привязкой к предложению.
* \param [in] sentence Предложение, для которого формируется ошибка.
* \param [in] lineNumber Номер строки, к которой привязана ошибка.
* \param [in] message Сообщение об ошибке.
* \return Объект Diagnostic{kind=InputFormatError, ...}.
*/
Diagnostic makeDiag(const RawSentence& sentence, int lineNumber, const QString& message)
{
    return Diagnostic{
        DiagnosticKind::InputFormatError,
        lineNumber,
        sentence.sentId,
        message,
        -1
    };
}

/*!
* \brief Построить индекс ID → указатель на токен.
* \param [in] sentence Предложение с заполненным списком tokens.
* \return Хеш-таблица для O(1) доступа по ID.
*/
QHash<int, const RawToken*> buildTokenIndex(const RawSentence& sentence)
{
    QHash<int, const RawToken*> index;
    index.reserve(sentence.tokens.size());
    for (const auto& t : sentence.tokens) {
        index[t.id] = &t;
    }
    return index;
}

} // namespace

std::variant<RawSentence, Diagnostic> parseSentenceBlock(
    const QStringList& block, int firstLineNumber)
{
    RawSentence sentence;
    sentence.firstLineNumber = firstLineNumber;

    bool hasSentId = false;
    bool hasText = false;
    int expectedNextId = 1;

    for (int i = 0; i < block.size(); ++i) {
        const QString& line = block[i];
        int lineNumber = firstLineNumber + i;

        // Пропускаем пустые строки внутри блока
        if (line.isEmpty()) {
            continue;
        }

        // Обрабатываем комментарии (строки, начинающиеся с '#')
        if (line.startsWith('#')) {
            // Комментарий должен начинаться с "# ", иначе это ошибка формата
            if (!line.startsWith(QStringLiteral("# "))) {
                return Diagnostic{
                    DiagnosticKind::InputFormatError,
                    lineNumber,
                    std::nullopt,
                    QStringLiteral("Ошибка формата комментария"),
                    -1
                };
            }

            // Извлекаем содержимое после "# "
            QString content = line.mid(2);
            if (content.startsWith(QStringLiteral("sent_id = "))) {
                sentence.sentId = content.mid(10).trimmed();
                hasSentId = true;
            } else if (content.startsWith(QStringLiteral("text = "))) {
                sentence.text = content.mid(7).trimmed();
                hasText = true;
            }
            continue;
        }

        // Строка данных — должна содержать табуляции (токен или MWT)
        if (!line.contains('\t')) {
            return Diagnostic{
                DiagnosticKind::InputFormatError,
                lineNumber,
                std::nullopt,
                QStringLiteral("Ожидается 10 колонок, найдено 1"),
                -1
            };
        }

        // Определяем тип строки: MWT (ID содержит '-') или обычный токен
        QStringList cols = line.split('\t');
        QString firstCol = cols[0];

        if (firstCol.contains('-')) {
            // Разбираем MWT и добавляем в список
            auto mwtResult = parseMwtLine(line, lineNumber);
            if (std::holds_alternative<Diagnostic>(mwtResult)) {
                return std::get<Diagnostic>(mwtResult);
            }
            sentence.mwtRecords.append(std::get<MwtRecord>(mwtResult));
        } else {
            // Разбираем обычный токен и добавляем в список
            auto tokenResult = parseTokenLine(line, lineNumber, expectedNextId);
            if (std::holds_alternative<Diagnostic>(tokenResult)) {
                return std::get<Diagnostic>(tokenResult);
            }
            sentence.tokens.append(std::get<RawToken>(tokenResult));
        }
    }

    // Проверяем, что в блоке были обязательные комментарии
    if (!hasSentId) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            firstLineNumber,
            std::nullopt,
            QStringLiteral("Отсутствует '# sent_id'"),
            -1
        };
    }

    if (!hasText) {
        return Diagnostic{
            DiagnosticKind::InputFormatError,
            firstLineNumber,
            std::nullopt,
            QStringLiteral("Отсутствует '# text'"),
            -1
        };
    }

    return sentence;
}

/*!
 * \brief Проверить корректность структуры предложения по условиям,
 *        для которых нужно знать все токены сразу.
 * \param [in] sentence Валидированное предложение (после parseSentenceBlock).
 * \return std::nullopt если ошибок нет; Diagnostic{kind=InputFormatError} если структура нарушена.
 *
 * Выполняет следующие проверки:
 *  - число токенов не превышает 200;
 *  - для каждого токена HEAD находится в допустимом множестве
 *    ({0} ∪ ID существующих токенов) и не образует self-loop;
 *  - в дереве зависимостей ровно один корневой токен (HEAD=0);
 *  - нет циклов в дереве зависимостей (обход с маркировкой "в пути" / "завершён");
 *  - для каждой MWT-записи все ID из диапазона [rangeStart, rangeEnd]
 *    присутствуют в предложении и образуют непрерывную последовательность.
 */
std::optional<Diagnostic> validateSentenceStructure(const RawSentence& sentence)
{
    const int tokenCount = static_cast<int>(sentence.tokens.size());

    // 1. Лимит токенов
    if (tokenCount > 200) {
        return makeDiag(sentence, sentence.firstLineNumber,
                        QStringLiteral("Превышен максимум в 200 токенов"));
    }

    const auto idToToken = buildTokenIndex(sentence);

    // 2. HEAD: диапазон, существование, self-loop; параллельно считаем корни
    int rootCount = 0;
    for (const auto& token : sentence.tokens) {
        if (token.headId < 0) {
            return makeDiag(sentence, token.lineNumber,
                            QStringLiteral("Некорректный HEAD"));
        }
        if (token.headId != 0 && token.headId > tokenCount) {
            return makeDiag(sentence, token.lineNumber,
                            QStringLiteral("HEAD %1 ссылается на несуществующий токен")
                                .arg(token.headId));
        }
        if (token.headId == token.id) {
            return makeDiag(sentence, token.lineNumber,
                            QStringLiteral("HEAD образует петлю"));
        }
        if (token.headId == 0) {
            ++rootCount;
        }
    }

    // 3. Проверяем количество корней
    if (rootCount == 0) {
        return makeDiag(sentence, sentence.firstLineNumber,
                        QStringLiteral("Не найден корневой токен"));
    }
    if (rootCount > 1) {
        return makeDiag(sentence, sentence.firstLineNumber,
                        QStringLiteral("Несколько корней"));
    }

    // 4. Проверяем отсутствие циклов (DFS с 3 цветами)
    QHash<int, int> color;
    for (const auto& t : sentence.tokens) {
        color[t.id] = 0;
    }

    for (const auto& token : sentence.tokens) {
        if (color[token.id] != 0)
            continue;

        int current = token.id;
        while (current != 0) {
            int state = color.value(current, 0);
            if (state == 2)
                break;
            if (state == 1)
                return makeDiag(sentence, token.lineNumber,
                                QStringLiteral("HEAD образует петлю"));

            color[current] = 1;
            current = idToToken.value(current)->headId;
        }

        int fin = token.id;
        while (fin != 0 && color.value(fin, 0) == 1) {
            color[fin] = 2;
            fin = idToToken.value(fin)->headId;
        }
    }

    // 5. Проверяем MWT
    for (const auto& mwt : sentence.mwtRecords) {
        for (int id = mwt.rangeStart; id <= mwt.rangeEnd; ++id) {
            if (!idToToken.contains(id))
                return makeDiag(sentence, mwt.lineNumber,
                                QStringLiteral("Некорректный диапазон MWT"));
        }
    }

    return std::nullopt;
}
