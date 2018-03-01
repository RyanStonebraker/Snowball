//
//  childEvolver.hpp
//  snowball
//
//  Created by Thatcher Lane on 2/28/18.
//  Copyright © 2018 Thatcher Lane. All rights reserved.
//

#ifndef childEvolver_h
#define childEvolver_h

#include "WeightedNode.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
class childEvolver{
private:

public:
    childEvolver();
    float kingWeightPrime(WeightedNode &);
    float sigmaWeightPrime(WeightedNode &, int numberOfWeights);
    float nodeWeightPrime(WeightedNode &);

};
#endif /* childEvolver_h */
