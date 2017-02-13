/* Training_Collection_plugin.h
 * Gather training data for nueron reconstruction
 * 2016-10-25 : by Randall W. Suvanto
 */
 
#ifndef __TRAINING_COLLECTION_PLUGIN_H__
#define __TRAINING_COLLECTION_PLUGIN_H__

#include <QtGui>
#include <v3d_interface.h>
#include <iostream>
#include <string>
#include "v3d_message.h"
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>
#include "my_surf_objs.h"
#include "basic_surf_objs.h"
#include "basic_4dimage.h"
#include "SVMClassifier.h"
#include <time.h>
#include <fstream>



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

class Training : public QObject, public V3DPluginInterface2_1
{
	Q_OBJECT
	Q_INTERFACES(V3DPluginInterface2_1);
    

public:
	float getPluginVersion() const {return 1.1f;}

	QStringList menulist() const;
	void domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent);

	QStringList funclist() const ;
    bool dofunc(const QString &func_name, const V3DPluginArgList &input, V3DPluginArgList &output, V3DPluginCallback2 &callback, QWidget *parent);
};


bool setUpTrainingData(vector<Image4DSimple*> OneDImageData, vector<Image4DSimple*> TwoDImageData, vector<Image4DSimple*> ThreeDImageData, int averagex1D, int averagex2D, int averagey2D,int xDim, int yDim, int zDim, int numeberOfSamples, int numberOfSamples1D, int numberofSamples2D, vector<ROISegment> ROIs1D, vector<ROISegment> ROIs2D, vector<ROISegment> ROIs3D, vector<Image4DSimple*> Negative1DImageData, vector<Image4DSimple*> Negative2DImageData, vector<Image4DSimple*> Negative3DImageData, int maxNumberOf2DSamples, int maxNumberOf3DSamples, V3DPluginCallback2 &callback, QWidget *parent, unsigned char OneDArrayData[], unsigned char negativeArrayData1D[], unsigned char TwoDArrayData[], unsigned char negativeArrayData2D[], unsigned char ThreeDArrayData[], unsigned char negativeArrayData3D[]);

bool classifySamples(SVMClassifier* OneDClassifier, SVMClassifier* TwoDClassifier, SVMClassifier* ThreeDClassifier, int OneDImageArraySize, int TwoDImageArraySize, int ThreeDImageArraySize, int TwoDXDim, int TwoDYDim, int ThreeDXDim, int ThreeDYDim, int ThreeDZDim, V3DPluginCallback2 &callback,  QWidget *parent);

int prePredictedValue(ROISegment thisSegment);

#endif

