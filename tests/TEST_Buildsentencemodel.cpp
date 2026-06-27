/*!
* \file TEST_Buildsentencemodel.cpp
* \brief DDT-тесты для функции buildSentenceModel.
*
* Проверяет корректность построения SentenceModel из одного RawSentence:
*  - структура дерева (корень, количество токенов, дочерние узлы);
*  - линейная навигация (nextToken, nextNonPunct, previousToken);
*  - типизированные UPOS и DEPREL (enum);
*  - парсинг FEATS (Number, Tense, VerbForm, Degree, Case, булевы флаги);
*  - флаг MWT (isMwtFragment).
*/

#include "TEST_Buildsentencemodel.h"
#include "auxiliaryfunctionsfortesting.h"
#include "modelbuilder.h"
#include <QTest>

// ------------------------------------------------------------------------
// Структура ожиданий
// ------------------------------------------------------------------------

/*!
* \struct BuildExpect
* \brief Точечные ожидания для теста buildSentenceModel.
*
* Все поля имеют значения по умолчанию, означающие «не проверять».
* В _data() заполняются только те поля, что указаны в спецификации
* для конкретного теста. Универсальная функция проверки обходит
* только заполненные поля.
*/
struct BuildExpect {
    // Структура дерева
    int expectedTokenCount = -1;       ///< Ожидаемое число токенов. -1: не проверять.
    int expectedRootId = -1;           ///< Ожидаемый ID корневого токена. -1: не проверять.

    // Дочерние узлы (одиночная проверка)
    int checkChildrenId = -1;          ///< ID токена для проверки children. -1: не проверять.
    QList<int> expectedChildren;       ///< Ожидаемые ID дочерних токенов. Пусто: не проверять.

    // Дочерние узлы (множественная проверка: список токенов, у каждого ровно один child)
    QList<int> checkChildrenIds;       ///< ID токенов для проверки children. Пусто: не проверять.
    QList<int> expectedSingleChildIds; ///< Ожидаемый единственный child ID для каждого токена.

    // Родительский узел (одиночная проверка)
    int checkParentId = -1;            ///< ID токена для проверки parent. -1: не проверять.
    int expectedParentId = -1;         ///< Ожидаемый ID родителя. -1: не проверять.

    // Родительский узел (множественная проверка)
    QList<int> checkParentIds;         ///< ID токенов для проверки parent. Пусто: не проверять.
    QList<int> expectedParentIds;      ///< Ожидаемые ID родителя для каждого токена.

    // Линейная навигация: nextToken
    int checkNextId = -1;              ///< ID токена для проверки nextToken. -1: не проверять.
    int expectedNextId = -1;           ///< Ожидаемый ID nextToken. 0: nullptr. -1: не проверять.

    // Линейная навигация: previousToken
    int checkPrevId = -1;              ///< ID токена для проверки previousToken. -1: не проверять.
    int expectedPrevId = -1;           ///< Ожидаемый ID previousToken. 0: nullptr. -1: не проверять.

    // Линейная навигация: nextNonPunct
    int checkNextNonPunctId = -1;      ///< ID токена для проверки nextNonPunct. -1: не проверять.
    int expectedNextNonPunctId = -1;   ///< Ожидаемый ID nextNonPunct. 0: nullptr. -1: не проверять.

    // Цепочка линейных связей
    QList<int> expectedLinearChain;    ///< Ожидаемая цепочка nextToken ID. Пусто: не проверять.

    // UPOS
    int checkUposId = -1;               ///< ID токена для проверки upos. -1: не проверять.
    Upos expectedUpos = Upos::X;       ///< Ожидаемый UPOS. Upos::X: не проверять (флаг checkUposId).
    bool hasUposCheck = false;         ///< Флаг: проверять upos.

    // DEPREL
    int checkDeprelId = -1;            ///< ID токена для проверки deprel. -1: не проверять.
    Deprel expectedDeprel = Deprel::Other; ///< Ожидаемый DEPREL.
    bool hasDeprelCheck = false;       ///< Флаг: проверять deprel.

    // FEATS: Number
    int checkNumberId = -1;            ///< ID токена для проверки number. -1: не проверять.
    QString expectedNumber;            ///< Ожидаемое Number. "_" — пропустить, "" — пусто.

    // FEATS: Tense
    int checkTenseId = -1;             ///< ID токена для проверки tense. -1: не проверять.
    QString expectedTense;             ///< Ожидаемое Tense. "_" — пропустить, "" — пусто.

    // FEATS: VerbForm
    int checkVerbFormId = -1;          ///< ID токена для проверки verbForm. -1: не проверять.
    QString expectedVerbForm;          ///< Ожидаемое VerbForm. "_" — пропустить, "" — пусто.

    // FEATS: Degree
    int checkDegreeId = -1;            ///< ID токена для проверки degree. -1: не проверять.
    QString expectedDegree;            ///< Ожидаемое Degree. "_" — пропустить, "" — пусто.

    // FEATS: Case
    int checkCaseId = -1;              ///< ID токена для проверки caseValue. -1: не проверять.
    QString expectedCase;              ///< Ожидаемый Case. "_" — пропустить, "" — пусто.

    // FEATS: Person
    int checkPersonId = -1;            ///< ID токена для проверки person. -1: не проверять.
    QString expectedPerson;            ///< Ожидаемый Person. "_" — пропустить, "" — пусто.

