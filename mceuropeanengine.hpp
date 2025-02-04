/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2007, 2008 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file mceuropeanengine.hpp
    \brief Monte Carlo European option engine
*/

#ifndef montecarlo_european_engine_hpp
#define montecarlo_european_engine_hpp

#include <ql/pricingengines/vanilla/mcvanillaengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/termstructures/volatility/equityfx/blackvariancecurve.hpp>

#include "constantblackscholesprocess.hpp"
#include <iostream>

namespace QuantLib {

    //! European option pricing engine using Monte Carlo simulation
    /*! \ingroup vanillaengines

        \test the correctness of the returned value is tested by
              checking it against analytic results.
    */
    template <class RNG = PseudoRandom, class S = Statistics>
    class MCEuropeanEngine_2 : public MCVanillaEngine<SingleVariate, RNG, S> {
    public:
        typedef
            typename MCVanillaEngine<SingleVariate, RNG, S>::path_generator_type
            path_generator_type;
        typedef
            typename MCVanillaEngine<SingleVariate, RNG, S>::path_pricer_type
            path_pricer_type;
        typedef typename MCVanillaEngine<SingleVariate, RNG, S>::stats_type
            stats_type;
        // constructor
        MCEuropeanEngine_2(
            const boost::shared_ptr<GeneralizedBlackScholesProcess>& process,
            Size timeSteps,
            Size timeStepsPerYear,
            bool brownianBridge,
            bool antitheticVariate,
            Size requiredSamples,
            Real requiredTolerance,
            Size maxSamples,
            BigNatural seed,
            bool withConstantParameters);

    private:
        bool withConstantParameters;

        // Introduction of the path generator
                //Get the RNG
            ext::shared_ptr<path_generator_type> pathGenerator() const override {
            Size dimensions = MCVanillaEngine<SingleVariate, RNG, S>::process_->factors();
            TimeGrid grid = this->timeGrid();
                
            typename RNG::rsg_type generator =
                RNG::make_sequence_generator(dimensions * (grid.size() - 1), MCVanillaEngine<SingleVariate, RNG, S>::seed_);
         
                
                //Test of the boolean "WithConstantParameters" parameters
            if (this ->withConstantParameters)
            {
             
                
                //Get the parameters from the generalizedBSProcess class
                ext::shared_ptr<GeneralizedBlackScholesProcess> BS_process =
                ext::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(this->process_);
                Time time_of_extraction = grid.back();
                
                //Get the process
                double strike = ext::dynamic_pointer_cast<StrikedTypePayoff>(MCVanillaEngine<SingleVariate, RNG, S>::arguments_.payoff)->strike();
                
                double volatility_ = BS_process->blackVolatility()->blackVol(time_of_extraction, strike);
                
                double riskFreeRate_ = BS_process->riskFreeRate()->zeroRate(time_of_extraction, Continuous);
                
                double dividend_ = BS_process->dividendYield()->zeroRate(time_of_extraction, Continuous);
                
                double underlyingValue_ = BS_process->x0();
                
                // Instanciate a constant Black and Scholes Process with the extracted parameters
                ext::shared_ptr<ConstantBlackScholesProcess> Cst_BS_process(new ConstantBlackScholesProcess(underlyingValue_, riskFreeRate_, volatility_, dividend_));
                
                // Return a new path generator with constant Black and Scholes Process using the parameters
                return ext::shared_ptr<path_generator_type>(
                    new path_generator_type(Cst_BS_process, grid,
                      generator, MCVanillaEngine<SingleVariate, RNG, S>::brownianBridge_));
                
            }
            else
            {     // return the classical path generator
                return ext::shared_ptr<path_generator_type>(
                    new path_generator_type(MCVanillaEngine<SingleVariate, RNG, S>::process_, grid,
                        generator, MCVanillaEngine<SingleVariate, RNG, S>::brownianBridge_));
                
            
            }
        }


    protected:
        boost::shared_ptr<path_pricer_type> pathPricer() const override; //override the path pricer
    };

    //! Monte Carlo European engine factory
    template <class RNG = PseudoRandom, class S = Statistics>
    class MakeMCEuropeanEngine_2 {
    public:
        MakeMCEuropeanEngine_2(
            const boost::shared_ptr<GeneralizedBlackScholesProcess>&);
        // named parameters
        MakeMCEuropeanEngine_2& withSteps(Size steps);
        MakeMCEuropeanEngine_2& withStepsPerYear(Size steps);
        MakeMCEuropeanEngine_2& withBrownianBridge(bool b = true);
        MakeMCEuropeanEngine_2& withSamples(Size samples);
        MakeMCEuropeanEngine_2& withAbsoluteTolerance(Real tolerance);
        MakeMCEuropeanEngine_2& withMaxSamples(Size samples);
        MakeMCEuropeanEngine_2& withSeed(BigNatural seed);
        MakeMCEuropeanEngine_2& withAntitheticVariate(bool b = true);
        MakeMCEuropeanEngine_2& withBSPConstantParameters(bool withConstantParameters); //parameter constantBS
        // conversion to pricing engine
        operator boost::shared_ptr<PricingEngine>() const;
    private:
        boost::shared_ptr<GeneralizedBlackScholesProcess> process_;
        bool antithetic_;
        Size steps_, stepsPerYear_, samples_, maxSamples_;
        Real tolerance_;
        bool brownianBridge_;
        BigNatural seed_;
        bool withConstantParameters_; //parameter
    };

