/*!
* \file TEST_buildSentenceModel.cpp
* \brief DDT-тесты для функции buildSentenceModel.
*
* Проверяет корректность построения SentenceModel из одного RawSentence:
*  - структура дерева (корень, количество токенов);
*  - линейная навигация (nextToken, nextNonPunct);
*  - типизированные UPOS и DEPREL (enum);
*  - парсинг FEATS (Number, VerbForm, Degree, Case, булевы флаги);
*  - флаг MWT (isMwtFragment).
*/

#include "TEST_Buildsentencemodel.h"
#include "auxiliaryfunctionsfortesting.h"
#include "inputmodule.h"
#include <QTest>

TEST_BuildSentenceModel::TEST_BuildSentenceModel() {}
TEST_BuildSentenceModel::~TEST_BuildSentenceModel() {}

// ========================================================================
// 1. Структура дерева
// ========================================================================

void TEST_BuildSentenceModel::TestStructure_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("expectedRootId");
    QTest::addColumn<int>("expectedTokenCount");

    // Тест 1.1
    {
        RawSentence s = makeRawSentence(1, "test", "Word");
        addToken(s, makeRawToken(1, 1, "Word", "NOUN", 0, "root"));
        QTest::addRow("1.1_one_token") << s << 1 << 1;
    }

    // Тест 1.2
    {
        RawSentence s = makeRawSentence(1, "test", "What if Google Morphed Into GoogleOS?");
        addToken(s, makeRawToken(1, 1, "What",    "PRON",  4, "mark"));
        addToken(s, makeRawToken(2, 2, "if",      "SCONJ", 4, "mark"));
        addToken(s, makeRawToken(3, 3, "Google",  "PROPN", 4, "nsubj"));
        addToken(s, makeRawToken(4, 4, "Morphed", "VERB",  0, "root"));
        addToken(s, makeRawToken(5, 5, "into",    "ADP",   4, "obl"));
        addToken(s, makeRawToken(6, 6, "GoogleOS","PROPN", 4, "flat"));
        addToken(s, makeRawToken(7, 7, "?",       "PUNCT", 4, "punct"));
        QTest::addRow("1.2_regular") << s << 4 << 7;
    }

    // Тест 1.9
    {
        RawSentence s = makeRawSentence(1, "test", "The old man");
        addToken(s, makeRawToken(1, 1, "The", "DET",  2, "det"));
        addToken(s, makeRawToken(2, 2, "old", "ADJ",  3, "amod"));
        addToken(s, makeRawToken(3, 3, "man", "NOUN", 0, "root"));
        QTest::addRow("1.9_three_levels") << s << 3 << 3;
    }
}

void TEST_BuildSentenceModel::TestStructure()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, expectedRootId);
    QFETCH(int, expectedTokenCount);

    SentenceModel model = buildSentenceModel(rawSentence);
    QCOMPARE(model.tokens.size(), expectedTokenCount);
    QVERIFY(model.rootToken != nullptr);
    QCOMPARE(model.rootToken->id, expectedRootId);
}

// ========================================================================
// 2. Линейная навигация
// ========================================================================

