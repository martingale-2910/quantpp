#ifndef RNG_HXX
#define RNG_HXX

#include <random> // std::mt19937, std::normal_distribution
#include <chrono> // std::chrono::high_resolution_clock
#include <algorithm> // std::generate

#include "numeric.hxx"

static ulong seed{static_cast<ulong>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};
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

#endif // RNG_HXX