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
* \brief Проверить, что строка содержит ровно 10 колонок.
* \param [in] cols Колонки, полученные разбиением строки по табуляции.
* \param [in] lineNumber Номер строки во входном файле.
* \return Diagnostic при ошибке, std::nullopt при успехе.
*/
void validateColumnCount(const QStringList& cols, int lineNumber)
{
    if (cols.size() == 10)
        return;
    throw Diagnostic{
        DiagnosticKind::InputFormatError,
        lineNumber,
        std::nullopt,
        QStringLiteral("Ожидается 10 колонок, найдено %1").arg(cols.size()),
        -1
    };
}

/*!
* \brief Разобрать целочисленную колонку с проверкой минимума.
* \param [in] value Строковое значение колонки.
* \param [in] lineNumber Номер строки во входном файле.
* \param [in] minValue Минимально допустимое значение.
* \param [in] errorMessage Сообщение об ошибке при нарушении условия.
* \return Целое значение при успехе, Diagnostic при ошибке.
*/
int parseIntColumn(const QString& value, int lineNumber,
                   int minValue, const QString& errorMessage)
{
    bool ok = false;
    int result = value.toInt(&ok);
    if (!ok || result < minValue) {
        throw Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            errorMessage,
            -1
        };
    }
    return result;
}

/*!
* \brief Проверить, что UPOS входит в справочник допустимых значений.
* \param [in] upos Строковое значение UPOS.
* \param [in] lineNumber Номер строки во входном файле.
* \return Diagnostic при ошибке, std::nullopt при успехе.
*/
void validateUposValue(const QString& upos, int lineNumber)
{
    if (validUpos.contains(upos))
        return;
    throw Diagnostic{
        DiagnosticKind::InputFormatError,
        lineNumber,
        std::nullopt,
        QStringLiteral("Некорректный UPOS"),
        -1
    };
}

/*!
* \brief Проверить, что колонка FORM не пуста.
* \param [in] form Строковое значение FORM.
* \param [in] lineNumber Номер строки во входном файле.
*/
void validateFormValue(const QString& form, int lineNumber)
{
    if (!form.isEmpty())
        return;
    throw Diagnostic{
        DiagnosticKind::InputFormatError,
        lineNumber,
        std::nullopt,
        QStringLiteral("Колонка FORM не может быть пустой"),
        -1
    };
}

/*!
* \brief Проверить строгий порядок ID: 1, 2, 3...
* \param [in] id Текущий ID токена.
* \param [in,out] expectedNextId Ожидаемый следующий ID.
* \param [in] lineNumber Номер строки во входном файле.
*/
void validateIdOrder(int id, int& expectedNextId, int lineNumber)
{
    if (id != expectedNextId) {
        throw Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Нарушен порядок ID"),
            -1
        };
    }
    expectedNextId = id + 1;
}

