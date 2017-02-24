//
//  Mean_Shift.hpp
//  
//
//  Created by Randall Suvanto on 2/23/17.
//
//

#ifndef Mean_Shift_h
#define Mean_Shift_h

#include <QtGui>
#include <v3d_interface.h>
#include <iostream.h>
#include <fstream>
#include "mean_shift_dialog.h"
#include <iostream>
#include <string>
#include <sstream>


LandmarkList meanShift(vector<NeuronTree> trees, Image4DSimple tImage);

LandmarkList mean_shift_center(unsigned char * image_data, LandmarkList LList);


#endif /* Mean_Shift_h */
