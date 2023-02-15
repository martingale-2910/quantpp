#ifndef QUANTSIM_HXX
#define QUANTSIM_HXX

#include <cmath> // std::sqrt

#include "numeric.hxx"
#include "model.hxx"
#include "rng.hxx"

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

#endif // QUANTSIM_HXX