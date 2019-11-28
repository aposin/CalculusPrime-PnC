/*
Copyright 2019 Association for the promotion of open - source insurance software and for the establishment of open interface standards in the insurance industry

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <chrono>
#include <iostream>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/unit_test.hpp>
#include <calculusprime/cache/DefaultCacheFactory.h>
#include <calculusprime/IRatingEngine.h>
#include <calculusprime/RatingEngineException.h>
#include <calculusprime/RatingEngineFactory.h>
#include <calculusprime/Value.h>
#include "testfunctions.h"

using boost::unit_test_framework::test_suite;
using namespace CalculusPrime;

typedef IRatingEngine::Value_t Value_t;
typedef IRatingEngine::InstanceVariable_t InstanceVariable_t;

static const std::string g_dummyVtarKey("ee3812d0-8cf1-11e9-b475-0800200c9a66");

template <class ValueType>
class OStreamVisitor
    : public boost::static_visitor<>
{
public:
    OStreamVisitor(std::ostream& os_)
        : m_os(os_)
    {
    }

    void operator()(const IRatingEngine::Variant_t& variant_) const
    {
        m_os << variant_;
    }
    void operator()(const IRatingEngine::InstanceVariable_t& instanceVariable_) const
    {
        bool first = true;
        for (const IRatingEngine::InstanceVariable_t::value_type& iv : instanceVariable_) {
            if (!first) {
                m_os << ',';
            }
            else {
                first = false;
            }
            m_os << iv.first << '=' << iv.second;
        }
    }
private:
    std::ostream& m_os;
};

namespace boost {
std::ostream& operator<< (std::ostream& os_, const IRatingEngine::Value_t& value_)
{
    boost::apply_visitor(OStreamVisitor<IRatingEngine::Value_t>(os_), value_);
    return os_;
}
}

namespace {
IRatingEngine::Map_t makeInput()
{
    IRatingEngine::Map_t input{
        { "Wohnungswert", 300000.0 },
        { "Gut_checked", "N" },
        { "Besser_checked", "N" },
        { "Beste_checked", "J" },
        { "Er_WohnWert", "B" },
        { "Aqua_1000l_checked", "N" },
        { "Aqua_2000l_checked", "J" },
        { "HP_Hund_checked", "J" },
        { "Anz_Hunde", 2.0 },
        { "HP_Pferd_checked", "J" },
        { "Anz_Pferde", 4.0 },
        { "VS_Geldschrank", 0.0 },
        { "VS_Schmuck", 0.0 },
        { "VS_Schmuck_unversperrt", 0.0 },
        { "VS_Bargeld", 0.0 },
        { "VS_HP_Privat_Sport", 0.0 },
        { "Anzahl_VP", 0.0 }
    };
    return std::move(input);
}

IRatingEngine::Map_t makeInstanceInput()
{
    IRatingEngine::Map_t input{
        { "Wohnungswert", IRatingEngine::InstanceVariable_t { {"1", 300000.0}, {"2", 200000.0} } },
        { "Gut_checked", IRatingEngine::InstanceVariable_t{ { "1", "N" }, { "2", "N" } } },
        { "Besser_checked", IRatingEngine::InstanceVariable_t{ { "1", "N"}, { "2", "J"} } },
        { "Beste_checked", IRatingEngine::InstanceVariable_t{ { "1", "J"}, { "2", "N"} } },
        { "Er_WohnWert", IRatingEngine::InstanceVariable_t{ { "1", "B"}, { "2", "A"} } },
        { "Aqua_1000l_checked", IRatingEngine::InstanceVariable_t{ { "1", "N"}, { "2", "J"} } },
        { "Aqua_2000l_checked", IRatingEngine::InstanceVariable_t{ { "1", "J"}, { "2", "N"} } },
        { "HP_Hund_checked", IRatingEngine::InstanceVariable_t{ { "1", "J"}, { "2", "N"} } },
        { "Anz_Hunde", IRatingEngine::InstanceVariable_t{ { "1", 2.0}, { "2", 0.0} } },
        { "HP_Pferd_checked", IRatingEngine::InstanceVariable_t{ { "1", "J"}, { "2", "N"} } },
        { "Anz_Pferde", IRatingEngine::InstanceVariable_t{ { "1", 4.0}, { "2", 0.0} } },
        { "VS_Geldschrank", IRatingEngine::InstanceVariable_t{ { "1", 0.0 }, { "2", 11000.0} } },
        { "VS_Schmuck", IRatingEngine::InstanceVariable_t{ { "1", 0.0 }, { "2", 20000.0} } },
        { "VS_Schmuck_unversperrt", IRatingEngine::InstanceVariable_t{ { "1", 0.0 }, { "2", 10000.0} } },
        { "VS_Bargeld", IRatingEngine::InstanceVariable_t{ { "1", 0.0 }, { "2", 5000.0} } },
        { "VS_HP_Privat_Sport", IRatingEngine::InstanceVariable_t{ { "", 0.0 } } },
        { "Anzahl_VP", IRatingEngine::InstanceVariable_t{ { "", 0.0 } } }
    };
    return std::move(input);
}

IRatingEngine::RatingFormulaMap_t makeRatingFormulas()
{
    IRatingEngine::RatingFormulaMap_t variables {
        {
            "Praemie_Gut",
            "if (Gut_checked = 'J') then return max(Wohnungswert, GetSachtarif('MINVSGU')) * GetSachtarif('VARGUT1') else    return 0.0 end"
        },
        {
            "Praemie_Besser",
            "if (Besser_checked = 'J') then return max(Wohnungswert, GetSachtarif('MINVSBE')) * GetSachtarif('VARBESSER1') else return 0.0 end"
        },
        {
            "Praemie_Beste",
            "if (Beste_checked = 'J') then return max(Wohnungswert, GetSachtarif('MINVSSB')) * GetSachtarif('VARBESTE1') else return 0.0 end"
        },
        {
            "Summenermittlungsrabatt",
            "if (Er_WohnWert <> 'B') then     return GetSachtarif('HHSUERM') else     return 1 end"
        },
        {
            "Aqua_1000",
            "if (Aqua_1000l_checked = 'J') then    return 1.05 else    return 1 end"
        },
        {
            "Aqua_2000",
            "if (Aqua_2000l_checked = 'J') then    return 1.1 else    return 1 end"
        },
        {
            "Nachlass_Hunde",
            "if (Beste_checked = 'J') then   return GetSachtarif('HHNLHU1') else   return 1 end"
        },
        {
            "Variante",
            "if (Gut_checked = 'J') then   return 'Gut' else      if (Besser_checked = 'J') then          return 'BESSER'     else         return 'SBeste' end"
        },
        {
            "Praemie_pro_VP",
            "if (Anzahl_VP > 0) then     return GetTarifpraemie('VS_HP_Privat_Sport', 0, 'Variante') * Anzahl_VP else     return 0.0 end"
        },
        {
            "Variante_Person",
            "if (Gut_checked = 'J') then   return 'ERGut' else      if (Besser_checked = 'J') then          return 'ERBESSER'     else         return 'ERBeste' end"
        },
        {
            "Test",
            "return (Test1+Test2+Test3+Test4+Test5+Test5+Test6+Test7+Test8+Test9+Test10)"
        },
        {
            "Test1",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSGU')) * GetSachtarif('VARGUT1')"
        },
        {
            "Test2",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSGU')) * GetSachtarif('VARGUT1')"
        },
        {
            "Test3",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSGU')) * GetSachtarif('VARGUT1')"
        },
        {
            "Test4",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSGU')) * GetSachtarif('VARGUT1')"
        },
        {
            "Test5",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSGU')) * GetSachtarif('VARGUT1')"
        },
        {
            "Test6",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSSB')) * GetSachtarif('VARBESTE1')"
        },
        {
            "Test7",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSSB')) * GetSachtarif('VARBESTE1')"
        },
        {
            "Test8",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSSB')) * GetSachtarif('VARBESTE1')"
        },
        {
            "Test9",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSSB')) * GetSachtarif('VARBESTE1')"
        },
        {
            "Test10",
            "return 0*max(Wohnungswert,  GetSachtarif('MINVSSB')) * GetSachtarif('VARBESTE1')"
        }
    };
    return std::move(variables);
}

std::vector<RatingOutput> makeRatingOutput()
{
    std::vector<RatingOutput> output{
        {
            "Wohnungspraemie",
            0,
            "return (Praemie_Gut + Praemie_Besser + Praemie_Beste)*Aqua_1000*Aqua_2000*Summenermittlungsrabatt+Test",
            ""
        },
        {
            "Elektronikpauschalpraemie",
            0,
            "if (Beste_checked = 'J') then      return max(Wohnungswert, GetSachtarif('MINVSEL')) * GetSachtarif('HHPAUVS1') else      return 0.0 end",
            ""
        },
        {
            "HP_Hunde_Praemie",
            0,
            "if (HP_Hund_checked = 'J') then     return GetSachtarif('HHZDHU1')*Anz_Hunde * Nachlass_Hunde else     return 0 end",
            ""
        },
        {
            "HP_Pferde_Praemie",
            0,
            "if (HP_Pferd_checked = 'J') then      return GetSachtarif('HHZDPF1')*Anz_Pferde else     return 0 end",
            ""
        },
        {
            "Geldschrank_Praemie",
            0,
            "if (VS_Geldschrank > 0)  then    return GetSachtarif('GESCHR1') * (VS_Geldschrank - GetProdschabl('HH50123','MINDSUM')) else    return 0.0 end",
            ""
        },
        {
            "Schmuck_Praemie",
            0,
            "if (VS_Schmuck > 0)  then    return GetSachtarif('SCHMVER1') * (VS_Schmuck - GetProdschabl('HH50122','MINDSUM')) else    return 0.0 end",
            ""
        },
        {
            "Schmuck_unversperrrt_Praemie",
            0,
            "if (VS_Schmuck_unversperrt > 0)  then    return GetSachtarif('SCHMUNV1') * (VS_Schmuck_unversperrt - GetProdschabl('HH50121','MINDSUM')) else    return 0.0 end",
            ""
        },
        {
            "Bargeld_Praemie",
            0,
            "if (VS_Bargeld> 0)  then    return GetSachtarif('BARGELD') * (VS_Bargeld - GetProdschabl('HH50120','MINDSUM')) else    return 0.0 end",
            ""
        },
        {
            "HP_Privat_Sport_Praemie",
            0,
            "if (VS_HP_Privat_Sport > 0) then   return GetTarifpraemie('HH1245', VS_HP_Privat_Sport, 0, Variante) + Praemie_pro_VP else    return 0.0 end",
            ""
        }
    };
    return std::move(output);
}

std::vector<RatingOutput> makeInstanceRatingOutput()
{
    std::vector<RatingOutput> output{
        {
            "Wohnungspraemie",
            0,
            "return (Praemie_Gut + Praemie_Besser + Praemie_Beste)*Aqua_1000*Aqua_2000*Summenermittlungsrabatt+Test",
            "1"
        },
        {
            "Elektronikpauschalpraemie",
            0,
            "if (Beste_checked = 'J') then      return max(Wohnungswert, GetSachtarif('MINVSEL')) * GetSachtarif('HHPAUVS1') else      return 0.0 end",
            "1"
        },
        {
            "HP_Hunde_Praemie",
            0,
            "if (HP_Hund_checked = 'J') then     return GetSachtarif('HHZDHU1')*Anz_Hunde * Nachlass_Hunde else     return 0 end",
            "1"
        },
        {
            "HP_Pferde_Praemie",
            0,
            "if (HP_Pferd_checked = 'J') then      return GetSachtarif('HHZDPF1')*Anz_Pferde else     return 0 end",
            "1"
        },
        {
            "Geldschrank_Praemie",
            0,
            "if (VS_Geldschrank > 0)  then    return GetSachtarif('GESCHR1') * (VS_Geldschrank - GetProdschabl('HH50123','MINDSUM')) else    return 0.0 end",
            "1"
        },
        {
            "Schmuck_Praemie",
            0,
            "if (VS_Schmuck > 0)  then    return GetSachtarif('SCHMVER1') * (VS_Schmuck - GetProdschabl('HH50122','MINDSUM')) else    return 0.0 end",
            "1"
        },
        {
            "Schmuck_unversperrrt_Praemie",
            0,
            "if (VS_Schmuck_unversperrt > 0)  then    return GetSachtarif('SCHMUNV1') * (VS_Schmuck_unversperrt - GetProdschabl('HH50121','MINDSUM')) else    return 0.0 end",
            "1"
        },
        {
            "Bargeld_Praemie",
            0,
            "if (VS_Bargeld> 0)  then    return GetSachtarif('BARGELD') * (VS_Bargeld - GetProdschabl('HH50120','MINDSUM')) else    return 0.0 end",
            "1"
        },
        {
            "HP_Privat_Sport_Praemie",
            0,
            "if (VS_HP_Privat_Sport > 0) then   return GetTarifpraemie('HH1245', VS_HP_Privat_Sport, 0, Variante) + Praemie_pro_VP else    return 0.0 end",
            "1"
        },

        {
            "Wohnungspraemie",
            0,
            "return (Praemie_Gut + Praemie_Besser + Praemie_Beste)*Aqua_1000*Aqua_2000*Summenermittlungsrabatt+Test",
            "2"
        },
        {
            "Elektronikpauschalpraemie",
            0,
            "if (Beste_checked = 'J') then      return max(Wohnungswert, GetSachtarif('MINVSEL')) * GetSachtarif('HHPAUVS1') else      return 0.0 end",
            "2"
        },
        {
            "HP_Hunde_Praemie",
            0,
            "if (HP_Hund_checked = 'J') then     return GetSachtarif('HHZDHU1')*Anz_Hunde * Nachlass_Hunde else     return 0 end",
            "2"
        },
        {
            "HP_Pferde_Praemie",
            0,
            "if (HP_Pferd_checked = 'J') then      return GetSachtarif('HHZDPF1')*Anz_Pferde else     return 0 end",
            "2"
        },
        {
            "Geldschrank_Praemie",
            0,
            "if (VS_Geldschrank > 0)  then    return GetSachtarif('GESCHR1') * (VS_Geldschrank - GetProdschabl('HH50123','MINDSUM')) else    return 0.0 end",
            "2"
        },
        {
            "Schmuck_Praemie",
            0,
            "if (VS_Schmuck > 0)  then    return GetSachtarif('SCHMVER1') * (VS_Schmuck - GetProdschabl('HH50122','MINDSUM')) else    return 0.0 end",
            "2"
        },
        {
            "Schmuck_unversperrrt_Praemie",
            0,
            "if (VS_Schmuck_unversperrt > 0)  then    return GetSachtarif('SCHMUNV1') * (VS_Schmuck_unversperrt - GetProdschabl('HH50121','MINDSUM')) else    return 0.0 end",
            "2"
        },
        {
            "Bargeld_Praemie",
            0,
            "if (VS_Bargeld> 0)  then    return GetSachtarif('BARGELD') * (VS_Bargeld - GetProdschabl('HH50120','MINDSUM')) else    return 0.0 end",
            "2"
        },
        {
            "HP_Privat_Sport_Praemie",
            0,
            "if (VS_HP_Privat_Sport > 0) then   return GetTarifpraemie('HH1245', VS_HP_Privat_Sport, 0, Variante) + Praemie_pro_VP else    return 0.0 end",
            "2"
        }

    };
    return std::move(output);
}

void dumpResults(const IRatingEngine::Map_t& results_)
{
    // comment this in if needed
    //for (const IRatingEngine::Map_t::value_type& result : results_) {
    //    std::cout << result.first << ' ';
    //    boost::apply_visitor(OStreamVisitor<IRatingEngine::IRatingEngine::Value_t>(std::cout), result.second);
    //    std::cout << '\n';
    //}
}

void validateResults(IRatingEngine::Map_t& results_)
{
    BOOST_CHECK_EQUAL(results_["Wohnungspraemie"], Value_t(1320));
    BOOST_CHECK_EQUAL(results_["Elektronikpauschalpraemie"], Value_t(600));
    BOOST_CHECK_EQUAL(results_["Bargeld_Praemie"], Value_t(0));
    BOOST_CHECK_EQUAL(results_["HP_Hunde_Praemie"], Value_t(210));
    BOOST_CHECK_EQUAL(results_["HP_Pferde_Praemie"], Value_t(600));
    BOOST_CHECK_EQUAL(results_["HP_Privat_Sport_Praemie"], Value_t(0));
    BOOST_CHECK_EQUAL(results_["Geldschrank_Praemie"], Value_t(0));
    BOOST_CHECK_EQUAL(results_["Schmuck_Praemie"], Value_t(0));
    BOOST_CHECK_EQUAL(results_["Schmuck_unversperrrt_Praemie"], Value_t(0));
}

void validateInstanceResults(IRatingEngine::Map_t& results_)
{
    BOOST_CHECK_EQUAL(results_["Wohnungspraemie"], Value_t(InstanceVariable_t{ { "1", 1320 }, { "2", 623.7 } }));
    BOOST_CHECK_EQUAL(results_["Elektronikpauschalpraemie"], Value_t(InstanceVariable_t{ { "1", 600 }, { "2", 0 } }));
    BOOST_CHECK_EQUAL(results_["Bargeld_Praemie"], Value_t(InstanceVariable_t{ { "1", 0 }, { "2", 450 } }));
    BOOST_CHECK_EQUAL(results_["HP_Hunde_Praemie"], Value_t(InstanceVariable_t{ { "1", 210 }, { "2", 0 } }));
    BOOST_CHECK_EQUAL(results_["HP_Pferde_Praemie"], Value_t(InstanceVariable_t{ { "1", 600 }, { "2", 0 } }));
    BOOST_CHECK_EQUAL(results_["HP_Privat_Sport_Praemie"], Value_t(InstanceVariable_t{ { "1", 0 }, { "2", 0 } }));
    BOOST_CHECK_EQUAL(results_["Geldschrank_Praemie"], Value_t(InstanceVariable_t{ { "1", 0 }, { "2", 5 } }));
    BOOST_CHECK_EQUAL(results_["Schmuck_Praemie"], Value_t(InstanceVariable_t{ { "1", 0 }, { "2", 150 } }));
    BOOST_CHECK_EQUAL(results_["Schmuck_unversperrrt_Praemie"], Value_t(InstanceVariable_t{ { "1", 0 }, { "2", 900 } }));
}
}

void testCalculation()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(DefaultCacheFactory::getFunctionResultCache(g_dummyVtarKey));
    std::shared_ptr<IParseTreeCache> parseTreeCache(DefaultCacheFactory::getParseTreeCache(g_dummyVtarKey));
    std::vector<std::shared_ptr<IFunction>> functions{ std::make_shared<GetProdschablFunction>(), std::make_shared<GetSachtarifFunction>(), std::make_shared<GetTarifpraemieFunction>() };

    std::unique_ptr<IRatingEngine> ratingEngine(RatingEngineFactory::createRatingEngine(functions, functionResultCache, parseTreeCache));

    IRatingEngine::Map_t input(makeInput());
    IRatingEngine::RatingFormulaMap_t ratingFormulas(makeRatingFormulas());
    std::vector<RatingOutput> ratingOutput(makeRatingOutput());

    IRatingEngine::Map_t results = ratingEngine->calculate(g_dummyVtarKey, input, ratingFormulas, ratingOutput);
    dumpResults(results);
    validateResults(results);
}

void testInstanceCalculation()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(DefaultCacheFactory::getFunctionResultCache(g_dummyVtarKey));
    std::shared_ptr<IParseTreeCache> parseTreeCache(DefaultCacheFactory::getParseTreeCache(g_dummyVtarKey));
    std::vector<std::shared_ptr<IFunction>> functions{ std::make_shared<GetProdschablFunction>(), std::make_shared<GetSachtarifFunction>(), std::make_shared<GetTarifpraemieFunction>() };

    std::unique_ptr<IRatingEngine> ratingEngine(RatingEngineFactory::createRatingEngine(functions, functionResultCache, parseTreeCache));

    IRatingEngine::Map_t input(makeInstanceInput());
    IRatingEngine::RatingFormulaMap_t ratingFormulas(makeRatingFormulas());
    std::vector<RatingOutput> ratingOutput(makeInstanceRatingOutput());

    IRatingEngine::Map_t results = ratingEngine->calculate(g_dummyVtarKey, input, ratingFormulas, ratingOutput);
    dumpResults(results);
    validateInstanceResults(results);
}

void testCalculationPerformance()
{
    DefaultCacheFactory::clear();
    std::shared_ptr<IFunctionResultCache> functionResultCache(DefaultCacheFactory::getFunctionResultCache(g_dummyVtarKey));
    std::shared_ptr<IParseTreeCache> parseTreeCache(DefaultCacheFactory::getParseTreeCache(g_dummyVtarKey));
    std::vector<std::shared_ptr<IFunction>> functions{ std::make_shared<GetProdschablFunction>(), std::make_shared<GetSachtarifFunction>(), std::make_shared<GetTarifpraemieFunction>() };

    std::unique_ptr<IRatingEngine> ratingEngine(RatingEngineFactory::createRatingEngine(functions, functionResultCache, parseTreeCache));

    IRatingEngine::Map_t input(makeInput());
    IRatingEngine::RatingFormulaMap_t ratingFormulas(makeRatingFormulas());
    std::vector<RatingOutput> ratingOutput(makeRatingOutput());

    typedef std::chrono::high_resolution_clock Clock_t;
    std::chrono::time_point<Clock_t> startTime, endTime;

    startTime = Clock_t::now();
    IRatingEngine::Map_t results = ratingEngine->calculate(g_dummyVtarKey, input, ratingFormulas, ratingOutput);
    endTime = Clock_t::now();
    std::cout << "duration first calculation: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << " microseconds\n";
    validateResults(results);

    startTime = Clock_t::now();
    results = ratingEngine->calculate(g_dummyVtarKey, input, ratingFormulas, ratingOutput);
    endTime = Clock_t::now();
    std::cout << "duration second calculation: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << " microseconds\n";
    validateResults(results);

    startTime = Clock_t::now();
    results = ratingEngine->calculate(g_dummyVtarKey, input, ratingFormulas, ratingOutput);
    endTime = Clock_t::now();
    std::cout << "duration third calculation: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << " microseconds\n";

    validateResults(results);
}
