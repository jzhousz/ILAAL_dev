/* Jie Zhou March 2010
 *
 */

#ifndef __DATACLASSINTERFACE_H__
#define __DATACLASSINTERFACE_H__



class DataClassifierInterface
{
public:
   	virtual ~DataClassifierInterface() {}
    virtual void train(float* features, int *targets, int numberCubes, int numOfFeatures) = 0;  
    virtual int classifyASample(float*, int numOfFeatures) = 0;  
    //may add another one that classifies a lot of samples together.
};

#endif
