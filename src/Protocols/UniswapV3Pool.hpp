#ifndef UNISWAP_V3_POOL_HPP
#define UNISWAP_V3_POOL_HPP

#include <cmath>
#include "../Utilities/Utilities.hpp"

class UniswapV3Pool : public PoolInterface {
public:
    UniswapV3Pool(std::unordered_map<Token *, double> quantities,
                  double pool_fee, 
                  double slippage_controller) : PoolInterface(quantities, pool_fee) {
        if (slippage_controller < 1) {
            throw std::invalid_argument("invalid slippage controller");
        } else {
            slippage_controller_ = slippage_controller;
        }
        weights_ = quantities;
    }
    double getWeight(Token *token) const {
        return weights_.find(token) -> second;
    }

private:
    std::unordered_map<Token *, double> weights_;
    double slippage_controller_;

    double ComputeInvariant() const {
        double ans = 1;

        for(auto [token, weight] : weights_)
            ans *= weight;
        
        ans *= slippage_controller_;
        ans /= (sqrt(slippage_controller_) - 1);
        ans /= (sqrt(slippage_controller_) - 1);

        return ans;
    }
    double ComputeSpotExchangeRate(Token *input_token, Token *output_token) const {
        double r1 = GetQuantity(input_token), C1 = getWeight(input_token);
        double r2 = GetQuantity(output_token), C2 = getWeight(output_token);

        return (r1 + C1 / (sqrt(slippage_controller_) - 1)) / (r2 + C2 / (sqrt(slippage_controller_) - 1));
    }
    double ComputeSwappedQuantity(Token *input_token, Token *output_token, double input_quantity) const {
        double r1 = GetQuantity(input_token), C1 = getWeight(input_token);
        double r2 = GetQuantity(output_token), C2 = getWeight(output_token);

        double B = sqrt(slippage_controller_) - 1;

        double r1_prime = (r1 + input_quantity);
        double r2_prime = ComputeInvariant() / (r1_prime + C1 / B) - C2 / B;

        return r2 - r2_prime;
    }
};

#endif