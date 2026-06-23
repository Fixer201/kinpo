#pragma once

#include "datamodel.h"
#include <QString>
#include <variant>

std::variant<CheckerRuntime, Diagnostic> initializeRuntime(const RunConfig& config);