void TEST_BuildSentenceModel::TestNextNonPunct_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("startId");
    QTest::addColumn<int>("expectedNextId");
    QTest::addColumn<bool>("useNonPunct");

    // 1.3
    {
        RawSentence s = makeRawSentence(1, "test", "Word . Word");
        addToken(s, makeRawToken(1, 1, "Word", "NOUN",  0, "root"));
        addToken(s, makeRawToken(2, 2, ".",    "PUNCT", 1, "punct"));
        addToken(s, makeRawToken(3, 3, "Word", "NOUN",  1, "conj"));
        QTest::addRow("1.3_nonpunct") << s << 1 << 3 << true;
        QTest::addRow("1.3_raw")     << s << 1 << 2 << false;
    }

    // 1.4
    {
        RawSentence s = makeRawSentence(1, "test", "Word . ! Word");
        addToken(s, makeRawToken(1, 1, "Word", "NOUN",  0, "root"));
        addToken(s, makeRawToken(2, 2, ".",    "PUNCT", 1, "punct"));
        addToken(s, makeRawToken(3, 3, "!",    "PUNCT", 1, "punct"));
        addToken(s, makeRawToken(4, 4, "Word", "NOUN",  1, "conj"));
        QTest::addRow("1.4_from1_np") << s << 1 << 4 << true;
        QTest::addRow("1.4_from1_r")  << s << 1 << 2 << false;
        QTest::addRow("1.4_from2_np") << s << 2 << 4 << true;
        QTest::addRow("1.4_from2_r")  << s << 2 << 3 << false;
        QTest::addRow("1.4_from3_np") << s << 3 << 4 << true;
        QTest::addRow("1.4_from3_r")  << s << 3 << 4 << false;
    }

    // 1.5
    {
        RawSentence s = makeRawSentence(1, "test", "I don't . go");
        addMwt(s, 1, 2, 3, "don't");
        addToken(s, makeRawToken(1, 1, "I",    "PRON", 4, "nsubj"));
        addToken(s, makeRawToken(2, 2, "do",   "AUX",  4, "aux"));
        addToken(s, makeRawToken(3, 3, "n't",  "PART", 4, "advmod"));
        addToken(s, makeRawToken(4, 4, ".",    "PUNCT",4, "punct"));
        addToken(s, makeRawToken(5, 5, "go",   "VERB", 0, "root"));
        QTest::addRow("1.5_from1_np") << s << 1 << 5 << true;
        QTest::addRow("1.5_from2_np") << s << 2 << 5 << true;
        QTest::addRow("1.5_from3_np") << s << 3 << 5 << true;
        QTest::addRow("1.5_from4_np") << s << 4 << 5 << true;
        QTest::addRow("1.5_from1_r")  << s << 1 << 2 << false;
        QTest::addRow("1.5_from2_r")  << s << 2 << 3 << false;
        QTest::addRow("1.5_from3_r")  << s << 3 << 4 << false;
        QTest::addRow("1.5_from4_r")  << s << 4 << 5 << false;
    }
}

void TEST_BuildSentenceModel::TestNextNonPunct()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, startId);
    QFETCH(int, expectedNextId);
    QFETCH(bool, useNonPunct);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* start = model.tokensById.value(startId, nullptr);
    QVERIFY(start != nullptr);

    const TokenNode* result = useNonPunct ? start->nextNonPunct() : start->nextToken;
    if (expectedNextId == 0) {
        QVERIFY(result == nullptr);
    } else {
        QVERIFY(result != nullptr);
        QCOMPARE(result->id, expectedNextId);
    }
}

// ========================================================================
// 3. FEATS — Number
// ========================================================================

void TEST_BuildSentenceModel::TestFeatsNumber_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("checkId");
    QTest::addColumn<QString>("expectedNumber");

    // 1.6
    {
        RawSentence s = makeRawSentence(1, "test", "cat");
        addToken(s, makeRawToken(1, 1, "cat", "NOUN", 0, "root",
                                QStringLiteral("Number=Sing|Person=3")));
        QTest::addRow("1.6_sing") << s << 1 << "Sing";
    }

    // 1.7
    {
        RawSentence s = makeRawSentence(1, "test", "cat");
        addToken(s, makeRawToken(1, 1, "cat", "NOUN", 0, "root", QStringLiteral("_")));
        QTest::addRow("1.7_empty") << s << 1 << "";
    }

    // 1.12
    {
        RawSentence s = makeRawSentence(1, "test", "cat cats");
        addToken(s, makeRawToken(1, 1, "cat",  "NOUN", 0, "root", QStringLiteral("Number=Sing")));
        addToken(s, makeRawToken(2, 2, "cats", "NOUN", 1, "conj", QStringLiteral("Number=Plur")));
        QTest::addRow("1.12_sing") << s << 1 << "Sing";
        QTest::addRow("1.12_plur") << s << 2 << "Plur";
    }
}

void TEST_BuildSentenceModel::TestFeatsNumber()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, checkId);
    QFETCH(QString, expectedNumber);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* node = model.tokensById.value(checkId, nullptr);
    QVERIFY(node != nullptr);

    QString actual;
    if (node->features.number.has_value()) {
        actual = (node->features.number.value() == NumberValue::Sing) ? "Sing" : "Plur";
    }
    QCOMPARE(actual, expectedNumber);
}

// ========================================================================
// 4. FEATS — VerbForm
// ========================================================================

void TEST_BuildSentenceModel::TestFeatsVerbForm_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("checkId");
    QTest::addColumn<QString>("expectedVerbForm");

    // 1.14
    {
        RawSentence s = makeRawSentence(1, "test", "go gone");
        addToken(s, makeRawToken(1, 1, "go",   "VERB", 0, "root", QStringLiteral("VerbForm=Inf")));
        addToken(s, makeRawToken(2, 2, "gone", "VERB", 1, "conj", QStringLiteral("VerbForm=Part")));
        QTest::addRow("1.14_inf")  << s << 1 << "Inf";
        QTest::addRow("1.14_part") << s << 2 << "Part";
    }
}

