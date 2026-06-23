#include "runtimemodule.h"
#include "wordlists.h"

#include "rule_art001.h"
#include "rule_art002.h"
#include "rule_art003.h"
#include "rule_art004.h"
#include "rule_art005a.h"
#include "rule_art006.h"
#include "rule_det001.h"
#include "rule_det002.h"
#include "rule_det003.h"
#include "rule_det004.h"
#include "rule_det005.h"
#include "rule_prep001.h"
#include "rule_prep002.h"
#include "rule_prep003.h"
#include "rule_prep004.h"
#include "rule_prep005.h"
#include "rule_conj001.h"
#include "rule_conj002.h"
#include "rule_conj003.h"
#include "rule_conj004.h"
#include "rule_conj005.h"
#include "rule_conj006.h"
#include "rule_aux001.h"
#include "rule_aux002.h"
#include "rule_aux003.h"
#include "rule_aux004.h"
#include "rule_aux005.h"
#include "rule_aux006.h"
#include "rule_other001.h"
#include "rule_other002.h"
#include "rule_other003.h"
#include "rule_other004.h"
#include "rule_other005.h"
#include "rule_other006.h"

#include <QList>
#include <QDebug>

namespace {

const QList<const Rule*>& allRules()
{
    static const QList<const Rule*> rules = {
        &Rule_ART001::instance(),
        &Rule_ART002::instance(),
        &Rule_ART003::instance(),
        &Rule_ART004::instance(),
        &Rule_ART005a::instance(),
        &Rule_ART006::instance(),
        &Rule_DET001::instance(),
        &Rule_DET002::instance(),
        &Rule_DET003::instance(),
        &Rule_DET004::instance(),
        &Rule_DET005::instance(),
        &Rule_PREP001::instance(),
        &Rule_PREP002::instance(),
        &Rule_PREP003::instance(),
        &Rule_PREP004::instance(),
        &Rule_PREP005::instance(),
        &Rule_CONJ001::instance(),
        &Rule_CONJ002::instance(),
        &Rule_CONJ003::instance(),
        &Rule_CONJ004::instance(),
        &Rule_CONJ005::instance(),
        &Rule_CONJ006::instance(),
        &Rule_AUX001::instance(),
        &Rule_AUX002::instance(),
        &Rule_AUX003::instance(),
        &Rule_AUX004::instance(),
        &Rule_AUX005::instance(),
        &Rule_AUX006::instance(),
        &Rule_OTHER001::instance(),
        &Rule_OTHER002::instance(),
        &Rule_OTHER003::instance(),
        &Rule_OTHER004::instance(),
        &Rule_OTHER005::instance(),
        &Rule_OTHER006::instance(),
    };
    return rules;
}

void loadPriorityIndex(PriorityIndex& idx)
{
    idx.conditionsByHigherRule[QStringLiteral("DET-001")][QStringLiteral("ART-006")] = PriorityConditionKind::Always;
    idx.conditionsByHigherRule[QStringLiteral("PREP-004")][QStringLiteral("AUX-002")] = PriorityConditionKind::Always;
    idx.conditionsByHigherRule[QStringLiteral("ART-003")][QStringLiteral("ART-001")] = PriorityConditionKind::Art003LanguageCase;
}

} // namespace

std::variant<CheckerRuntime, Diagnostic> initializeRuntime(const RunConfig& config)
{
    CheckerRuntime runtime;
    runtime.config = config;

    for (const Rule* rule : allRules()) {
        for (Upos upos : rule->anchorUpos()) {
            runtime.dispatch[upos].insert(rule);
        }
    }

    loadPriorityIndex(runtime.priorityIndex);

    QString listsDir = config.listsDir.value_or(QString());
    auto [res, warns] = loadResources(listsDir);
    for (const QString& w : warns)
        qWarning() << "[initializeRuntime]" << w;
    runtime.resources = std::move(res);

    return runtime;
}