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

// Читает входной файл в список строк. При отсутствии файла или ошибке доступа
// возвращает Diagnostic с категорией InputFileError.
std::variant<QStringList, Diagnostic> readFile(const QString& path)
{
    QFile file(path);
    if (!file.exists())
        return makeError(DiagnosticKind::InputFileError,
                         QStringLiteral("Входной файл не найден: %1").arg(path));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return makeError(DiagnosticKind::InputFileError,
                         QStringLiteral("Не удалось открыть входной файл: %1").arg(path));

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

// Разбивает список строк на блоки по пустым строкам, парсит каждый блок
// через parseSentenceBlock и проверяет структуру через validateSentenceStructure.
// Возвращает RawDocument при успехе или Diagnostic при первой ошибке.
std::variant<RawDocument, Diagnostic> parseAndValidate(const QStringList& lines)
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

    if (blocks.isEmpty())
        return makeError(DiagnosticKind::InputFormatError,
                         QStringLiteral("Входной файл не содержит предложений."));

    if (blocks.size() > 1000)
        return makeError(DiagnosticKind::InputFormatError,
                         QStringLiteral("Превышено максимальное число предложений (1000): найдено %1.").arg(blocks.size()));

    RawDocument doc;
    for (const Block& block : blocks) {
        auto result = parseSentenceBlock(block.lines, block.firstLine);
        if (std::holds_alternative<Diagnostic>(result))
            return std::get<Diagnostic>(result);

        RawSentence sentence = std::get<RawSentence>(result);
        auto diag = validateSentenceStructure(sentence);
        if (diag.has_value())
            return diag.value();

        doc.sentences.append(std::move(sentence));
    }

    return doc;
}

// Преобразует RawDocument в DocumentModel: строит SentenceModel для каждого
// предложения, затем индекс sentById для доступа по идентификатору.
// unique_ptr гарантирует стабильность адресов TokenNode.
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

// Оркестратор слоя ввода: readFile -> parseAndValidate -> buildModel.
// При ошибке на любом этапе возвращает Diagnostic.
std::variant<DocumentModel, Diagnostic> runInput(const CheckerRuntime& runtime)
{
    auto fileResult = readFile(runtime.config.inputPath);
    if (std::holds_alternative<Diagnostic>(fileResult))
        return std::get<Diagnostic>(fileResult);

    const QStringList& lines = std::get<QStringList>(fileResult);

    auto parseResult = parseAndValidate(lines);
    if (std::holds_alternative<Diagnostic>(parseResult))
        return std::get<Diagnostic>(parseResult);

    const RawDocument& rawDoc = std::get<RawDocument>(parseResult);
    return buildModel(rawDoc);
}