void TEST_BuildSentenceModel::TestFeatsVerbForm()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, checkId);
    QFETCH(QString, expectedVerbForm);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* node = model.tokensById.value(checkId, nullptr);
    QVERIFY(node != nullptr);

    QString actual;
    if (node->features.verbForm.has_value()) {
        switch (node->features.verbForm.value()) {
        case VerbFormValue::Inf:  actual = "Inf";  break;
        case VerbFormValue::Part: actual = "Part"; break;
        }
    }
    QCOMPARE(actual, expectedVerbForm);
}

// ========================================================================
// 5. FEATS — Degree
// ========================================================================

void TEST_BuildSentenceModel::TestFeatsDegree_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("checkId");
    QTest::addColumn<QString>("expectedDegree");

    // 1.15
    {
        RawSentence s = makeRawSentence(1, "test", "big bigger biggest");
        addToken(s, makeRawToken(1, 1, "big",     "ADJ", 0, "root", QStringLiteral("Degree=Pos")));
        addToken(s, makeRawToken(2, 2, "bigger",  "ADJ", 1, "conj", QStringLiteral("Degree=Cmp")));
        addToken(s, makeRawToken(3, 3, "biggest", "ADJ", 1, "conj", QStringLiteral("Degree=Sup")));
        QTest::addRow("1.15_pos") << s << 1 << "Pos";
        QTest::addRow("1.15_cmp") << s << 2 << "Cmp";
        QTest::addRow("1.15_sup") << s << 3 << "Sup";
    }
}

void TEST_BuildSentenceModel::TestFeatsDegree()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, checkId);
    QFETCH(QString, expectedDegree);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* node = model.tokensById.value(checkId, nullptr);
    QVERIFY(node != nullptr);

    QString actual;
    if (node->features.degree.has_value()) {
        switch (node->features.degree.value()) {
        case DegreeValue::Pos: actual = "Pos"; break;
        case DegreeValue::Cmp: actual = "Cmp"; break;
        case DegreeValue::Sup: actual = "Sup"; break;
        }
    }
    QCOMPARE(actual, expectedDegree);
}

// ========================================================================
// 6. FEATS — Case
// ========================================================================

void TEST_BuildSentenceModel::TestFeatsCase_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("checkId");
    QTest::addColumn<QString>("expectedCase");

    // 1.16
    {
        RawSentence s = makeRawSentence(1, "test", "he saw him");
        addToken(s, makeRawToken(1, 1, "he",  "PRON", 2, "nsubj", QStringLiteral("Case=Nom")));
        addToken(s, makeRawToken(2, 2, "saw", "VERB", 0, "root"));
        addToken(s, makeRawToken(3, 3, "him", "PRON", 2, "obj",   QStringLiteral("Case=Acc")));
        QTest::addRow("1.16_nom") << s << 1 << "Nom";
        QTest::addRow("1.16_acc") << s << 3 << "Acc";
    }
}

void TEST_BuildSentenceModel::TestFeatsCase()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, checkId);
    QFETCH(QString, expectedCase);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* node = model.tokensById.value(checkId, nullptr);
    QVERIFY(node != nullptr);

    QString actual;
    if (node->features.caseValue.has_value()) {
        actual = (node->features.caseValue.value() == CaseValue::Nom) ? "Nom" : "Acc";
    }
    QCOMPARE(actual, expectedCase);
}

// ========================================================================
// 7. FEATS — Булевы флаги
// ========================================================================

void TEST_BuildSentenceModel::TestFeatsBool_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("checkId");
    QTest::addColumn<bool>("expectedPoss");
    QTest::addColumn<bool>("expectedPolarityNeg");
    QTest::addColumn<bool>("expectedNumTypeOrd");
    QTest::addColumn<bool>("expectedVoicePass");

    // 1.17
    {
        RawSentence s = makeRawSentence(1, "test", "his not first seen");
        addToken(s, makeRawToken(1, 1, "his",   "PRON", 3, "det",    QStringLiteral("Poss=Yes")));
        addToken(s, makeRawToken(2, 2, "not",   "PART", 3, "advmod", QStringLiteral("Polarity=Neg")));
        addToken(s, makeRawToken(3, 3, "first", "ADJ",  0, "root",   QStringLiteral("NumType=Ord")));
        addToken(s, makeRawToken(4, 4, "seen",  "VERB", 3, "conj",   QStringLiteral("Voice=Pass")));
        QTest::addRow("1.17_poss")       << s << 1 << true  << false << false << false;
        QTest::addRow("1.17_polarity")   << s << 2 << false << true  << false << false;
        QTest::addRow("1.17_numtype")   << s << 3 << false << false << true  << false;
        QTest::addRow("1.17_voice")      << s << 4 << false << false << false << true;
    }

    // 1.18
    {
        RawSentence s = makeRawSentence(1, "test", "was");
        addToken(s, makeRawToken(1, 1, "was", "AUX", 0, "root",
                                QStringLiteral("Tense=Past|Voice=Pass")));
        QTest::addRow("1.18_voice") << s << 1 << false << false << false << true;
    }
}

