#include <iostream>
#include <vector>
#include <valarray>
#include <tuple>
#include <random>
#include <chrono>
#include <cmath>

using vecd = std::valarray<double>;
// using vecd = std::vector<double>;

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::mt19937 rng = std::mt19937(seed);
std::normal_distribution<double> stdnormd = std::normal_distribution<>(0.0, 1.0);;

double generate_stdnorm()
{
    return stdnormd(rng);
};

vecd generate_stdnormv(uint nsamples)
{
    vecd res(nsamples);
    for(uint i = 0; i < nsamples; ++i)
    {
        res[i] = generate_stdnorm();
    }
    return res;
};

struct BS
{
    double r;
    double vol;

    BS(double r, double vol): r(r), vol(vol) {}

    double discount_factor(double t) const
    {
        return std::exp(-this->r * t);
    }
};

vecd simulate_bs_values(BS const & bs, double s0, double dt, uint npaths, uint nsteps)
{
    vecd st = vecd(s0, npaths);
    std::cout << "[0]" <<  " st := " << st.sum() / st.size() << std::endl;
    for(uint i = 0; i < nsteps; ++i)
    {
        vecd z = generate_stdnormv(nsteps);
        st = st * (1 + bs.r * dt + bs.vol * std::sqrt(dt) * z);
        std::cout << "[" << i + 1 << "]" <<  " st := " << st.sum() / st.size() << std::endl;
    }
    return st;
};

enum class Right
{
    CALL, PUT
};


template<Right right>
double vanilla_payoff(double s, double k)
{
    if(right == Right::CALL)
    {
        return std::max(s - k, 0.0);
    }
    else
    {
        return std::max(k - s, 0.0);
    }
}

enum class Style
{
    EUROPEAN, AMERICAN
};

template<Right right, Style style>
struct Option
{
    double strike;
    double ttm;
    // static const Right right = _right;
    // static const Style style = _style;

    Option(double strike, double ttm): strike(strike), ttm(ttm) {}
    double payoff(double s) const { return vanilla_payoff<right>(s, this->strike); }
    vecd payoff(vecd const & s) const { return (s - this->strike).apply([](double xi)->double{ return vanilla_payoff<right>(xi, 0.0); }); }
    
};

using EuropeanCall = Option<Right::CALL, Style::EUROPEAN>;
using EuropeanPut = Option<Right::PUT, Style::EUROPEAN>;

template<typename Option>
double compute_mc_price(BS const & model, Option const & option, double s0, uint npaths, uint nsteps)
{
    double dt = option.ttm / nsteps;
    vecd st = simulate_bs_values(model, s0, dt, npaths, nsteps);
    vecd v0 = model.discount_factor(option.ttm)*option.payoff(st);
    return v0.sum() / v0.size();
}

int main(int argc, char** argv)
{
    double r = 0.05;
    double vol = 0.2;
    auto model = BS(r, vol);

    double k = 110;
    double t = 1.0;

    auto option = EuropeanCall(k, t);

    double s0 = 100;
    uint npaths = 10000;
    uint nsteps = 100;

    std::cout << "Using seed " << seed << std::endl;
    double mc_value = compute_mc_price<>(model, option, s0, npaths, nsteps);
    std::cout << "mc_value:=" << mc_value << std::endl;

    return 0;
}