#pragma once
#ifndef REACTIONDIFFUSION2_REACTIONMODEL_HPP
#define REACTIONDIFFUSION2_REACTIONMODEL_HPP

#include <array>

#include "CellConcentration.hpp"

template <unsigned int ChemicalCount>
class AbstractReactionModel {
public:
    virtual std::array<double, ChemicalCount> update(CellConcentration<ChemicalCount> conc,  std::array<double, ChemicalCount> conv)= 0;
    virtual ~AbstractReactionModel()= default;
};

class GrayScottModel : public AbstractReactionModel<2> {
public:
    explicit GrayScottModel(double feed=0.055, double kill=0.062, double dA=1.0, double dB=0.5)
    : dA(dA), dB(dB), feed(feed), kill(kill) {}

    static GrayScottModel* coral() {
        return new GrayScottModel();
    }

    static GrayScottModel* mitosis() {
        return new GrayScottModel(0.0367, 0.0649);
    }

    std::array<double, 2> update(CellConcentration<2> conc,  std::array<double, 2> conv) override {
        double newConcA = conc[0] + (dA*conv[0] - conc[0]*conc[1]*conc[1] + feed*(1-conc[0]))*1;
        newConcA = std::min(1.0, std::max(0.0, newConcA));

        double newConcB = conc[1] + (dB*conv[1] + conc[0]*conc[1]*conc[1] - (kill+feed)*conc[1])*1;
        newConcB = std::min(1.0, std::max(0.0, newConcB));

        return std::array<double, 2>{newConcA, newConcB};
    }

private:
    double dA=1.0, dB=0.5, feed=0.055, kill=0.062;
};

#endif //REACTIONDIFFUSION2_REACTIONMODEL_HPP
