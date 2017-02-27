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


QList<NeuronSWC> meanShift(vector<NeuronTree> trees, unsigned char * image_data, V3DLONG in_sz[4]);

LandmarkList mean_shift_center(unsigned char * image_data, LandmarkList LList, V3DLONG in_sz[4]);

QList<NeuronSWC> ReMapMarkersToSWC(LandmarkList inputLList, vector<NeuronTree> inputTree);


#endif /* Mean_Shift_h */
