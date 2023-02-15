#ifndef QUANTMC_HXX
#define QUANTMC_HXX

#include "numeric.hxx"
#include "model.hxx"
#include "option.hxx"
#include "quantsim.hxx"

template<Right right>
double compute_mc_price(BS const & model, EuropeanOption<right> const & option, double s0, std::size_t npaths, std::size_t nsteps)
{
    double dt = option.ttm / nsteps;
    vecd st = simulate_model_values(model, vecd(s0, npaths), dt, nsteps);
    vecd v0 = model.discount_factor(option.ttm)*(option.payoff(st));
    return v0.sum() / v0.size();
};

#endif // QUANTMC_HXX