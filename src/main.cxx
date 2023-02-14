#include <iostream> // std::cout, std::endl
#include <valarray> // std::valarray
#include <random> // std::mt19937, std::normal_distribution
#include <algorithm> // std::generate, std::find
#include <chrono> // std::chrono::high_resolution_clock
#include <cmath> // std::exp, std::sqrt
#include <string_view> // std::string_view
#include <string> // std::string
#include <sstream> // std::ostringstream

// ARG PARSE

// std::unordered_map<std::string, std::string> get_cmd_options(int argc, char ** argv, std::vector<std::string> options)
// {

//     char ** itr = std::find(begin, end, option);
//     if (itr != end && ++itr != end)
//     {
//         return (T)(*itr);
//     }
//     return dftval;
// }

// bool cmd_option_exists(char** begin, char** end, const std::string& option)
// {
//     return std::find(begin, end, option) != end;
// }


// FORMAT

template<typename... Ts>
std::string format(std::string_view str_view, Ts... values)
{
    std::ostringstream oss;
    auto format_single = [&oss, &str_view] (auto value)
        {
            std::size_t open_bracket_pos = str_view.find('{');
            if (open_bracket_pos == std::string::npos)
            {
                return;
            }
            else
            {
                std::size_t close_bracket_pos = str_view.find('}', open_bracket_pos + 1);
                if (close_bracket_pos == std::string::npos) 
                { 
                    return;
                }
                else
                {
                    oss << str_view.substr(0, open_bracket_pos) << value;
                    str_view = str_view.substr(close_bracket_pos + 1);
                }
            }
        };
    (format_single(values),...);
    oss << str_view;
    return oss.str();
}

// NUMERIC

using vecd = std::valarray<double>;
using matd = std::valarray<vecd>;
using ulong = unsigned long;

// RNG

static const ulong seed{static_cast<ulong>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};
static std::mt19937 rng{seed};
static std::normal_distribution<double> stdnormd{0.0, 1.0};

double generate_stdnorm()
{
    return stdnormd(rng);
};

vecd generate_stdnorm(std::size_t nsamples)
{
    vecd res(nsamples);
    std::generate(std::begin(res), std::end(res), []()->double { return generate_stdnorm(); });
    return res;
};

// MODELS

struct BS
{
    double r;
    double vol;

    BS(double r, double vol): r(r), vol(vol) {};

    double discount_factor(double t) const
    {
        return std::exp(-this->r * t);
    };
};

// MODEL SIMULATIONS

vecd compute_model_step(BS const & bs, vecd & s, double dt, vecd const & z)
{
    s = std::move(s * (1 + bs.r * dt + bs.vol * std::sqrt(dt) * z));
    return s;
};

vecd simulate_model_values(BS const & bs, vecd const & s0, double dt, std::size_t nsteps)
{
    vecd sti = s0;
    std::size_t npaths = s0.size();
    for (std::size_t i = 0; i < nsteps; ++i)
    {
        vecd z = generate_stdnorm(npaths);
        sti = std::move(compute_model_step(bs, sti, dt, z));
    }
    return sti;
};

matd simulate_model_paths(BS const & bs, vecd const & s0, double dt, std::size_t nsteps)
{
    vecd sti = s0;
    std::size_t npaths = s0.size();
    matd st(nsteps);
    for (std::size_t i = 1; i < nsteps + 1; ++i)
    {
        vecd z = generate_stdnorm(npaths);
        sti = std::move(compute_model_step(bs, sti, dt, z));
        st[i] = sti;
    }
    return st;
}

// OPTIONS

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

// MC VALUE COMPUTATIONS

template<Right right>
double compute_mc_price(BS const & model, EuropeanOption<right> const & option, double s0, std::size_t npaths, std::size_t nsteps)
{
    double dt = option.ttm / nsteps;
    vecd st = std::move(simulate_model_values(model, vecd(s0, npaths), dt, nsteps));
    vecd v0 = std::move(model.discount_factor(option.ttm)*(option.payoff(st)));
    return v0.sum() / v0.size();
};

int main(int argc, char** argv)
{
    double r = 0.05;
    double vol = 0.2;
    auto model = BS(r, vol);

    double k = 110;
    double t = 1.0;
    
    auto callopt = EuropeanCall(k, t);
    auto putopt = EuropeanPut(k, t);

    double s0 = 100;
    uint npaths = 100000;
    uint nsteps = 240;

    std::cout << "Using seed " << seed << std::endl;
    auto time1 = std::chrono::high_resolution_clock::now().time_since_epoch();
    double val1 = compute_mc_price(model, callopt, s0, npaths, nsteps);
    auto time2 = std::chrono::high_resolution_clock::now().time_since_epoch();
    ulong duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();
    double val2 = compute_mc_price(model, putopt, s0, npaths, nsteps);
    auto time3 = std::chrono::high_resolution_clock::now().time_since_epoch();
    ulong duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time2).count();

    std::cout << format("[CALL] MC price = {}, pricing duration = {}ms\n", val1, duration1);
    std::cout << format("[PUT] MC price = {}, pricing duration = {}ms\n", val2, duration2);
    std::cout << duration2;

    return 0;
};