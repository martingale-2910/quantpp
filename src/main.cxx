#include <iostream>
#include <valarray>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>

const bool DEBUG = false;

using vecd = std::valarray<double>;
using ulong = unsigned long;

// RNG

static const ulong seed{static_cast<ulong>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};
static std::mt19937 rng{seed};
static std::normal_distribution<double> stdnormd{0.0, 1.0};

double generate_stdnorm()
{
    return stdnormd(rng);
};

vecd generate_stdnorm(uint nsamples)
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

vecd simulate_model_values(BS const & bs, vecd const & s0, double dt, uint nsteps)
{
    vecd st = s0;
    std::size_t npaths = s0.size();
    if (DEBUG)
    {
        std::cout << "[0]" <<  " st := " << st.sum() / st.size() << std::endl;
    };
    for(uint i = 0; i < nsteps; ++i)
    {
        vecd z = generate_stdnorm(npaths);
        st = std::move(compute_model_step(bs, st, dt, z));
        if (DEBUG)
        {
            std::cout << "[" << i + 1 << "]" <<  " st := " << st.sum() / st.size() << std::endl;
        };
    }
    return st;
};

// OPTIONS

enum class Right
{
    CALL, PUT
};

template<Right right>
constexpr auto vanilla_payoff = [](double s, double k)->double { return right == Right::CALL ? std::max(s - k, 0.0) : std::max(k - s, 0.0); };

enum class Style
{
    EUROPEAN, AMERICAN
};

template<Right right, Style style>
struct Option
{
    double strike;
    double ttm;

    Option(double strike, double ttm): strike(strike), ttm(ttm) {};
    double payoff(double s) const { return vanilla_payoff<right>(s, this->strike); };
    vecd payoff(vecd const & s) const { return (s - this->strike).apply([](double xi)->double{ return vanilla_payoff<right>(xi, 0.0); }); };
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
double compute_mc_price(BS const & model, EuropeanOption<right> const & option, double s0, uint npaths, uint nsteps)
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
    double val2 = compute_mc_price(model, putopt, s0, npaths, nsteps);
    auto time3 = std::chrono::high_resolution_clock::now().time_since_epoch();
    std::cout << "[CALL] mc_value := " << val1 << ", duration := " << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() << "ms" << std::endl;
    std::cout << "[PUT] mc_value := " << val2 << ", duration := " << std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time2).count() << "ms" << std::endl;

    return 0;
};