/*!
* \brief Собрать RawToken из уже проверенных колонок.
* \param [in] cols Колонки исходной строки.
* \param [in] lineNumber Номер строки во входном файле.
* \param [in] id Распарсенный ID токена.
* \param [in] head Распарсенный HEAD токена.
* \return Заполненная структура RawToken.
*/
RawToken assembleRawToken(const QStringList& cols, int lineNumber, int id, int head)
{
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
* \brief Разобрать строку обычного токена CoNLL-U.
* \param [in] line Исходная строка (10 колонок через \t).
* \param [in] lineNumber Номер строки во входном файле (для диагностики).
* \param [in,out] expectedNextId Ожидаемый следующий ID (проверка порядка 1,2,3...).
* \return RawToken при успехе, или Diagnostic{kind=InputFormatError} при ошибке.
*
* Проверяет: ровно 10 колонок, целочисленный ID >= 1, HEAD целое >= 0,
* UPOS в справочнике, FORM не пустое, строгий порядок ID.
*/
RawToken parseTokenLine(const QString& line, int lineNumber, int& expectedNextId)
{
    QStringList cols = line.split('\t');
    validateColumnCount(cols, lineNumber);

    const int id = parseIntColumn(cols[0], lineNumber, 1,
                                  QStringLiteral("Некорректный ID"));
    const int head = parseIntColumn(cols[6], lineNumber, 0,
                                    QStringLiteral("Некорректный HEAD"));

    validateUposValue(cols[3], lineNumber);
    validateFormValue(cols[1], lineNumber);
    validateIdOrder(id, expectedNextId, lineNumber);

    return assembleRawToken(cols, lineNumber, id, head);
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
MwtRecord parseMwtLine(const QString& line, int lineNumber)
{
    QStringList cols = line.split('\t');
    if (cols.size() != 10) {
        throw Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Ожидается 10 колонок, найдено %1").arg(cols.size()),
            -1
        };
    }

    QStringList parts = cols[0].split('-');
    if (parts.size() != 2) {
        throw Diagnostic{
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
        throw Diagnostic{
            DiagnosticKind::InputFormatError,
            lineNumber,
            std::nullopt,
            QStringLiteral("Некорректный диапазон MWT"),
            -1
        };
    }

    for (int i = 2; i < cols.size(); ++i) {
        if (cols[i] != QStringLiteral("_")) {
            throw Diagnostic{
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

/*!
* \brief Разобрать один блок строк CoNLL-U в RawSentence.
* \param [in] block Список строк одного предложения (без пустых разделителей).
* \param [in] firstLineNumber Номер первой строки блока во входном файле.
* \return Разобранное предложение RawSentence.
* \throws Diagnostic{InputFormatError} при ошибке формата.
*
* Разделяет строки на комментарии (# sent_id, # text), токены и MWT.
* Выполняет локальную валидацию каждой строки.
*/
RawSentence parseSentenceBlock(const QStringList& block, int firstLineNumber)
{
    RawSentence sentence;
    sentence.firstLineNumber = firstLineNumber;

    bool hasSentId = false;
    bool hasText = false;
    int expectedNextId = 1;

    for (int i = 0; i < block.size(); ++i) {
        const QString& line = block[i];
        int lineNumber = firstLineNumber + i;

        if (!line.isEmpty()) {
            if (line.startsWith('#')) {
                // Комментарий должен начинаться с "# ", иначе это ошибка формата
                if (!line.startsWith(QStringLiteral("# "))) {
                    throw Diagnostic{
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
            } else {
                // Строка данных — должна содержать табуляции (токен или MWT)
                if (!line.contains('\t')) {
                    throw Diagnostic{
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
                    sentence.mwtRecords.append(parseMwtLine(line, lineNumber));
                } else {
                    // Разбираем обычный токен и добавляем в список
                    sentence.tokens.append(parseTokenLine(line, lineNumber, expectedNextId));
                }
            }
        }
    }

    // Проверяем, что в блоке были обязательные комментарии
    if (!hasSentId) {
        throw Diagnostic{
            DiagnosticKind::InputFormatError,
            firstLineNumber,
            std::nullopt,
            QStringLiteral("Отсутствует '# sent_id'"),
            -1
        };
    }

    if (!hasText) {
        throw Diagnostic{
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
void validateSentenceStructure(const RawSentence& sentence)
{
    const int tokenCount = static_cast<int>(sentence.tokens.size());

    // 1. Лимит токенов
    if (tokenCount > 200) {
        throw makeDiag(sentence, sentence.firstLineNumber,
                       QStringLiteral("Превышен максимум в 200 токенов"));
    }

    const auto idToToken = buildTokenIndex(sentence);

    // 2. HEAD: диапазон, существование, self-loop; параллельно считаем корни
    int rootCount = 0;
    for (const auto& token : sentence.tokens) {
        if (token.headId < 0) {
            throw makeDiag(sentence, token.lineNumber,
                           QStringLiteral("Некорректный HEAD"));
        }
        if (token.headId != 0 && token.headId > tokenCount) {
            throw makeDiag(sentence, token.lineNumber,
                           QStringLiteral("HEAD %1 ссылается на несуществующий токен")
                               .arg(token.headId));
        }
        if (token.headId == token.id) {
            throw makeDiag(sentence, token.lineNumber,
                           QStringLiteral("HEAD образует петлю"));
        }
        if (token.headId == 0)
            ++rootCount;
    }

    // 3. Проверяем количество корней
    if (rootCount == 0) {
        throw makeDiag(sentence, sentence.firstLineNumber,
                       QStringLiteral("Не найден корневой токен"));
    }
    if (rootCount > 1) {
        throw makeDiag(sentence, sentence.firstLineNumber,
                       QStringLiteral("Несколько корней"));
    }

    // 4. Проверяем отсутствие циклов (DFS с 3 цветами)
    QHash<int, int> color;
    for (const auto& t : sentence.tokens)
        color[t.id] = 0;

    for (const auto& token : sentence.tokens) {
        if (color[token.id] == 0) {
            int current = token.id;
            bool reachedProcessed = false;
            while (current != 0 && !reachedProcessed) {
                int state = color.value(current, 0);
                if (state == 2) {
                    reachedProcessed = true;
                } else if (state == 1) {
                    throw makeDiag(sentence, token.lineNumber,
                                   QStringLiteral("HEAD образует петлю"));
                } else {
                    color[current] = 1;
                    current = idToToken.value(current)->headId;
                }
            }

            int fin = token.id;
            while (fin != 0 && color.value(fin, 0) == 1) {
                color[fin] = 2;
                fin = idToToken.value(fin)->headId;
            }
        }
    }

    // 5. Проверяем MWT
    for (const auto& mwt : sentence.mwtRecords) {
        for (int id = mwt.rangeStart; id <= mwt.rangeEnd; ++id) {
            if (!idToToken.contains(id)) {
                throw makeDiag(sentence, mwt.lineNumber,
                               QStringLiteral("Некорректный диапазон MWT"));
            }
        }
    }
}
