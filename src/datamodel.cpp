/*!
* \file datamodel.cpp
* \brief Реализация вспомогательных функций для datamodel.h.
*/

#include "datamodel.h"

// ------------------------------------------------------------------------
// TokenNode navigation
// ------------------------------------------------------------------------

const TokenNode* TokenNode::nextNonPunct() const
{
    const TokenNode* current = nextToken;
    while (current != nullptr) {
        if (current->upos != Upos::PUNCT && !current->isMwtFragment)
            return current;
        current = current->nextToken;
    }
    return nullptr;
}

const TokenNode* TokenNode::previousNonPunct() const
{
    const TokenNode* current = previousToken;
    while (current != nullptr) {
        if (current->upos != Upos::PUNCT && !current->isMwtFragment)
            return current;
        current = current->previousToken;
    }
    return nullptr;
}

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
