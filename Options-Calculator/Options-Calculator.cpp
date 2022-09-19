#include <iostream>
#include <utility>
#include <cmath>

double HALF_ROOT = sqrt(0.5);
double APPROX_TOLERANCE = 0.0001;
int MAX_ITERATIONS = 1000;

// Calculate d1 value in Black-Scholes formula
double calcD1(double vol, double stockPri, double strikePri, double rf, double timeToExp)
{
    if (vol <= 0 or timeToExp <= 0 or strikePri <= 0)
    {
        throw std::invalid_argument("Received invalid volume, expiry time, or strike price");
    }

    return (1 / (vol * sqrt(timeToExp))) * (log((stockPri / strikePri)) + (rf + ((vol * vol) / 2)) * timeToExp);
}

// Calculate d2 value in Black-Scholes formula
double calcD2(double d1, double vol, double timeToExp)
{
    if (vol <= 0 or timeToExp <= 0)
    {
        throw std::invalid_argument("Received invalid volume or expiry time");
    }

    return d1 - (vol * sqrt(timeToExp));
}

// Calculate CDF of normal function
double cdfNorm(double val)
{
    return 0.5 * erfc(-val * HALF_ROOT);
}

// Calculates Black Scholes Call price
double blackScholesCall(double vol, double stockPri, double strikePri, double rf, double timeToExp, double d1, double d2)
{
    return cdfNorm(d1) * stockPri - cdfNorm(d2) * strikePri * exp(-rf * timeToExp);
}

// Calculates Black Scholes Put price
double blackScholesPut(double vol, double stockPri, double strikePri, double rf, double timeToExp, double d1, double d2)
{
    return -cdfNorm(-d1) * stockPri + cdfNorm(-d2) * strikePri * exp(-rf * timeToExp);
}

// Calculates derivative of Call/Put price with respect to volatility
double calcVega(double stockPri, double d1, double timeToExp)
{
    return stockPri * cdfNorm(d1) * sqrt(timeToExp);
}

// Approximates implied volatility by using Newton-Raphson root finding of Call Price -
double calcImpliedVolatility(bool isCallOption, double callPrice, double stockPri, double strikePri, double rf, double timeToExp)
{
    double currDiff = 1;

    int iterCount = 0;
    double closestVol = 1;

    auto blackScholesFunction = blackScholesPut;

    if (isCallOption)
    {
        blackScholesFunction = blackScholesCall;
    }

    while (currDiff > APPROX_TOLERANCE && iterCount < MAX_ITERATIONS)
    {
        iterCount += 1;

        double d1 = calcD1(closestVol, stockPri, strikePri, rf, timeToExp);
        double d2 = calcD2(d1, closestVol, timeToExp);

        double bschFuncVal = blackScholesFunction(closestVol, stockPri, strikePri, rf, timeToExp, d1, d2) - callPrice;
        double derivVal = calcVega(stockPri, d1, timeToExp);

        // Error Checking to prevent divide by zero error when calculating new vol guess
        if (derivVal == 0)
        {
            return -1;
        }

        double newVol = closestVol - (bschFuncVal / derivVal);
        currDiff = fabs((newVol - closestVol) / closestVol);

        closestVol = newVol;
    }

    return closestVol;
}

extern "C"
{
    double calcD1_py(double vol, double stockPri, double strikePri, double rf, double timeToExp) { return calcD1(vol, stockPri, strikePri, rf, timeToExp); }
    double calcD2_py(double d1, double vol, double timeToExp) { return calcD2(d1, vol, timeToExp); }
    double cdfNorm_py(double val) { return cdfNorm(val); }
    double blackScholesCall_py(double vol, double stockPri, double strikePri, double rf, double timeToExp, double d1, double d2) { return blackScholesCall(vol, stockPri, strikePri, rf, timeToExp, d1, d2); }
    double blackScholesPut_py(double vol, double stockPri, double strikePri, double rf, double timeToExp, double d1, double d2) { return blackScholesPut(vol, stockPri, strikePri, rf, timeToExp, d1, d2); }
    double calcVega_py(double stockPri, double d1, double timeToExp) { return calcVega(stockPri, d1, timeToExp); }
    double calcImpliedVolatility_py(bool isCallOption, double callPrice, double stockPri, double strikePri, double rf, double timeToExp) { return calcImpliedVolatility(isCallOption, callPrice, stockPri, strikePri, rf, timeToExp); }
}