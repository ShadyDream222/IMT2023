//
//  calculate_values.cpp
//  
//
//  Created by Lucas on 18/03/2023.
//

#include "calculate_values.hpp"
#include "constantblackscholesprocess.hpp"
#include <ql/pricingengines/vanilla/mcvanillaengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/termstructures/volatility/equityfx/blackvariancecurve.hpp>
#include <boost/shared_ptr.hpp>
#include <ql/quantlib.hpp>
#include <iostream>

namespace QuantLib{

//Definition of the function calculate_values
void calculate_values(const boost::shared_ptr<GeneralizedBlackScholesProcess>& BS_process, double time_of_extraction, double strike, double *volatility_, double *riskFreeRate_, double *dividend_, double *underlyingValue_)
{

//Calculate the desired values and send them to the pointer address
    
    *volatility_ = BS_process->blackVolatility()->blackVol(time_of_extraction, strike);

    *riskFreeRate_ = BS_process->riskFreeRate()->zeroRate(time_of_extraction, Continuous);

    *dividend_ = BS_process->dividendYield()->zeroRate(time_of_extraction, Continuous);

    *underlyingValue_ = BS_process->x0();
}
    
}