    class EuropeanPathPricer_2 : public PathPricer<Path> {
    public:
        EuropeanPathPricer_2(Option::Type type,
            Real strike,
            DiscountFactor discount);
        Real operator()(const Path& path) const;
    private:
        PlainVanillaPayoff payoff_;
        DiscountFactor discount_;
    };


    // inline definitions

    template <class RNG, class S>
    inline
        MCEuropeanEngine_2<RNG, S>::MCEuropeanEngine_2(
            const boost::shared_ptr<GeneralizedBlackScholesProcess>& process,
            Size timeSteps,
            Size timeStepsPerYear,
            bool brownianBridge,
            bool antitheticVariate,
            Size requiredSamples,
            Real requiredTolerance,
            Size maxSamples,
            BigNatural seed,
            bool withConstantParameters)

        : MCVanillaEngine<SingleVariate, RNG, S>(process,
            timeSteps,
            timeStepsPerYear,
            brownianBridge,
            antitheticVariate,
            false,
            requiredSamples,
            requiredTolerance,
            maxSamples,
            seed) {
        this -> withConstantParameters = withConstantParameters;
    }


    template <class RNG, class S>
    inline
        boost::shared_ptr<typename MCEuropeanEngine_2<RNG, S>::path_pricer_type>
        MCEuropeanEngine_2<RNG, S>::pathPricer() const {

        boost::shared_ptr<PlainVanillaPayoff> payoff =
            boost::dynamic_pointer_cast<PlainVanillaPayoff>(
                this->arguments_.payoff);
        QL_REQUIRE(payoff, "non-plain payoff given");

        boost::shared_ptr<GeneralizedBlackScholesProcess> process =
            boost::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                this->process_);
        QL_REQUIRE(process, "Black-Scholes process required");

        return boost::shared_ptr<
            typename MCEuropeanEngine_2<RNG, S>::path_pricer_type>(
                new EuropeanPathPricer_2(
                    payoff->optionType(),
                    payoff->strike(),
                    process->riskFreeRate()->discount(this->timeGrid().back())));
    }


    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>::MakeMCEuropeanEngine_2(
        const boost::shared_ptr<GeneralizedBlackScholesProcess>& process)
        : process_(process), antithetic_(false),
        steps_(Null<Size>()), stepsPerYear_(Null<Size>()),
        samples_(Null<Size>()), maxSamples_(Null<Size>()),
        tolerance_(Null<Real>()), brownianBridge_(false), seed_(0), withConstantParameters_(false) {}

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withBSPConstantParameters(bool withConstantParameters) {
        withConstantParameters_ = withConstantParameters;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withSteps(Size steps) {
        steps_ = steps;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withStepsPerYear(Size steps) {
        stepsPerYear_ = steps;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withSamples(Size samples) {
        QL_REQUIRE(tolerance_ == Null<Real>(),
            "tolerance already set");
        samples_ = samples;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withAbsoluteTolerance(Real tolerance) {
        QL_REQUIRE(samples_ == Null<Size>(),
            "number of samples already set");
        QL_REQUIRE(RNG::allowsErrorEstimate,
            "chosen random generator policy "
            "does not allow an error estimate");
        tolerance_ = tolerance;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withMaxSamples(Size samples) {
        maxSamples_ = samples;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withSeed(BigNatural seed) {
        seed_ = seed;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withBrownianBridge(bool brownianBridge) {
        brownianBridge_ = brownianBridge;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCEuropeanEngine_2<RNG, S>&
        MakeMCEuropeanEngine_2<RNG, S>::withAntitheticVariate(bool b) {
        antithetic_ = b;
        return *this;
    }

    template <class RNG, class S>
    inline
        MakeMCEuropeanEngine_2<RNG, S>::operator boost::shared_ptr<PricingEngine>()
        const {
        QL_REQUIRE(steps_ != Null<Size>() || stepsPerYear_ != Null<Size>(),
            "number of steps not given");
        QL_REQUIRE(steps_ == Null<Size>() || stepsPerYear_ == Null<Size>(),
            "number of steps overspecified");
        return boost::shared_ptr<PricingEngine>(new
            MCEuropeanEngine_2<RNG, S>(process_,
                steps_,
                stepsPerYear_,
                brownianBridge_,
                antithetic_,
                samples_, tolerance_,
                maxSamples_,
                seed_,
                withConstantParameters_)); 
    }



    inline EuropeanPathPricer_2::EuropeanPathPricer_2(Option::Type type,
        Real strike,
        DiscountFactor discount)
        : payoff_(type, strike), discount_(discount) {
        QL_REQUIRE(strike >= 0.0,
            "strike less than zero not allowed");
    }

    inline Real EuropeanPathPricer_2::operator()(const Path& path) const {
        QL_REQUIRE(path.length() > 0, "the path cannot be empty");
        return payoff_(path.back()) * discount_;
    }

}


#endif