void TEST_BuildSentenceModel::TestFeatsBool()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, checkId);
    QFETCH(bool, expectedPoss);
    QFETCH(bool, expectedPolarityNeg);
    QFETCH(bool, expectedNumTypeOrd);
    QFETCH(bool, expectedVoicePass);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* node = model.tokensById.value(checkId, nullptr);
    QVERIFY(node != nullptr);

    QCOMPARE(node->features.poss, expectedPoss);
    QCOMPARE(node->features.polarityNeg, expectedPolarityNeg);
    QCOMPARE(node->features.numTypeOrd, expectedNumTypeOrd);
    QCOMPARE(node->features.voicePass, expectedVoicePass);
}

// ========================================================================
// 8. UPOS enum
// ========================================================================

void TEST_BuildSentenceModel::TestUposEnum_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("checkId");
    QTest::addColumn<Upos>("expectedUpos");

    // 1.10
    {
        RawSentence s = makeRawSentence(1, "test",
            "it is done well by the 3 old boy John and to $ if foo !");
        addToken(s, makeRawToken(1,  1,  "it",    "PRON",  3, "nsubj"));
        addToken(s, makeRawToken(2,  2,  "is",    "AUX",   3, "aux"));
        addToken(s, makeRawToken(3,  3,  "done",  "VERB",  0, "root"));
        addToken(s, makeRawToken(4,  4,  "well",  "ADV",   3, "advmod"));
        addToken(s, makeRawToken(5,  5,  "by",    "ADP",   3, "obl"));
        addToken(s, makeRawToken(6,  6,  "the",   "DET",   9, "det"));
        addToken(s, makeRawToken(7,  7,  "3",     "NUM",   9, "nummod"));
        addToken(s, makeRawToken(8,  8,  "old",   "ADJ",   9, "amod"));
        addToken(s, makeRawToken(9,  9,  "boy",   "NOUN",  5, "obj"));
        addToken(s, makeRawToken(10, 10, "John",  "PROPN", 9, "appos"));
        addToken(s, makeRawToken(11, 11, "and",   "CCONJ", 9, "cc"));
        addToken(s, makeRawToken(12, 12, "to",    "PART",  3, "mark"));
        addToken(s, makeRawToken(13, 13, "$",     "SYM",   3, "obj"));
        addToken(s, makeRawToken(14, 14, "if",    "SCONJ", 3, "mark"));
        addToken(s, makeRawToken(15, 15, "foo",   "X",     3, "obj"));
        addToken(s, makeRawToken(16, 16, "!",     "PUNCT", 3, "punct"));

        QTest::addRow("1.10_pron")  << s << 1  << Upos::PRON;
        QTest::addRow("1.10_aux")   << s << 2  << Upos::AUX;
        QTest::addRow("1.10_verb")  << s << 3  << Upos::VERB;
        QTest::addRow("1.10_adv")   << s << 4  << Upos::ADV;
        QTest::addRow("1.10_adp")   << s << 5  << Upos::ADP;
        QTest::addRow("1.10_det")   << s << 6  << Upos::DET;
        QTest::addRow("1.10_num")   << s << 7  << Upos::NUM;
        QTest::addRow("1.10_adj")   << s << 8  << Upos::ADJ;
        QTest::addRow("1.10_noun")  << s << 9  << Upos::NOUN;
        QTest::addRow("1.10_propn") << s << 10 << Upos::PROPN;
        QTest::addRow("1.10_cconj") << s << 11 << Upos::CCONJ;
        QTest::addRow("1.10_part")  << s << 12 << Upos::PART;
        QTest::addRow("1.10_sym")   << s << 13 << Upos::SYM;
        QTest::addRow("1.10_sconj") << s << 14 << Upos::SCONJ;
        QTest::addRow("1.10_x")     << s << 15 << Upos::X;
        QTest::addRow("1.10_punct") << s << 16 << Upos::PUNCT;
    }
}

