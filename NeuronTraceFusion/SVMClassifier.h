/* Jie Zhou March 2010
 *
 */

#ifndef __SVMCLASSIFIER_H__
#define __SVMCLASSIFIER_H__

//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "DataClassifierInterface.h"
#include "svm.h"

class SVMClassifier : public DataClassifierInterface
{
private:      
   //some internal problem model for svm: (make it static to preserve???)
    struct svm_model * pmodel;
    void setParameters(char**argv, int argc, struct svm_parameter &param) ;
public:
    SVMClassifier();
    ~SVMClassifier(); 	
  
    void train(float* features, int *targets, int numberCubes, int numOfFeatures);  //implement the pure virtual one in interface
    int classifyASample(float*, int numOfFeatures);  //implement the pure virtual one in interface
    int classifyASample(float* feature, int numOfFeatures, struct svm_model *pmodel);
    int classifyASampleWithProbability(float* feature, int numOfFeatures, struct svm_model *pmodel, double *probability);
    struct svm_model *  loadSVMModel();
};

#endif
