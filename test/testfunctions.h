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
#ifndef CP_TESTFUNCTIONS_H
#define CP_TESTFUNCTIONS_H 1

#include <calculusprime/IFunction.h>

namespace CalculusPrime {

class GetProdschablFunction : public IFunction
{
public:
    GetProdschablFunction()
        : m_name("GetProdschabl")
        , m_numberOfArgs(2)
    {
    }

    virtual ~GetProdschablFunction();

    virtual const std::string& getName() const override
    {
        return m_name;
    }

    virtual std::size_t getNumberOfArgs() const override
    {
        return m_numberOfArgs;
    }

    virtual Value execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_) override;

private:
    const std::string m_name;
    const int m_numberOfArgs;
};


class GetSachtarifFunction : public IFunction
{
public:
    GetSachtarifFunction()
        : m_name("GetSachtarif")
        , m_numberOfArgs(1)
    {
    }

    virtual ~GetSachtarifFunction();

    virtual const std::string& getName() const override
    {
        return m_name;
    }

    virtual std::size_t getNumberOfArgs() const override
    {
        return m_numberOfArgs;
    }

    virtual Value execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_) override;

private:
    const std::string m_name;
    const int m_numberOfArgs;
};


class GetTarifpraemieFunction : public IFunction
{
public:
    GetTarifpraemieFunction()
        : m_name("GetTarifpraemie")
        , m_numberOfArgs(3)
    {
    }

    virtual ~GetTarifpraemieFunction();

    virtual const std::string& getName() const override
    {
        return m_name;
    }

    virtual std::size_t getNumberOfArgs() const override
    {
        return m_numberOfArgs;
    }

    virtual Value execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_) override;

private:
    const std::string m_name;
    const int m_numberOfArgs;
};

} // namespace CalculusPrime

#endif // #ifndef CP_TESTFUNCTIONS_H
