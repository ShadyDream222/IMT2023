
QuantLib Project Report 

Presentation :

The project presented is an exercise in the use of the QuantLib library, particularly in the context of MonteCarlo engines.

Monte Carlo machines use stochastic simulation techniques to generate a large number of random scenarios that represent potential outcomes for an investment or investment portfolio. These scenarios are then used to calculate descriptive statistics such as volatility, value at risk and expected gain. Simulation results can be used to assess past and future performance of an investment or investment portfolio, as well as to help make informed investment decisions based on expected risks and returns.

However, In Monte Carlo engines, repeated calls to the process methods may cause a performance hit; especially when the process is an instance of the `GeneralizedBlackScholesProcess` class, whose methods in turn make expensive method calls to the contained term structures. The performance of the engine can be increased at the expense of some accuracy.

Here the main goal is to  create a new class that models a Black-Scholes process with constant parameters (underlying value, risk-free rate, dividend yield, and volatility); then modify the Monte Carlo engines copied in this repository so that they still take a generic Black-Scholes process and an additional boolean parameter.  If the boolean is `false`, the engine runs as usual; if it is `true`, the engine extracts the constant parameters from the original process (based on the exercise date of the option; for instance, the constant risk-free rate should be the zero-rate of the full risk-free curve at the exercise date) and runs the Monte Carlo simulation with an instance of the constant process.



Classe Constant Black & Scholes Process :


In Constant Black & Scholes process hpp file,  this code defines a class named "ConstantBlackScholesProcess" which is derived from the "StochasticProcess1D" class. This class represents a one-dimensional diffusion process that follows the Black-Scholes model with constant parameters.


The class has four public methods: 
the ConstantBlackScholesProcess constructor, the x0 method, the drift method, the diffusion method, and the apply method.

The constructor takes as parameters the initial values of the underlying asset, the risk-free interest rate, the volatility and the dividend, which are stored as private members of the class.

The x0 method returns the initial value of the underlying.

The drift method takes as parameters the time and value of the underlying asset at that time, and returns the drift of the process at that time. Drift is calculated based on the class parameters and the Black-Scholes model formula.

The diffusion method takes as parameters the time and the value of the underlying asset at that moment, and returns the diffusion of the process at that moment. Diffusion is also calculated based on the class parameters and the Black-Scholes model formula.

Finally, the apply method takes as parameters the initial value of the underlying asset and a small change in this value (dx), and returns the new value of the underlying asset after applying the small change. This method uses the previously calculated drift and diffusion to estimate the new price of the underlying asset.



Explanation in the case of European Actions :

The first change was to add the “withConstantParameters” variable to constructors and methods.
Then we added a particular “path generator” function to implement a Black Scholes Process with constant parameters if this variable is True.

The path generator method corresponds to the function that created the path from the process linked to the option (MCVanillaEngine example for European options). It is therefore used to model the evolution of the underlying.


Code explanation : 



![Pathgenerator](/Photo report/photo1.png)




If the constant is True then:

The following parameters of the process are retrieved: volatility, risk-free rate, dividend and value of the underlying.
Then we create a process instance ConstantBlackScholesProcess with these parameters and we return this instance.


![True](/Photo report/photo2.png)



If the constant is False, We go back to the usual process.



![False](/Photo report/photo3.png)


For Asian and Barrier options, the code is similar. The only change being the returned process type that corresponds to the class.


