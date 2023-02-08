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

    // TODO: How to work-around the necessity of having a dft constructor?
    BS() = default;
    BS(double r, double vol): r(r), vol(vol) {};

    double discount_factor(double t) const
    {
        return std::exp(-this->r * t);
    };
};

// MODEL SIMULATIONS

vecd compute_bs_step(BS const & bs, vecd & s, double dt, vecd const & z)
{
    s = std::move(s * (1 + bs.r * dt + bs.vol * std::sqrt(dt) * z));
    return s;
};

vecd simulate_bs_values(BS const & bs, double s0, double dt, uint npaths, uint nsteps)
{
    vecd st = vecd(s0, npaths);
    if (DEBUG)
    {
        std::cout << "[0]" <<  " st := " << st.sum() / st.size() << std::endl;
    };
    for(uint i = 0; i < nsteps; ++i)
    {
        vecd z = generate_stdnorm(npaths);
        st = std::move(st * (1 + bs.r * dt + bs.vol * std::sqrt(dt) * z));
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
struct Payoff;

template<>
struct Payoff<Right::CALL>
{
    static double compute(double s, double k)
    {
        return std::max(s - k, 0.0);
    };
};

template<>
struct Payoff<Right::PUT>
{
    static double compute(double s, double k)
    {
        return std::max(k - s, 0.0);
    }; 
};

template<Right right>
constexpr auto vanilla_payoff = &(Payoff<right>::compute);

enum class Style
{
    EUROPEAN, AMERICAN
};

template<Right right, Style style>
struct Option
{
    double strike;
    double ttm;

    // TODO: How to work-around the necessity of having a dft constructor?
    Option() = default;
    Option(double strike, double ttm): strike(strike), ttm(ttm) {};
    double payoff(double s) const { return vanilla_payoff<right>(s, this->strike); };
    vecd payoff(vecd const & s) const { return (s - this->strike).apply([](double xi)->double{ return vanilla_payoff<right>(xi, 0.0); }); };
};

using EuropeanCall = Option<Right::CALL, Style::EUROPEAN>;
using EuropeanPut = Option<Right::PUT, Style::EUROPEAN>;

using AmericanCall = Option<Right::CALL, Style::AMERICAN>;
using AmericanPut = Option<Right::PUT, Style::AMERICAN>;

// MC VALUE COMPUTATIONS

// TODO: How to work-around the necessity of having a constructor?
template<typename Model, typename Option>
struct MCValue
{
    MCValue(Model const & model, Option const & option);
};

template<typename EuropeanOption>
struct MCValue<BS, EuropeanOption>
{
    BS model;
    EuropeanOption option;

    MCValue(BS const & model, EuropeanOption const & option)
    {
        this->model = model;
        this->option = option;
    };

    double compute(double s0, uint npaths, uint nsteps) const
    {
        double dt = this->option.ttm / nsteps;
        vecd st = simulate_bs_values(this->model, s0, dt, npaths, nsteps);
        vecd v0 = this->model.discount_factor(this->option.ttm)*(this->option).payoff(st);
        return v0.sum() / v0.size();
    };
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
    double val1 = MCValue(model, callopt).compute(s0, npaths, nsteps);
    double val2 = MCValue(model, putopt).compute(s0, npaths, nsteps);
    std::cout << "[CALL] mc_value := " << val1 << std::endl;
    std::cout << "[PUT] mc_value := " << val2 << std::endl;

    return 0;
};