void TEST_BuildSentenceModel::TestUposEnum()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, checkId);
    QFETCH(Upos, expectedUpos);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* node = model.tokensById.value(checkId, nullptr);
    QVERIFY(node != nullptr);
    QCOMPARE(node->upos, expectedUpos);
}

// ========================================================================
// 9. DEPREL enum
// ========================================================================

void TEST_BuildSentenceModel::TestDeprelEnum_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("checkId");
    QTest::addColumn<Deprel>("expectedDeprel");

    // 1.11
    {
        RawSentence s = makeRawSentence(1, "test",
            "the old man was seen by the boy and his friend Mr Smith Jr well "
            "there is a job in town where he works to go President neither nor "
            "John it be ready !");

        addToken(s, makeRawToken(1,  1,  "the",       "DET",   3, "det"));
        addToken(s, makeRawToken(2,  2,  "old",       "ADJ",   3, "amod"));
        addToken(s, makeRawToken(3,  3,  "man",       "NOUN",  4, "nsubj:pass"));
        addToken(s, makeRawToken(4,  4,  "seen",      "VERB",  0, "root"));
        addToken(s, makeRawToken(5,  5,  "was",       "AUX",   4, "aux:pass"));
        addToken(s, makeRawToken(6,  6,  "by",        "ADP",   8, "case"));
        addToken(s, makeRawToken(7,  7,  "the",       "DET",   8, "det"));
        addToken(s, makeRawToken(8,  8,  "boy",       "NOUN",  4, "obl"));
        addToken(s, makeRawToken(9,  9,  "and",       "CCONJ", 11, "cc"));
        addToken(s, makeRawToken(10, 10, "his",       "PRON",  11, "nmod:poss"));
        addToken(s, makeRawToken(11, 11, "friend",    "NOUN",  8, "conj"));
        addToken(s, makeRawToken(12, 12, "Mr",        "PROPN", 13, "nmod:desc"));
        addToken(s, makeRawToken(13, 13, "Smith",     "PROPN", 11, "flat:name"));
        addToken(s, makeRawToken(14, 14, "Jr",        "PROPN", 13, "flat"));
        addToken(s, makeRawToken(15, 15, "well",      "ADV",   4, "advmod"));
        addToken(s, makeRawToken(16, 16, "there",     "PRON",  17, "expl"));
        addToken(s, makeRawToken(17, 17, "is",        "AUX",   19, "aux"));
        addToken(s, makeRawToken(18, 18, "a",         "DET",   19, "det"));
        addToken(s, makeRawToken(19, 19, "job",       "NOUN",  4, "obl"));
        addToken(s, makeRawToken(20, 20, "in",        "ADP",   21, "case"));
        addToken(s, makeRawToken(21, 21, "town",      "NOUN",  19, "nmod"));
        addToken(s, makeRawToken(22, 22, "where",     "SCONJ", 24, "mark"));
        addToken(s, makeRawToken(23, 23, "he",        "PRON",  24, "nsubj"));
        addToken(s, makeRawToken(24, 24, "works",     "VERB",  19, "advcl"));
        addToken(s, makeRawToken(25, 25, "to",        "ADP",   26, "fixed"));
        addToken(s, makeRawToken(26, 26, "go",        "VERB",  24, "xcomp"));
        addToken(s, makeRawToken(27, 27, "President", "PROPN", 4, "appos"));
        addToken(s, makeRawToken(28, 28, "neither",   "CCONJ", 4, "cc:preconj"));
        addToken(s, makeRawToken(29, 29, "nor",       "CCONJ", 4, "cc"));
        addToken(s, makeRawToken(30, 30, "John",      "PROPN", 4, "vocative"));
        addToken(s, makeRawToken(31, 31, "it",        "PRON",  4, "obj"));
        addToken(s, makeRawToken(32, 32, "be",        "AUX",   33, "cop"));
        addToken(s, makeRawToken(33, 33, "ready",     "ADJ",   4, "conj"));
        addToken(s, makeRawToken(34, 34, "!",         "PUNCT", 4, "punct"));

        QTest::addRow("1.11_det")        << s << 1  << Deprel::Det;
        QTest::addRow("1.11_amod")       << s << 2  << Deprel::Amod;
        QTest::addRow("1.11_nsubjpass")  << s << 3  << Deprel::NsubjPass;
        QTest::addRow("1.11_root")       << s << 4  << Deprel::Root;
        QTest::addRow("1.11_auxpass")    << s << 5  << Deprel::AuxPass;
        QTest::addRow("1.11_case")       << s << 6  << Deprel::Case;
        QTest::addRow("1.11_det2")       << s << 7  << Deprel::Det;
        QTest::addRow("1.11_obl")        << s << 8  << Deprel::Obl;
        QTest::addRow("1.11_cc")         << s << 9  << Deprel::Cc;
        QTest::addRow("1.11_nmodposs")   << s << 10 << Deprel::NmodPoss;
        QTest::addRow("1.11_conj")       << s << 11 << Deprel::Conj;
        QTest::addRow("1.11_nmoddesc")   << s << 12 << Deprel::NmodDesc;
        QTest::addRow("1.11_flatname")   << s << 13 << Deprel::FlatName;
        QTest::addRow("1.11_flat")       << s << 14 << Deprel::Flat;
        QTest::addRow("1.11_advmod")     << s << 15 << Deprel::Advmod;
        QTest::addRow("1.11_expl")       << s << 16 << Deprel::Expl;
        QTest::addRow("1.11_aux")        << s << 17 << Deprel::Aux;
        QTest::addRow("1.11_det3")       << s << 18 << Deprel::Det;
        QTest::addRow("1.11_obl2")       << s << 19 << Deprel::Obl;
        QTest::addRow("1.11_case2")      << s << 20 << Deprel::Case;
        QTest::addRow("1.11_nmod")       << s << 21 << Deprel::Nmod;
        QTest::addRow("1.11_mark")       << s << 22 << Deprel::Mark;
        QTest::addRow("1.11_nsubj")      << s << 23 << Deprel::Nsubj;
        QTest::addRow("1.11_advcl")      << s << 24 << Deprel::Advcl;
        QTest::addRow("1.11_fixed")      << s << 25 << Deprel::Fixed;
        QTest::addRow("1.11_xcomp")      << s << 26 << Deprel::Xcomp;
        QTest::addRow("1.11_appos")      << s << 27 << Deprel::Appos;
        QTest::addRow("1.11_ccpreconj")  << s << 28 << Deprel::CcPreconj;
        QTest::addRow("1.11_cc2")        << s << 29 << Deprel::Cc;
        QTest::addRow("1.11_vocative")   << s << 30 << Deprel::Vocative;
        QTest::addRow("1.11_obj")        << s << 31 << Deprel::Obj;
        QTest::addRow("1.11_cop")        << s << 32 << Deprel::Cop;
        QTest::addRow("1.11_conj2")      << s << 33 << Deprel::Conj;
        QTest::addRow("1.11_punct")      << s << 34 << Deprel::Punct;
    }
}

