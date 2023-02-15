#ifndef MODEL_HXX
#define MODEL_HXX

#include<cmath> // std::exp

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

#endif // MODEL_HXX