    // FEATS: булевы флаги
    int checkBoolId = -1;              ///< ID токена для проверки булевых флагов. -1: не проверять.
    bool expectedPoss = false;         ///< Ожидаемое Poss=Yes.
    bool expectedPolarityNeg = false;  ///< Ожидаемое Polarity=Neg.
    bool expectedNumTypeOrd = false;    ///< Ожидаемое NumType=Ord.
    bool expectedVoicePass = false;    ///< Ожидаемое Voice=Pass.
    bool hasBoolCheck = false;         ///< Флаг: проверять булевы флаги.

    // MWT
    int checkMwtId = -1;               ///< ID токена для проверки isMwtFragment. -1: не проверять.
    bool expectedMwtFragment = false;   ///< Ожидаемый флаг MWT.
    int expectedMwtRecordsCount = -1;  ///< Ожидаемое число MWT-записей. -1: не проверять.

    // MWT: множественная проверка (список токенов и ожидаемых флагов)
    QList<int> checkMwtIds;            ///< ID токенов для проверки isMwtFragment. Пусто: не проверять.
    QList<bool> expectedMwtFragments;   ///< Ожидаемые флаги MWT для каждого токена.
};

Q_DECLARE_METATYPE(BuildExpect)

// ------------------------------------------------------------------------
// Конструктор / деструктор
// ------------------------------------------------------------------------

TEST_BuildSentenceModel::TEST_BuildSentenceModel() {}
TEST_BuildSentenceModel::~TEST_BuildSentenceModel() {}

// ------------------------------------------------------------------------
// Данные тестов (1.1-1.18)
// ------------------------------------------------------------------------

