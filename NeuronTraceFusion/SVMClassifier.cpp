#include "SVMClassifier.h"
#include <iostream>

#include "svm.h"   //libsvm

//wrapper of libsvm toolbox

//LIBLINEAR (linearSVM) for training speed? http://www.csie.ntu.edu.tw/~cjlin/liblinear/

//Add make it quiet mode.

//What about scaling ???

//can be used to pass in arguments: parse options {"-t 0", "", ""}? -- start from 1.
SVMClassifier::SVMClassifier()
{

}

SVMClassifier::~SVMClassifier() 	
{ //	svm_destroy_model(pmodel); 
}
 

//build a model
void SVMClassifier::train(float* features, int *targets, int numberCubes, int numOfFeatures)
{
    struct svm_parameter param;
    setParameters(NULL, 0, param);

    struct svm_problem prob;
    struct svm_node *x_space;
	prob.l = numberCubes;
    prob.y = new double[prob.l];
	prob.x = new struct svm_node *[prob.l]; //init pointer to nodes, essentially a 2D array of svm_node
	x_space = new struct svm_node[prob.l*(numOfFeatures+1)]; //store all the x of all data samples.
    int i,j;
	for(i=0;i<prob.l; i++)
	{
            prob.y[i] = (double) targets[i];
	        prob.x[i] = x_space+i*(numOfFeatures+1); //current x_space's starting address.
            for(j=0; j<numOfFeatures; j++)
            {
              prob.x[i][j].index = j+1;
              prob.x[i][j].value= (double )features[i*numOfFeatures+j];
            }
            prob.x[i][j].index = -1; //indicate  the end of current sample. Different from LibSVM Java!
    }
    pmodel = svm_train(&prob, &param);

    //try here:
    for (i=0;i<prob.l; i++)
    {     
      struct svm_node *x = x_space + i*(numOfFeatures+1);
      int predict_label = (int) svm_predict(pmodel,x);
      std::cout << "p:" << predict_label <<  "(" << targets[i] << ") ";      

    } 
    svm_save_model("test_svm_model.txt", pmodel);

    //clean up
	delete[] prob.y;
	delete[] prob.x;
	delete[] x_space;
     
    svm_destroy_param(&param);
}

struct svm_model *  SVMClassifier::loadSVMModel()
{
       pmodel= svm_load_model("test_svm_model.txt");
       return pmodel;      
}

//not used since needs to pass in pmodel for SVM.
int SVMClassifier::classifyASample(float* feature, int numOfFeatures) //; (svm_model *pmodel)
{ return NULL; }

//use the model  (for classification, assume no probability estimation)
//int SVMClassifier::classifyASample(float* feature, int numOfFeatures)
int SVMClassifier::classifyASample(float* feature, int numOfFeatures, struct svm_model *pmodel)
{
   	//int svm_type=svm_get_svm_type(pmodel);
    int predict_label;
    struct svm_node *x;

    x =  new struct svm_node [numOfFeatures+1];
    int i;
    for(i=0; i< numOfFeatures; i++)
    {
			x[i].index = i+1;
			x[i].value = (double) feature[i];
    }
    x[i].index = -1;  //set last index as -1.

    //why in memory copy does not work??? will test later. wrong type?
    //pmodel= svm_load_model("test_svm_model.txt");
    predict_label = (int) svm_predict(pmodel,x);

    delete[] x;
    //svm_destroy_model(pmodel);
    return predict_label;
}

int SVMClassifier::classifyASampleWithProbability(float* feature, int numOfFeatures, struct svm_model *pmodel, double *probability)
{
   	//int svm_type=svm_get_svm_type(pmodel);
    int predict_label;
    struct svm_node *x;

    x =  new struct svm_node [numOfFeatures+1];
    int i;
    for(i=0; i< numOfFeatures; i++)
    {
			x[i].index = i+1;
			x[i].value = (double) feature[i];
    }
    x[i].index = -1;  //set last index as -1.

    //probability return the probability for each class
    predict_label = (int) svm_predict_probability(pmodel,x, probability);

    delete[] x;
    //svm_destroy_model(pmodel);
    return predict_label;
}



void SVMClassifier::setParameters(char**argv, int argc, struct svm_parameter &param)
{
   	// default values
	param.svm_type = C_SVC;
	param.kernel_type = LINEAR; // { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED }; /* kernel_type */
	param.degree = 3;
	param.gamma = 0;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 1; //in order to get confidence score
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

    // parse options { "-t 0", "", ""}? -- start from 1.
    for(int i=0;i<argc;i++)
	//for(i=1;i<argc;i++)
	{
		if(argv[i][0] != '-') break;
		if(++i>=argc)
		{
			return; //exit_with_help();
        }
		switch(argv[i-1][1])
		{
			case 's':
				param.svm_type = atoi(argv[i]);
				break;
			case 't':
				param.kernel_type = atoi(argv[i]);
				break;
			case 'd':
				param.degree = atoi(argv[i]);
				break;
			case 'g':
				param.gamma = atof(argv[i]);
				break;
			case 'r':
				param.coef0 = atof(argv[i]);
				break;
			case 'n':
				param.nu = atof(argv[i]);
				break;
			case 'm':
				param.cache_size = atof(argv[i]);
				break;
			case 'c':
				param.C = atof(argv[i]);
				break;
			case 'e':
				param.eps = atof(argv[i]);
				break;
			case 'p':
				param.p = atof(argv[i]);
				break;
			case 'h':
				param.shrinking = atoi(argv[i]);
				break;
			case 'b':
				param.probability = atoi(argv[i]);
				break;
			/*
            case 'q':
				svm_print_string = &print_null;
				i--;
				break;
			case 'v':
				cross_validation = 1;
				nr_fold = atoi(argv[i]);
				if(nr_fold < 2)
				{
					fprintf(stderr,"n-fold cross validation: n must >= 2\n");
					//exit_with_help();
				}
				break; */
			case 'w':
				++param.nr_weight;
				param.weight_label = (int *)realloc(param.weight_label,sizeof(int)*param.nr_weight);
				param.weight = (double *)realloc(param.weight,sizeof(double)*param.nr_weight);
				param.weight_label[param.nr_weight-1] = atoi(&argv[i-1][2]);
				param.weight[param.nr_weight-1] = atof(argv[i]);
				break;
			default:
				fprintf(stderr,"Unknown option: -%c\n", argv[i-1][1]);
		}
    }
}