void TEST_BuildSentenceModel::TestDeprelEnum()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, checkId);
    QFETCH(Deprel, expectedDeprel);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* node = model.tokensById.value(checkId, nullptr);
    QVERIFY(node != nullptr);
    QCOMPARE(node->deprel, expectedDeprel);
}

// ========================================================================
// 10. MWT флаг
// ========================================================================

void TEST_BuildSentenceModel::TestMwtFlag_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<int>("checkId");
    QTest::addColumn<bool>("expectedMwtFragment");

    // 1.8
    {
        RawSentence s = makeRawSentence(1, "test", "I don't know");
        addMwt(s, 1, 2, 3, "don't");
        addToken(s, makeRawToken(1, 1, "I",     "PRON", 4, "nsubj"));
        addToken(s, makeRawToken(2, 2, "do",    "AUX",  4, "aux"));
        addToken(s, makeRawToken(3, 3, "n't",   "PART", 4, "advmod"));
        addToken(s, makeRawToken(4, 4, "know",  "VERB", 0, "root"));
        QTest::addRow("1.8_not_mwt")  << s << 1 << false;
        QTest::addRow("1.8_mwt_2")    << s << 2 << true;
        QTest::addRow("1.8_mwt_3")    << s << 3 << true;
        QTest::addRow("1.8_not_mwt4") << s << 4 << false;
    }
}

void TEST_BuildSentenceModel::TestMwtFlag()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(int, checkId);
    QFETCH(bool, expectedMwtFragment);

    SentenceModel model = buildSentenceModel(rawSentence);
    TokenNode* node = model.tokensById.value(checkId, nullptr);
    QVERIFY(node != nullptr);
    QCOMPARE(node->isMwtFragment, expectedMwtFragment);
}


