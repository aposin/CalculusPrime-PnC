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
#include "testfunctions.h"
#include <boost/test/unit_test.hpp>

namespace CalculusPrime {

GetProdschablFunction::~GetProdschablFunction()
{
}

Value GetProdschablFunction::execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_)
{
    BOOST_REQUIRE_EQUAL(params_.size(), m_numberOfArgs);

    Value tarRisikoSchlParm = params_[0];
    BOOST_REQUIRE(tarRisikoSchlParm.isString());

    Value attributeParm = params_[1];
    BOOST_REQUIRE(attributeParm.isString());

    std::string tarRisikoSchl = tarRisikoSchlParm.asString();
    std::string attribute = attributeParm.asString();

    if (attribute == "MINDSUM") {
        if (tarRisikoSchl == "HH50123") {
            return Value(10000.0);
        }
        else if (tarRisikoSchl == "HH50122") {
            return Value(15000.0);
        }
        else if (tarRisikoSchl == "HH50121") {
            return Value(1000.0);
        }
        else if (tarRisikoSchl == "HH50120") {
            return Value(500.0);
        }
        else {
            throw std::runtime_error("unknown TARRISIKOSCHL: " + tarRisikoSchl);
        }
    }
    else {
        throw std::runtime_error("unknown ATTRIBUTE: " + attribute);
    }
}


GetSachtarifFunction::~GetSachtarifFunction()
{
}

Value GetSachtarifFunction::execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_)
{
    BOOST_REQUIRE_EQUAL(params_.size(), m_numberOfArgs);

    Value sTarKritParm = params_[0];
    BOOST_REQUIRE(sTarKritParm.isString());

    const std::string sTarKrit = sTarKritParm.asString();

    if (sTarKrit == "MINVSSB") {
        return Value(100000.0);
    }
    else if (sTarKrit == "VARBESTE1") {
        return Value(0.004);
    }
    else if (sTarKrit == "HHNLHU1") {
        return Value(0.7);
    }
    else if (sTarKrit == "MINVSEL") {
        return Value(100000.0);
    }
    else if (sTarKrit == "HHPAUVS1") {
        return Value(0.002);
    }
    else if (sTarKrit == "HHZDHU1") {
        return Value(150.0);
    }
    else if (sTarKrit == "HHZDPF1") {
        return Value(150.0);
    }
    else if (sTarKrit == "MINVSGU") {
        return Value(20000.0);
    }
    else if (sTarKrit == "VARGUT1") {
        return Value(0.002);
    }
    else if (sTarKrit == "MINVSBE") {
        return Value(20000.0);
    }
    else if (sTarKrit == "VARBESSER1") {
        return Value(0.0033);
    }
    else if (sTarKrit == "HHSUERM") {
        return Value(0.9);
    }
    else if (sTarKrit == "GESCHR1") {
        return Value(0.005);
    }
    else if (sTarKrit == "SCHMVER1") {
        return Value(0.03);
    }
    else if (sTarKrit == "SCHMUNV1") {
        return Value(0.10);
    }
    else if (sTarKrit == "BARGELD") {
        return Value(0.10);
    }
    else {
        throw std::runtime_error("unknown STARKRIT: " + sTarKrit);
    }
}


GetTarifpraemieFunction::~GetTarifpraemieFunction()
{
}

Value GetTarifpraemieFunction::execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_)
{
    BOOST_REQUIRE_EQUAL(params_.size(), m_numberOfArgs);

    Value tarifKritParm = params_[0];
    BOOST_REQUIRE(tarifKritParm.isString());

    Value bereichVonParm = params_[1];
    BOOST_REQUIRE(bereichVonParm.isDouble());

    Value klasseVonParm = params_[2];
    BOOST_REQUIRE(klasseVonParm.isDouble());

    std::string tarifKrit = tarifKritParm.asString();
    double bereichVon = bereichVonParm.asDouble();
    double klasseVon = klasseVonParm.asDouble();

    if (tarifKrit == "HH1245") {
        return Value(123.0);
    }
    else {
        throw std::runtime_error("unknown TARIFKRIT: " + tarifKrit);
    }
}

} // namespace CalculusPrime
