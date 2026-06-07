/*!
* \file datamodel.cpp
* \brief Реализация вспомогательных функций для datamodel.h.
*/

#include "datamodel.h"

QString diagnosticKindToString(DiagnosticKind kind)
{
    switch (kind) {
    case DiagnosticKind::CliUsageError:     return QStringLiteral("CliUsageError");
    case DiagnosticKind::ResourceLoadError: return QStringLiteral("ResourceLoadError");
    case DiagnosticKind::InputFileError:    return QStringLiteral("InputFileError");
    case DiagnosticKind::InputFormatError:  return QStringLiteral("InputFormatError");
    case DiagnosticKind::OutputWriteError:  return QStringLiteral("OutputWriteError");
    case DiagnosticKind::InternalError:       return QStringLiteral("InternalError");
    }
    return QStringLiteral("Unknown");
}