void TEST_BuildSentenceModel::TestBuildSentenceModel_data()
{
    QTest::addColumn<RawSentence>("rawSentence");
    QTest::addColumn<BuildExpect>("expect");

    // === 1.1 Пустое предложение (1 токен) ================================
    {
        RawSentence s = makeRawSentence(1, "test", "Word");
        addToken(s, makeRawToken(1, 1, "Word", "NOUN", 0, "root"));
        BuildExpect e;
        e.expectedTokenCount = 1;
        e.expectedRootId = 1;
        e.checkChildrenId = 1;
        e.expectedChildren = {};
        e.checkPrevId = 1;
        e.expectedPrevId = 0;
        e.checkNextId = 1;
        e.expectedNextId = 0;
        e.expectedMwtRecordsCount = 0;
        QTest::addRow("1.1_one_token") << s << e;
    }

    // === 1.2 Обычное предложение (7 токенов) ============================
    {
        RawSentence s = makeRawSentence(1, "test", "What if Google Morphed Into GoogleOS?");
        addToken(s, makeRawToken(1, 1, "What",     "PRON",  4, "mark"));
        addToken(s, makeRawToken(2, 2, "if",       "SCONJ", 4, "mark"));
        addToken(s, makeRawToken(3, 3, "Google",   "PROPN", 4, "nsubj"));
        addToken(s, makeRawToken(4, 4, "Morphed",  "VERB",  0, "root"));
        addToken(s, makeRawToken(5, 5, "Into",     "ADP",   4, "obl"));
        addToken(s, makeRawToken(6, 6, "GoogleOS", "PROPN", 4, "flat"));
        addToken(s, makeRawToken(7, 7, "?",        "PUNCT", 4, "punct"));
        BuildExpect e;
        e.expectedRootId = 4;
        e.checkUposId = 3;
        e.expectedUpos = Upos::Prop;
        e.hasUposCheck = true;
        e.expectedLinearChain = {1, 2, 3, 4, 5, 6, 7};
        QTest::addRow("1.2_regular") << s << e;
    }

    // === 1.3 nextNonPunct (1 punct) ====================================
    {
        RawSentence s = makeRawSentence(1, "test", "Word . Word");
        addToken(s, makeRawToken(1, 1, "Word", "NOUN",  0, "root"));
        addToken(s, makeRawToken(2, 2, ".",    "PUNCT", 1, "punct"));
        addToken(s, makeRawToken(3, 3, "Word", "NOUN",  1, "conj"));
        BuildExpect e;
        e.checkNextNonPunctId = 1;
        e.expectedNextNonPunctId = 3;
        e.checkNextId = 1;
        e.expectedNextId = 2;
        QTest::addRow("1.3_next_nonpunct") << s << e;
    }

    // === 1.4 nextNonPunct (несколько punct подряд) =====================
    {
        RawSentence s = makeRawSentence(1, "test", "Word . ! Word");
        addToken(s, makeRawToken(1, 1, "Word", "NOUN",  0, "root"));
        addToken(s, makeRawToken(2, 2, ".",    "PUNCT", 1, "punct"));
        addToken(s, makeRawToken(3, 3, "!",    "PUNCT", 1, "punct"));
        addToken(s, makeRawToken(4, 4, "Word", "NOUN",  1, "conj"));
        BuildExpect e;
        e.checkNextNonPunctId = 1;
        e.expectedNextNonPunctId = 4;
        QTest::addRow("1.4_next_nonpunct_multi") << s << e;
    }

    // === 1.5 nextNonPunct (MWT + punct) ================================
    {
        RawSentence s = makeRawSentence(1, "test", "I don't . go");
        addMwt(s, 1, 2, 3, "don't");
        addToken(s, makeRawToken(1, 1, "I",    "PRON", 4, "nsubj"));
        addToken(s, makeRawToken(2, 2, "do",   "AUX",  4, "aux"));
        addToken(s, makeRawToken(3, 3, "n't",  "PART", 4, "advmod"));
        addToken(s, makeRawToken(4, 4, ".",    "PUNCT",4, "punct"));
        addToken(s, makeRawToken(5, 5, "go",   "VERB", 0, "root"));
        BuildExpect e;
        e.checkNextNonPunctId = 1;
        e.expectedNextNonPunctId = 5;
        QTest::addRow("1.5_next_nonpunct_mwt") << s << e;
    }

    // === 1.6 FEATS корректный парсинг ==================================
    {
        RawSentence s = makeRawSentence(1, "test", "cat");
        addToken(s, makeRawToken(1, 1, "cat", "NOUN", 0, "root",
                                 QStringLiteral("Number=Sing|Person=3")));
        BuildExpect e;
        e.checkNumberId = 1;
        e.expectedNumber = "Sing";
        e.checkPersonId = 1;
        e.expectedPerson = "3";
        e.checkBoolId = 1;
        e.expectedPoss = false;
        e.hasBoolCheck = true;
        QTest::addRow("1.6_feats_parse") << s << e;
    }

    // === 1.7 FEATS пустой ==============================================
    {
        RawSentence s = makeRawSentence(1, "test", "cat");
        addToken(s, makeRawToken(1, 1, "cat", "NOUN", 0, "root", QStringLiteral("_")));
        BuildExpect e;
        e.checkNumberId = 1;
        e.expectedNumber = "";
        e.checkVerbFormId = 1;
        e.expectedVerbForm = "";
        e.checkBoolId = 1;
        e.expectedPoss = false;
        e.hasBoolCheck = true;
        QTest::addRow("1.7_feats_empty") << s << e;
    }

    // === 1.8 MWT в середине предложения =================================
    {
        RawSentence s = makeRawSentence(1, "test", "I don't know");
        addMwt(s, 1, 2, 3, "don't");
        addToken(s, makeRawToken(1, 1, "I",    "PRON", 4, "nsubj"));
        addToken(s, makeRawToken(2, 2, "do",   "AUX",  4, "aux"));
        addToken(s, makeRawToken(3, 3, "n't",  "PART", 4, "advmod"));
        addToken(s, makeRawToken(4, 4, "know", "VERB", 0, "root"));
        BuildExpect e;
        e.expectedMwtRecordsCount = 1;
        // Токен 1: не MWT, токен 2: MWT, токен 3: MWT, токен 4: не MWT.
        e.checkMwtIds = {1, 2, 3, 4};
        e.expectedMwtFragments = {false, true, true, false};
        QTest::addRow("1.8_mwt_middle") << s << e;
    }

    // === 1.9 Иерархия (3 уровня) =======================================
    {
        RawSentence s = makeRawSentence(1, "test", "The old man");
        addToken(s, makeRawToken(1, 1, "The", "DET", 2, "det"));
        addToken(s, makeRawToken(2, 2, "old", "ADJ", 3, "amod"));
        addToken(s, makeRawToken(3, 3, "man", "NOUN", 0, "root"));
        BuildExpect e;
        // Токен 3: children == {2}, токен 2: children == {1}.
        e.checkChildrenIds = {3, 2};
        e.expectedSingleChildIds = {2, 1};
        // Токен 1: parent == 2, токен 2: parent == 3.
        e.checkParentIds = {1, 2};
        e.expectedParentIds = {2, 3};
        QTest::addRow("1.9_hierarchy") << s << e;
    }

    // === 1.10 UPOS — все значения enum class Upos ======================
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

        // 16 токенов — 16 отдельных addRow, каждый проверяет один UPOS.
        // Все используют одну и ту же RawSentence.
        struct UposCheck { int id; const char* tag; Upos upos; };
        UposCheck checks[] = {
            {1,  "1.10_pron",  Upos::Pron},
            {2,  "1.10_aux",   Upos::Aux},
            {3,  "1.10_verb",  Upos::Verb},
            {4,  "1.10_adv",   Upos::Adv},
            {5,  "1.10_adp",   Upos::Adp},
            {6,  "1.10_det",   Upos::Det},
            {7,  "1.10_num",   Upos::Num},
            {8,  "1.10_adj",   Upos::Adj},
            {9,  "1.10_noun",  Upos::Noun},
            {10, "1.10_propn", Upos::Prop},
            {11, "1.10_cconj", Upos::CConj},
            {12, "1.10_part",  Upos::Part},
            {13, "1.10_sym",   Upos::Sym},
            {14, "1.10_sconj", Upos::SConj},
            {15, "1.10_x",     Upos::X},
            {16, "1.10_punct", Upos::Punct},
        };
        for (const auto& c : checks) {
            BuildExpect e;
            e.checkUposId = c.id;
            e.expectedUpos = c.upos;
            e.hasUposCheck = true;
            QTest::addRow(c.tag) << s << e;
        }
    }

    // === 1.11 DEPREL — все значения enum class Deprel ==================
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

        // 34 токена — 34 отдельных addRow, каждый проверяет один DEPREL.
        struct DeprelCheck { int id; const char* tag; Deprel deprel; };
        DeprelCheck checks[] = {
            {1,  "1.11_det",        Deprel::Det},
            {2,  "1.11_amod",       Deprel::Amod},
            {3,  "1.11_nsubjpass",  Deprel::NsubjPass},
            {4,  "1.11_root",       Deprel::Root},
            {5,  "1.11_auxpass",    Deprel::AuxPass},
            {6,  "1.11_case",       Deprel::Case},
            {7,  "1.11_det2",       Deprel::Det},
            {8,  "1.11_obl",        Deprel::Obl},
            {9,  "1.11_cc",         Deprel::Cc},
            {10, "1.11_nmodposs",   Deprel::NmodPoss},
            {11, "1.11_conj",       Deprel::Conj},
            {12, "1.11_nmoddesc",   Deprel::NmodDesc},
            {13, "1.11_flatname",   Deprel::FlatName},
            {14, "1.11_flat",       Deprel::Flat},
            {15, "1.11_advmod",     Deprel::Advmod},
            {16, "1.11_expl",       Deprel::Expl},
            {17, "1.11_aux",        Deprel::Aux},
            {18, "1.11_det3",       Deprel::Det},
            {19, "1.11_obl2",       Deprel::Obl},
            {20, "1.11_case2",      Deprel::Case},
            {21, "1.11_nmod",       Deprel::Nmod},
            {22, "1.11_mark",       Deprel::Mark},
            {23, "1.11_nsubj",      Deprel::Nsubj},
            {24, "1.11_advcl",      Deprel::Advcl},
            {25, "1.11_fixed",     Deprel::Fixed},
            {26, "1.11_xcomp",      Deprel::Xcomp},
            {27, "1.11_appos",      Deprel::Appos},
            {28, "1.11_ccpreconj", Deprel::CcPreconj},
            {29, "1.11_cc2",        Deprel::Cc},
            {30, "1.11_vocative",   Deprel::Vocative},
            {31, "1.11_obj",        Deprel::Obj},
            {32, "1.11_cop",        Deprel::Cop},
            {33, "1.11_conj2",      Deprel::Conj},
            {34, "1.11_punct",      Deprel::Punct},
        };
        for (const auto& c : checks) {
            BuildExpect e;
            e.checkDeprelId = c.id;
            e.expectedDeprel = c.deprel;
            e.hasDeprelCheck = true;
            QTest::addRow(c.tag) << s << e;
        }
    }

    // === 1.12 FEATS — Number=Sing/Plur ================================
    {
        RawSentence s = makeRawSentence(1, "test", "cat cats");
        addToken(s, makeRawToken(1, 1, "cat",  "NOUN", 0, "root", QStringLiteral("Number=Sing")));
        addToken(s, makeRawToken(2, 2, "cats", "NOUN", 1, "conj", QStringLiteral("Number=Plur")));
        // Токен 1: Sing
        {
            BuildExpect e;
            e.checkNumberId = 1;
            e.expectedNumber = "Sing";
            QTest::addRow("1.12_sing") << s << e;
        }
        // Токен 2: Plur
        {
            BuildExpect e;
            e.checkNumberId = 2;
            e.expectedNumber = "Plur";
            QTest::addRow("1.12_plur") << s << e;
        }
    }

    // === 1.13 FEATS — Tense=Past/Pres =================================
    {
        RawSentence s = makeRawSentence(1, "test", "went goes");
        addToken(s, makeRawToken(1, 1, "went", "VERB", 0, "root", QStringLiteral("Tense=Past")));
        addToken(s, makeRawToken(2, 2, "goes", "VERB", 1, "conj", QStringLiteral("Tense=Pres")));
        {
            BuildExpect e;
            e.checkTenseId = 1;
            e.expectedTense = "Past";
            QTest::addRow("1.13_past") << s << e;
        }
        {
            BuildExpect e;
            e.checkTenseId = 2;
            e.expectedTense = "Pres";
            QTest::addRow("1.13_pres") << s << e;
        }
    }

    // === 1.14 FEATS — VerbForm=Inf/Part ===============================
    {
        RawSentence s = makeRawSentence(1, "test", "go gone");
        addToken(s, makeRawToken(1, 1, "go",   "VERB", 0, "root", QStringLiteral("VerbForm=Inf")));
        addToken(s, makeRawToken(2, 2, "gone", "VERB", 1, "conj", QStringLiteral("VerbForm=Part")));
        {
            BuildExpect e;
            e.checkVerbFormId = 1;
            e.expectedVerbForm = "Inf";
            QTest::addRow("1.14_inf") << s << e;
        }
        {
            BuildExpect e;
            e.checkVerbFormId = 2;
            e.expectedVerbForm = "Part";
            QTest::addRow("1.14_part") << s << e;
        }
    }

    // === 1.15 FEATS — Degree=Pos/Cmp/Sup ==============================
    {
        RawSentence s = makeRawSentence(1, "test", "big bigger biggest");
        addToken(s, makeRawToken(1, 1, "big",     "ADJ", 0, "root", QStringLiteral("Degree=Pos")));
        addToken(s, makeRawToken(2, 2, "bigger",  "ADJ", 1, "conj", QStringLiteral("Degree=Cmp")));
        addToken(s, makeRawToken(3, 3, "biggest", "ADJ", 1, "conj", QStringLiteral("Degree=Sup")));
        {
            BuildExpect e;
            e.checkDegreeId = 1;
            e.expectedDegree = "Pos";
            QTest::addRow("1.15_pos") << s << e;
        }
        {
            BuildExpect e;
            e.checkDegreeId = 2;
            e.expectedDegree = "Cmp";
            QTest::addRow("1.15_cmp") << s << e;
        }
        {
            BuildExpect e;
            e.checkDegreeId = 3;
            e.expectedDegree = "Sup";
            QTest::addRow("1.15_sup") << s << e;
        }
    }

    // === 1.16 FEATS — Case=Nom/Acc ====================================
    {
        RawSentence s = makeRawSentence(1, "test", "he saw him");
        addToken(s, makeRawToken(1, 1, "he",  "PRON", 2, "nsubj", QStringLiteral("Case=Nom")));
        addToken(s, makeRawToken(2, 2, "saw", "VERB", 0, "root"));
        addToken(s, makeRawToken(3, 3, "him", "PRON", 2, "obj",   QStringLiteral("Case=Acc")));
        {
            BuildExpect e;
            e.checkCaseId = 1;
            e.expectedCase = "Nom";
            QTest::addRow("1.16_nom") << s << e;
        }
        {
            BuildExpect e;
            e.checkCaseId = 3;
            e.expectedCase = "Acc";
            QTest::addRow("1.16_acc") << s << e;
        }
    }

    // === 1.17 FEATS — булевы флаги =====================================
    {
        RawSentence s = makeRawSentence(1, "test", "his not first seen");
        addToken(s, makeRawToken(1, 1, "his",   "PRON", 3, "det",    QStringLiteral("Poss=Yes")));
        addToken(s, makeRawToken(2, 2, "not",   "PART", 3, "advmod", QStringLiteral("Polarity=Neg")));
        addToken(s, makeRawToken(3, 3, "first", "ADJ",  0, "root",   QStringLiteral("NumType=Ord")));
        addToken(s, makeRawToken(4, 4, "seen",  "VERB", 3, "conj",   QStringLiteral("Voice=Pass")));
        {
            BuildExpect e;
            e.checkBoolId = 1;
            e.expectedPoss = true;
            e.hasBoolCheck = true;
            QTest::addRow("1.17_poss") << s << e;
        }
        {
            BuildExpect e;
            e.checkBoolId = 2;
            e.expectedPolarityNeg = true;
            e.hasBoolCheck = true;
            QTest::addRow("1.17_polarity") << s << e;
        }
        {
            BuildExpect e;
            e.checkBoolId = 3;
            e.expectedNumTypeOrd = true;
            e.hasBoolCheck = true;
            QTest::addRow("1.17_numtype") << s << e;
        }
        {
            BuildExpect e;
            e.checkBoolId = 4;
            e.expectedVoicePass = true;
            e.hasBoolCheck = true;
            QTest::addRow("1.17_voice") << s << e;
        }
    }

    // === 1.18 FEATS — комбинация признаков ============================
    {
        RawSentence s = makeRawSentence(1, "test", "was");
        addToken(s, makeRawToken(1, 1, "was", "AUX", 0, "root",
                                 QStringLiteral("Tense=Past|Voice=Pass")));
        BuildExpect e;
        e.checkTenseId = 1;
        e.expectedTense = "Past";
        e.checkBoolId = 1;
        e.expectedVoicePass = true;
        e.hasBoolCheck = true;
        QTest::addRow("1.18_combo") << s << e;
    }
}

