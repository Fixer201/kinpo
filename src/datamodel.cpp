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
        if (current->upos != Upos::Punct && !current->isMwtFragment)
            return current;
        current = current->nextToken;
    }
    return nullptr;
}

const TokenNode* TokenNode::previousNonPunct() const
{
    const TokenNode* current = previousToken;
    while (current != nullptr) {
        if (current->upos != Upos::Punct && !current->isMwtFragment)
            return current;
        current = current->previousToken;
    }
    return nullptr;
}

// ------------------------------------------------------------------------
// Operators and hashes
// ------------------------------------------------------------------------

bool AtomicEdit::operator==(const AtomicEdit& other) const
{
    return type == other.type &&
           targetTokenIds == other.targetTokenIds &&
           referenceTokenId == other.referenceTokenId &&
           newTokens == other.newTokens;
}

uint qHash(const AtomicEdit& e, uint seed) noexcept
{
    uint h = qHash(static_cast<int>(e.type), seed);
    for (int id : e.targetTokenIds)
        h ^= qHash(id, seed) + 0x9e3779b9;
    h ^= qHash(e.referenceTokenId, seed) + 0x9e3779b9;
    for (const QString& s : e.newTokens)
        h ^= qHash(s, seed) + 0x9e3779b9;
    return h;
}

bool CandidateError::operator==(const CandidateError& other) const
{
    return ruleId == other.ruleId &&
           sentId == other.sentId &&
           conflictTokenIds == other.conflictTokenIds &&
           edits == other.edits;
}

uint qHash(const CandidateError& ce, uint seed) noexcept
{
    uint h = qHash(ce.ruleId, seed);
    h ^= qHash(ce.sentId, seed) + 0x9e3779b9;
    for (int id : ce.conflictTokenIds)
        h ^= qHash(id, seed) + 0x9e3779b9;
    for (const AtomicEdit& e : ce.edits)
        h ^= qHash(e, seed) + 0x9e3779b9;
    return h;
}

bool VerbPrepEntry::operator==(const VerbPrepEntry& other) const
{
    return wrongPrep == other.wrongPrep &&
           action == other.action &&
           prep == other.prep;
}

uint qHash(const VerbPrepEntry& vpe, uint seed) noexcept
{
    uint h = qHash(static_cast<int>(vpe.action), seed);
    if (vpe.wrongPrep)
        h ^= qHash(*vpe.wrongPrep, seed) + 0x9e3779b9;
    if (vpe.prep)
        h ^= qHash(*vpe.prep, seed) + 0x9e3779b9;
    return h;
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
