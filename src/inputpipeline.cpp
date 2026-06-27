/*!
* \file inputpipeline.cpp
* \brief Реализация слоя ввода: чтение файла, парсинг, валидация, построение модели.
*
* Связывает три этапа обработки входных данных в единый конвейер:
* 1. readFile читает файл CoNLL-U в список строк;
* 2. parseAndValidate разбивает строки на блоки предложений и проверяет формат;
* 3. buildModel преобразует RawDocument в DocumentModel с деревьями зависимостей.
* Оркестратор runInput объединяет все три этапа.
*/

#include "inputpipeline.h"
#include "inputmodule.h"
#include "modelbuilder.h"
#include "wordlists.h"

#include <QFile>
#include <QTextStream>
#include <memory>

namespace {

// Создать Diagnostic с заданной категорией и сообщением
Diagnostic makeError(DiagnosticKind kind, const QString& message, int code = -1)
{
    Diagnostic d;
    d.kind = kind;
    d.message = message;
    d.code = code;
    return d;
}

} // namespace

/*!
* \brief Читать входной файл CoNLL-U в список строк.
* \param [in] path Путь ко входному файлу.
* \return QStringList при успехе, Diagnostic{InputFileError} при ошибке доступа.
*/
QStringList readFile(const QString& path)
{
    QFile file(path);
    if (!file.exists()) {
        throw makeError(DiagnosticKind::InputFileError,
                        QStringLiteral("Входной файл не найден: %1").arg(path));
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw makeError(DiagnosticKind::InputFileError,
                        QStringLiteral("Не удалось открыть входной файл: %1").arg(path));
    }

    QStringList lines;
    QTextStream in(&file);
    setUtf8Encoding(in);
    while (!in.atEnd())
        lines.append(in.readLine());
    file.close();
    return lines;
}

// Блок строк одного предложения и номер его первой строки во входном файле.
// Номер строки нужен для сообщений об ошибках парсинга.
struct Block {
    QStringList lines;
    int firstLine;
};

/*!
* \brief Разбить список строк на блоки предложений и проверить формат.
* \param [in] lines Строки входного файла.
* \return RawDocument при успехе, Diagnostic{InputFormatError} при первой ошибке.
*/
RawDocument parseAndValidate(const QStringList& lines)
{
    QList<Block> blocks;
    Block current;
    current.firstLine = 1;

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        if (line.trimmed().isEmpty()) {
            // Пустая строка завершает текущий блок
            if (!current.lines.isEmpty()) {
                blocks.append(current);
                current = Block();
                current.firstLine = i + 2;
            }
        } else {
            // Первая непустая строка фиксирует номер первой строки блока
            if (current.lines.isEmpty())
                current.firstLine = i + 1;
            current.lines.append(line);
        }
    }
    if (!current.lines.isEmpty())
        blocks.append(current);

    if (blocks.isEmpty()) {
        throw makeError(DiagnosticKind::InputFormatError,
                        QStringLiteral("Входной файл не содержит предложений."));
    }

    if (blocks.size() > 1000) {
        throw makeError(DiagnosticKind::InputFormatError,
                        QStringLiteral("Превышено максимальное число предложений (1000): найдено %1.").arg(blocks.size()));
    }

    RawDocument doc;
    for (const Block& block : blocks) {
        RawSentence sentence = parseSentenceBlock(block.lines, block.firstLine);
        validateSentenceStructure(sentence);
        doc.sentences.append(std::move(sentence));
    }

    return doc;
}

/*!
* \brief Преобразовать RawDocument в DocumentModel с деревьями зависимостей.
* \param [in] rawDoc Валидированное входное представление документа.
* \return Модель документа с индексом sentById.
*/
DocumentModel buildModel(const RawDocument& rawDoc)
{
    DocumentModel model;
    for (const RawSentence& raw : rawDoc.sentences) {
        auto sm = std::make_unique<SentenceModel>(buildSentenceModel(raw));
        model.sentences.push_back(std::move(sm));
    }
    // Индекс строится после заполнения списка, когда адреса стабильны
    for (auto& s : model.sentences)
        model.sentById[s->sentId] = s.get();
    return model;
}

/*!
* \brief Оркестратор слоя ввода: readFile, parseAndValidate, buildModel.
* \param [in] runtime Runtime-контекст с путём ко входному файлу.
* \return DocumentModel при успехе, Diagnostic при ошибке любого этапа.
*/
DocumentModel runInput(const CheckerRuntime& runtime)
{
    QStringList lines = readFile(runtime.config.inputPath);
    RawDocument rawDoc = parseAndValidate(lines);
    return buildModel(rawDoc);
}