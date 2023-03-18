//
//  calculate_values.hpp
//  
//
//  Created by Lucas on 18/03/2023.
//

#ifndef calculate_values_hpp
#define calculate_values_hpp

#include "constantblackscholesprocess.hpp"
#include <ql/pricingengines/vanilla/mcvanillaengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/termstructures/volatility/equityfx/blackvariancecurve.hpp>
#include <boost/shared_ptr.hpp>
#include <ql/quantlib.hpp>
#include <iostream>

namespace QuantLib{



void calculate_values(const boost::shared_ptr<GeneralizedBlackScholesProcess>& BS_process, double time_of_extraction, double strike, double* volatility_, double *riskFreeRate_, double *dividend_, double *underlyingValue_);


}



#endif /* calculate_values_hpp */
