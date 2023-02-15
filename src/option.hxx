#ifndef OPTION_HXX
#define OPTION_HXX

#include "numeric.hxx"

enum class Right
{
    CALL, PUT
};

enum class Style
{
    EUROPEAN, AMERICAN
};

template<Right right>
struct vanilla_payoff
{
    static double compute(double s, double k)
    {
        return right == Right::CALL ? std::max(s - k, 0.0) : std::max(k - s, 0.0);
    }

    static vecd compute(vecd const & s, double k)
    {
        if (right == Right::CALL)
        {
            return (s - k).apply([](double xi)->double { return std::max(xi, 0.0); });
        }
        else
        {
            return (k - s).apply([](double xi)->double { return std::max(xi, 0.0); });
        }
    }
};

template<Right right, Style style>
struct Option
{
    double strike;
    double ttm;

    Option(double strike, double ttm): strike(strike), ttm(ttm) {};
    double payoff(double s) const { return vanilla_payoff<right>::compute(s, this->strike); };
    vecd payoff(vecd const & s) const { return vanilla_payoff<right>::compute(s, this->strike); };
};

template<Right right>
using EuropeanOption = Option<right, Style::EUROPEAN>;
using EuropeanCall = EuropeanOption<Right::CALL>;
using EuropeanPut = EuropeanOption<Right::PUT>;

template<Right right>
using AmericanOption = Option<right, Style::AMERICAN>;
using AmericanCall = AmericanOption<Right::CALL>;
using AmericanPut = AmericanOption<Right::PUT>;

#endif // OPTION_HXX