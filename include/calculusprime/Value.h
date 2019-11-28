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
#ifndef CP_VALUE_H
#define CP_VALUE_H 1

#include <cmath>
#include <iosfwd>
#include <boost/optional.hpp>
#include <boost/throw_exception.hpp>
#include <boost/variant.hpp>
#include <calculusprime/internal/require.h>

namespace CalculusPrime {

//! \brief Generic value class for passing Double/String/Boolean values to/from the rating engine
class Value
{
public:
    //! \brief the supported data types
    typedef boost::variant<std::string, double, bool> Variant_t;

    Value()
    {
    }

    explicit Value(const Variant_t& value_)
        : m_value(value_)
    {
    }

    explicit Value(const char* value_)
        : m_value(std::string(value_))
    {
    }

    explicit Value(bool value_)
        : m_value(value_)
    {
    }

    explicit Value(double value_)
        : m_value(value_)
    {
    }

    explicit Value(const std::string& value_)
        : m_value(value_)
    {
    }

    explicit Value(int value_)
        : m_value(double(value_))
    {
    }

    ~Value()
    {
    }

    //! \return if the stored value is void
    bool isVoid() const
    {
        return !m_value;
    }

    //! \return if the stored value is bool
    bool isBool() const
    {
        CP_REQUIRE(m_value);
        const bool* b = boost::get<bool>(&(*m_value));
        return b != nullptr;
    }

    //! \return if the stored value as bool
    bool asBool() const
    {
        CP_REQUIRE(m_value);
        const bool* b = boost::get<bool>(&(*m_value));
        CP_REQUIRE(b != nullptr);
        return *b;
    }

    //! \return if the stored value is string
    bool isString() const
    {
        CP_REQUIRE(m_value);
        const std::string* s = boost::get<std::string>(&(*m_value));
        return s != nullptr;
    }

    //! \return if the stored value as string
    std::string asString() const
    {
        CP_REQUIRE(m_value);
        const std::string* s = boost::get<std::string>(&(*m_value));
        CP_REQUIRE(s != nullptr);
        return *s;
    }

    //! \return if the stored value is double
    bool isDouble() const
    {
        CP_REQUIRE(m_value);
        const double* d = boost::get<double>(&(*m_value));
        return d != nullptr;
    }

    //! \return if the stored value as double
    double asDouble() const
    {
        CP_REQUIRE(m_value);
        const double* d = boost::get<double>(&(*m_value));
        CP_REQUIRE(d != nullptr);
        return *d;
    }

    //! \brief Apply the specified visitor_ to the stored value. if the stored value is void, nothing is executed
    template <typename Visitor>
    void applyVisitor(const Visitor& visitor_) const
    {
        if (m_value) {
            boost::apply_visitor(visitor_, *m_value);
        }
    }

    //! \brief writes the specified value to the stream
    //! \param os_ the output stream
    //! \param value_ the value
    friend std::ostream& operator<< (std::ostream& os_, const Value& value_)
    {
        if (value_.isVoid()) {
            os_ << "VOID";
        }
        else if (value_.isBool()) {
            os_ << std::boolalpha << value_.asBool();
        }
        else if (value_.isDouble()) {
            // this is a hack to remove .0 from double to String conversion
            double d = value_.asDouble();
            long l = (long)d;
            if (d == l) {
                os_ << l;
            }
            else {
                os_ << d;
            }
        }
        else if (value_.isString()) {
            os_ << '\'' << value_.asString() << '\'';
        }
        return os_;
    }

    //! \brief less than operator for two values (may not have void values)
    friend bool operator <(const Value& lhs_, const Value& rhs_)
    {
        if (!lhs_.m_value || !rhs_.m_value) {
            BOOST_THROW_EXCEPTION(std::runtime_error("operator < not applicable for empty values"));
        }
        return *(lhs_.m_value) < *(rhs_.m_value);
    }

    //! \brief equals operator for two values. if one of the values is void, false is returned
    friend bool operator ==(const Value& lhs_, const Value& rhs_)
    {
        if (!lhs_.m_value || !rhs_.m_value) {
            return false;
        }
        if (lhs_.isDouble() && rhs_.isDouble()) {
            double diff = std::abs(lhs_.asDouble() - rhs_.asDouble());
            return diff < 0.00000000001;
        }
        return *(lhs_.m_value) == *(rhs_.m_value);
    }

private:
    boost::optional<Variant_t> m_value;
};

} // namespace CalculusPrime

#endif // #ifndef CP_VALUE_H
