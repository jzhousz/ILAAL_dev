//
//  ConnectOrNot.hpp
//  
//
//  Created by Randall Suvanto on 2/13/17.
//
//

#ifndef ConnectOrNot_hpp
#define ConnectOrNot_hpp

#include <stdio.h>
#include <sstream>
#include "SVMClassifier.h"
#include <cmath>
#include "basic_surf_objs.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>


struct ROISegment
{
    int x_start;
    int x_end;
    int y_start;
    int y_end;
    int z_start;
    int z_end;
    
    int originalx1;
    int originalx2;
    int originaly1;
    int originaly2;
    int originalz1;
    int originalz2;
};

bool connectOrNot(NeuronSWC node1, NeuronSWC node2, unsigned char* image1d, long dimensions[]);

int prePredictedValue(ROISegment thisSegment);


#endif /* ConnectOrNot_hpp */