// ------------------------------------------------------------------------
// Универсальная функция проверки
// ------------------------------------------------------------------------

void TEST_BuildSentenceModel::TestBuildSentenceModel()
{
    QFETCH(RawSentence, rawSentence);
    QFETCH(BuildExpect, expect);

    const QString tag = QString(QTest::currentDataTag());
    SentenceModel model = buildSentenceModel(rawSentence);

    // === 1. Количество токенов =========================================
    if (expect.expectedTokenCount != -1) {
        int actual = static_cast<int>(model.tokens.size());
        if (actual != expect.expectedTokenCount) {
            qDebug() << "[TEST FAIL]" << tag
                     << "Количество токенов: ожидалось =" << expect.expectedTokenCount
                     << "получено =" << actual;
        }
        QCOMPARE(actual, expect.expectedTokenCount);
    }

    // === 2. Корневой токен =============================================
    if (expect.expectedRootId != -1) {
        if (!model.rootToken) {
            qDebug() << "[TEST FAIL]" << tag << "rootToken: ожидался ID =" << expect.expectedRootId
                     << "получено = nullptr";
        }
        QVERIFY(model.rootToken != nullptr);
        if (model.rootToken->id != expect.expectedRootId) {
            qDebug() << "[TEST FAIL]" << tag << "rootToken.id: ожидался =" << expect.expectedRootId
                     << "получено =" << model.rootToken->id;
        }
        QCOMPARE(model.rootToken->id, expect.expectedRootId);
    }

    // === 3. Дочерние узлы ==============================================
    if (expect.checkChildrenId != -1 && !expect.expectedChildren.isEmpty()) {
        TokenNode* node = model.tokensById.value(expect.checkChildrenId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        QList<int> actualChildren;
        for (const TokenNode* child : node->children) {
            actualChildren.append(child->id);
        }
        if (actualChildren != expect.expectedChildren) {
            qDebug() << "[TEST FAIL]" << tag
                     << "Дочерние токены у токена" << expect.checkChildrenId
                     << "ожидалось:" << expect.expectedChildren
                     << "получено:" << actualChildren;
        }
        QCOMPARE(actualChildren, expect.expectedChildren);
    }

    // Проверка пустых children
    if (expect.checkChildrenId != -1 && expect.expectedChildren.isEmpty()) {
        TokenNode* node = model.tokensById.value(expect.checkChildrenId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (!node->children.isEmpty()) {
            qDebug() << "[TEST FAIL]" << tag
                     << "children.empty(): ожидалось = true у токена" << expect.checkChildrenId
                     << "получено = false, размер =" << node->children.size();
        }
        QVERIFY(node->children.isEmpty());
    }

    // Множественная проверка: список токенов, у каждого ровно один child.
    if (!expect.checkChildrenIds.isEmpty()) {
        QVERIFY(expect.checkChildrenIds.size() == expect.expectedSingleChildIds.size());
        for (int i = 0; i < expect.checkChildrenIds.size(); ++i) {
            int tokenId = expect.checkChildrenIds[i];
            int expectedChild = expect.expectedSingleChildIds[i];
            TokenNode* node = model.tokensById.value(tokenId, nullptr);
            QVERIFY2(node != nullptr, qPrintable(tag + ": токен " + QString::number(tokenId) + " не найден"));
            QList<int> actualChildren;
            for (const TokenNode* child : node->children) {
                actualChildren.append(child->id);
            }
            QList<int> expected = {expectedChild};
            if (actualChildren != expected) {
                qDebug() << "[TEST FAIL]" << tag
                         << "Дочерние токены у токена" << tokenId
                         << "ожидалось:" << expected
                         << "получено:" << actualChildren;
            }
            QCOMPARE(actualChildren, expected);
        }
    }

    // === 4. Родительский токен ========================================
    if (expect.checkParentId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkParentId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        int actualParentId = node->parent ? node->parent->id : 0;
        if (actualParentId != expect.expectedParentId) {
            qDebug() << "[TEST FAIL]" << tag
                     << "parent.id у токена" << expect.checkParentId
                     << "ожидался =" << expect.expectedParentId
                     << "получено =" << actualParentId;
        }
        QCOMPARE(actualParentId, expect.expectedParentId);
    }

    // Множественная проверка parent: список токенов с ожидаемыми родителями.
    if (!expect.checkParentIds.isEmpty()) {
        QVERIFY(expect.checkParentIds.size() == expect.expectedParentIds.size());
        for (int i = 0; i < expect.checkParentIds.size(); ++i) {
            int tokenId = expect.checkParentIds[i];
            int expectedParent = expect.expectedParentIds[i];
            TokenNode* node = model.tokensById.value(tokenId, nullptr);
            QVERIFY2(node != nullptr, qPrintable(tag + ": токен " + QString::number(tokenId) + " не найден"));
            int actualParentId = node->parent ? node->parent->id : 0;
            if (actualParentId != expectedParent) {
                qDebug() << "[TEST FAIL]" << tag
                         << "parent.id у токена" << tokenId
                         << "ожидался =" << expectedParent
                         << "получено =" << actualParentId;
            }
            QCOMPARE(actualParentId, expectedParent);
        }
    }

    // === 5. nextToken ==================================================
    if (expect.checkNextId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkNextId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (expect.expectedNextId == 0) {
            if (node->nextToken != nullptr) {
                qDebug() << "[TEST FAIL]" << tag
                         << "nextToken у токена" << expect.checkNextId
                         << "ожидался = nullptr";
            }
            QCOMPARE(node->nextToken, nullptr);
        } else {
            int actualNextId = node->nextToken ? node->nextToken->id : 0;
            if (actualNextId != expect.expectedNextId) {
                qDebug() << "[TEST FAIL]" << tag
                         << "nextToken.id у токена" << expect.checkNextId
                         << "ожидался =" << expect.expectedNextId
                         << "получено =" << actualNextId;
            }
            QCOMPARE(actualNextId, expect.expectedNextId);
        }
    }

    // === 6. previousToken ==============================================
    if (expect.checkPrevId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkPrevId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (expect.expectedPrevId == 0) {
            if (node->previousToken != nullptr) {
                qDebug() << "[TEST FAIL]" << tag
                         << "previousToken у токена" << expect.checkPrevId
                         << "ожидался = nullptr";
            }
            QCOMPARE(node->previousToken, nullptr);
        } else {
            int actualPrevId = node->previousToken ? node->previousToken->id : 0;
            if (actualPrevId != expect.expectedPrevId) {
                qDebug() << "[TEST FAIL]" << tag
                         << "previousToken.id у токена" << expect.checkPrevId
                         << "ожидался =" << expect.expectedPrevId
                         << "получено =" << actualPrevId;
            }
            QCOMPARE(actualPrevId, expect.expectedPrevId);
        }
    }

    // === 7. nextNonPunct ================================================
    if (expect.checkNextNonPunctId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkNextNonPunctId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        const TokenNode* np = node->nextNonPunct();
        if (expect.expectedNextNonPunctId == 0) {
            if (np != nullptr) {
                qDebug() << "[TEST FAIL]" << tag
                         << "nextNonPunct у токена" << expect.checkNextNonPunctId
                         << "ожидался = nullptr";
            }
            QCOMPARE(np, nullptr);
        } else {
            int actualNpId = np ? np->id : 0;
            if (actualNpId != expect.expectedNextNonPunctId) {
                qDebug() << "[TEST FAIL]" << tag
                         << "nextNonPunct.id у токена" << expect.checkNextNonPunctId
                         << "ожидался =" << expect.expectedNextNonPunctId
                         << "получено =" << actualNpId;
            }
            QCOMPARE(actualNpId, expect.expectedNextNonPunctId);
        }
    }

    // === 8. Цепочка линейных связей ====================================
    if (!expect.expectedLinearChain.isEmpty()) {
        for (int i = 0; i < expect.expectedLinearChain.size() - 1; ++i) {
            int fromId = expect.expectedLinearChain[i];
            int toId = expect.expectedLinearChain[i + 1];
            TokenNode* from = model.tokensById.value(fromId, nullptr);
            QVERIFY2(from != nullptr, qPrintable(tag + ": токен " + QString::number(fromId) + " не найден"));
            int actualNextId = from->nextToken ? from->nextToken->id : 0;
            if (actualNextId != toId) {
                qDebug() << "[TEST FAIL]" << tag
                         << "Линейная связь нарушена: токен" << fromId
                         << "→ nextToken.id =" << actualNextId
                         << "ожидался =" << toId;
            }
            QCOMPARE(actualNextId, toId);
        }
    }

    // === 9. UPOS =======================================================
    if (expect.hasUposCheck) {
        TokenNode* node = model.tokensById.value(expect.checkUposId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (node->upos != expect.expectedUpos) {
            qDebug() << "[TEST FAIL]" << tag
                     << "UPOS токена" << expect.checkUposId
                     << "ожидался =" << static_cast<int>(expect.expectedUpos)
                     << "получено =" << static_cast<int>(node->upos)
                     << "form =" << node->form;
        }
        QCOMPARE(node->upos, expect.expectedUpos);
    }

    // === 10. DEPREL ====================================================
    if (expect.hasDeprelCheck) {
        TokenNode* node = model.tokensById.value(expect.checkDeprelId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (node->deprel != expect.expectedDeprel) {
            qDebug() << "[TEST FAIL]" << tag
                     << "DEPREL токена" << expect.checkDeprelId
                     << "ожидался =" << static_cast<int>(expect.expectedDeprel)
                     << "получено =" << static_cast<int>(node->deprel)
                     << "form =" << node->form
                     << "deprelRaw =" << node->deprelRaw;
        }
        QCOMPARE(node->deprel, expect.expectedDeprel);
    }

    // === 11. FEATS: Number ============================================
    if (expect.checkNumberId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkNumberId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (expect.expectedNumber == "_") {
            // Пропустить проверку.
        } else if (expect.expectedNumber.isEmpty()) {
            if (node->features.number.has_value()) {
                qDebug() << "[TEST FAIL]" << tag
                         << "number у токена" << expect.checkNumberId
                         << "ожидался = пусто, получено ="
                         << (node->features.number.value() == NumberValue::Sing ? "Sing" : "Plur");
            }
            QVERIFY(!node->features.number.has_value());
        } else {
            QString actual;
            if (node->features.number.has_value()) {
                actual = (node->features.number.value() == NumberValue::Sing) ? "Sing" : "Plur";
            }
            if (actual != expect.expectedNumber) {
                qDebug() << "[TEST FAIL]" << tag
                         << "number у токена" << expect.checkNumberId
                         << "ожидался =" << expect.expectedNumber
                         << "получено =" << actual;
            }
            QCOMPARE(actual, expect.expectedNumber);
        }
    }

    // === 12. FEATS: Tense ==============================================
    if (expect.checkTenseId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkTenseId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (expect.expectedTense == "_") {
            // Пропустить проверку.
        } else if (expect.expectedTense.isEmpty()) {
            if (node->features.tense.has_value()) {
                qDebug() << "[TEST FAIL]" << tag
                         << "tense у токена" << expect.checkTenseId
                         << "ожидался = пусто, получено ="
                         << (node->features.tense.value() == TenseValue::Past ? "Past" : "Pres");
            }
            QVERIFY(!node->features.tense.has_value());
        } else {
            QString actual;
            if (node->features.tense.has_value()) {
                actual = (node->features.tense.value() == TenseValue::Past) ? "Past" : "Pres";
            }
            if (actual != expect.expectedTense) {
                qDebug() << "[TEST FAIL]" << tag
                         << "tense у токена" << expect.checkTenseId
                         << "ожидался =" << expect.expectedTense
                         << "получено =" << actual;
            }
            QCOMPARE(actual, expect.expectedTense);
        }
    }

    // === 13. FEATS: VerbForm ===========================================
    if (expect.checkVerbFormId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkVerbFormId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (expect.expectedVerbForm == "_") {
            // Пропустить проверку.
        } else if (expect.expectedVerbForm.isEmpty()) {
            if (node->features.verbForm.has_value()) {
                qDebug() << "[TEST FAIL]" << tag
                         << "verbForm у токена" << expect.checkVerbFormId
                         << "ожидался = пусто";
            }
            QVERIFY(!node->features.verbForm.has_value());
        } else {
            QString actual;
            if (node->features.verbForm.has_value()) {
                switch (node->features.verbForm.value()) {
                case VerbFormValue::Inf:  actual = "Inf";  break;
                case VerbFormValue::Part: actual = "Part"; break;
                }
            }
            if (actual != expect.expectedVerbForm) {
                qDebug() << "[TEST FAIL]" << tag
                         << "verbForm у токена" << expect.checkVerbFormId
                         << "ожидался =" << expect.expectedVerbForm
                         << "получено =" << actual;
            }
            QCOMPARE(actual, expect.expectedVerbForm);
        }
    }

    // === 14. FEATS: Degree =============================================
    if (expect.checkDegreeId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkDegreeId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (expect.expectedDegree == "_") {
            // Пропустить проверку.
        } else if (expect.expectedDegree.isEmpty()) {
            if (node->features.degree.has_value()) {
                qDebug() << "[TEST FAIL]" << tag
                         << "degree у токена" << expect.checkDegreeId
                         << "ожидался = пусто";
            }
            QVERIFY(!node->features.degree.has_value());
        } else {
            QString actual;
            if (node->features.degree.has_value()) {
                switch (node->features.degree.value()) {
                case DegreeValue::Pos: actual = "Pos"; break;
                case DegreeValue::Cmp: actual = "Cmp"; break;
                case DegreeValue::Sup: actual = "Sup"; break;
                }
            }
            if (actual != expect.expectedDegree) {
                qDebug() << "[TEST FAIL]" << tag
                         << "degree у токена" << expect.checkDegreeId
                         << "ожидался =" << expect.expectedDegree
                         << "получено =" << actual;
            }
            QCOMPARE(actual, expect.expectedDegree);
        }
    }

    // === 15. FEATS: Case ===============================================
    if (expect.checkCaseId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkCaseId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (expect.expectedCase == "_") {
            // Пропустить проверку.
        } else if (expect.expectedCase.isEmpty()) {
            if (node->features.caseValue.has_value()) {
                qDebug() << "[TEST FAIL]" << tag
                         << "caseValue у токена" << expect.checkCaseId
                         << "ожидался = пусто";
            }
            QVERIFY(!node->features.caseValue.has_value());
        } else {
            QString actual;
            if (node->features.caseValue.has_value()) {
                actual = (node->features.caseValue.value() == CaseValue::Nom) ? "Nom" : "Acc";
            }
            if (actual != expect.expectedCase) {
                qDebug() << "[TEST FAIL]" << tag
                         << "caseValue у токена" << expect.checkCaseId
                         << "ожидался =" << expect.expectedCase
                         << "получено =" << actual;
            }
            QCOMPARE(actual, expect.expectedCase);
        }
    }

    // === 16. FEATS: Person =============================================
    if (expect.checkPersonId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkPersonId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (expect.expectedPerson == "_") {
            // Пропустить проверку.
        } else if (expect.expectedPerson.isEmpty()) {
            if (!node->features.person.isEmpty()) {
                qDebug() << "[TEST FAIL]" << tag
                         << "person у токена" << expect.checkPersonId
                         << "ожидался = пусто, получено =" << node->features.person;
            }
            QVERIFY(node->features.person.isEmpty());
        } else {
            if (node->features.person != expect.expectedPerson) {
                qDebug() << "[TEST FAIL]" << tag
                         << "person у токена" << expect.checkPersonId
                         << "ожидался =" << expect.expectedPerson
                         << "получено =" << node->features.person;
            }
            QCOMPARE(node->features.person, expect.expectedPerson);
        }
    }

    // === 17. FEATS: булевы флаги =======================================
    if (expect.hasBoolCheck) {
        TokenNode* node = model.tokensById.value(expect.checkBoolId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (node->features.poss != expect.expectedPoss) {
            qDebug() << "[TEST FAIL]" << tag
                     << "poss у токена" << expect.checkBoolId
                     << "ожидался =" << expect.expectedPoss
                     << "получено =" << node->features.poss;
        }
        QCOMPARE(node->features.poss, expect.expectedPoss);

        if (node->features.polarityNeg != expect.expectedPolarityNeg) {
            qDebug() << "[TEST FAIL]" << tag
                     << "polarityNeg у токена" << expect.checkBoolId
                     << "ожидался =" << expect.expectedPolarityNeg
                     << "получено =" << node->features.polarityNeg;
        }
        QCOMPARE(node->features.polarityNeg, expect.expectedPolarityNeg);

        if (node->features.numTypeOrd != expect.expectedNumTypeOrd) {
            qDebug() << "[TEST FAIL]" << tag
                     << "numTypeOrd у токена" << expect.checkBoolId
                     << "ожидался =" << expect.expectedNumTypeOrd
                     << "получено =" << node->features.numTypeOrd;
        }
        QCOMPARE(node->features.numTypeOrd, expect.expectedNumTypeOrd);

        if (node->features.voicePass != expect.expectedVoicePass) {
            qDebug() << "[TEST FAIL]" << tag
                     << "voicePass у токена" << expect.checkBoolId
                     << "ожидался =" << expect.expectedVoicePass
                     << "получено =" << node->features.voicePass;
        }
        QCOMPARE(node->features.voicePass, expect.expectedVoicePass);
    }

    // === 18. MWT: isMwtFragment ========================================
    if (expect.checkMwtId != -1) {
        TokenNode* node = model.tokensById.value(expect.checkMwtId, nullptr);
        QVERIFY2(node != nullptr, qPrintable(tag + ": токен не найден"));
        if (node->isMwtFragment != expect.expectedMwtFragment) {
            qDebug() << "[TEST FAIL]" << tag
                     << "isMwtFragment у токена" << expect.checkMwtId
                     << "ожидался =" << expect.expectedMwtFragment
                     << "получено =" << node->isMwtFragment;
        }
        QCOMPARE(node->isMwtFragment, expect.expectedMwtFragment);
    }

    // Множественная проверка MWT: список токенов с ожидаемыми флагами.
    if (!expect.checkMwtIds.isEmpty()) {
        QVERIFY(expect.checkMwtIds.size() == expect.expectedMwtFragments.size());
        for (int i = 0; i < expect.checkMwtIds.size(); ++i) {
            int tokenId = expect.checkMwtIds[i];
            bool expectedFlag = expect.expectedMwtFragments[i];
            TokenNode* node = model.tokensById.value(tokenId, nullptr);
            QVERIFY2(node != nullptr, qPrintable(tag + ": токен " + QString::number(tokenId) + " не найден"));
            if (node->isMwtFragment != expectedFlag) {
                qDebug() << "[TEST FAIL]" << tag
                         << "isMwtFragment у токена" << tokenId
                         << "ожидался =" << expectedFlag
                         << "получено =" << node->isMwtFragment;
            }
            QCOMPARE(node->isMwtFragment, expectedFlag);
        }
    }

    // === 19. MWT: количество записей ===================================
    if (expect.expectedMwtRecordsCount != -1) {
        int actual = static_cast<int>(model.mwtRecords.size());
        if (actual != expect.expectedMwtRecordsCount) {
            qDebug() << "[TEST FAIL]" << tag
                     << "mwtRecords.size(): ожидалось =" << expect.expectedMwtRecordsCount
                     << "получено =" << actual;
        }
        QCOMPARE(actual, expect.expectedMwtRecordsCount);
    }
}