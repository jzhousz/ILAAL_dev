//
//  ConnectOrNot.cpp
//  
//
//  Created by Randall Suvanto on 2/13/17.
//

#include "ConnectOrNot.h"
using namespace std;


//conntectOrNot
//Inputs :: Neuron SWC node1, node2 are the two nodes that define the ROI - Starting and ending points of the neuron segment
//          unsigned char*  pointer to the original image array to extract the ROI
//          long dimensions array - The actual dimentions of the image
bool connectOrNot(NeuronSWC node1, NeuronSWC node2, unsigned char* image1d, long dimensions[]){
    
    //First extract the two nodes.
    
    ROISegment thisSegment;
    
    thisSegment.originalx1 = node1.x;
    thisSegment.originalx2 = node2.x;
    thisSegment.originaly1 = node1.y;
    thisSegment.originaly2 = node2.y;
    thisSegment.originalz1 = node1.z;
    thisSegment.originalz2 = node2.z;
    
    thisSegment.x_start = 0;
    thisSegment.y_start = 0;
    thisSegment.z_start = 0;
    
    thisSegment.x_end = 0;
    thisSegment.y_end = 0;
    thisSegment.z_end = 0;
    
    //Get info from the image
    //set the ImageData
    
    int xDim = dimensions[0];
    int yDim = dimensions[1];
    int zDim = dimensions[2];
    int cDim = dimensions[3];
    
    //Next determine if 1D, 2D, or 3D
    
    if((thisSegment.originalx1 == thisSegment.originalx2) && (thisSegment.originaly1 == thisSegment.originaly2) && (thisSegment.originalz1 == thisSegment.originalz2)){
        return true;
    }
    
    //1D Case
    else if((thisSegment.originalx1 == thisSegment.originalx2 && thisSegment.originaly1 == thisSegment.originaly2) || (thisSegment.originalx1 == thisSegment.originalx2 && thisSegment.originalz1 == thisSegment.originalz2) || (thisSegment.originaly1 == thisSegment.originaly2 && thisSegment.originalz1 == thisSegment.originalz2)){
        
        //First thing I will do is load the correct Classifier from disk. This is a .txt file.
        //STILL NEED TO THINK OF A WAY WHERE THE FILE NAME IS DYNAMIC, NOT JUST HARD CODED
        
        //cout << "ONE D CASE " << endl;
        std::stringstream str;
        
        str << "OneDClassifier.txt";
        
        
        //After the SVM Model is loaded, load the file with the dimension info
        
        fstream myfile("OneDClassifierInfo.txt", std::ios_base::in);
        
        int scaledX1D;
        
        myfile >> scaledX1D;
        
        const char * OneDClassifierName = str.str().c_str();
        SVMClassifier* OneDClassifier = new SVMClassifier();
        
        struct svm_model* pmodel1;
        
        pmodel1 = OneDClassifier->loadSVMModel(OneDClassifierName);
        
        //XDim changed
        char changingDim = '0';
        
        if(thisSegment.originaly1 == thisSegment.originaly2 && thisSegment.originalz1 == thisSegment.originalz2){
            thisSegment.x_start = min(thisSegment.originalx1, thisSegment.originalx2);
            thisSegment.x_end = ceil(max(thisSegment.originalx1, thisSegment.originalx2));
            changingDim = 'X';
        }
        
        //YDIM CHANGES
        else if(thisSegment.originalx1 == thisSegment.originalx2 && thisSegment.originalz1 == thisSegment.originalz2){
            thisSegment.x_start = min(thisSegment.originaly1, thisSegment.originaly2);
            thisSegment.x_end = ceil(max(thisSegment.originaly1, thisSegment.originaly2));
            changingDim = 'Y';
        }
        //ZDIM CHANGES
        else if(thisSegment.originalx1 == thisSegment.originalx2 && thisSegment.originaly1 == thisSegment.originaly2){
            thisSegment.x_start = min(thisSegment.originalz1, thisSegment.originalz2);
            thisSegment.x_end = ceil(max(thisSegment.originalz1, thisSegment.originalz2));
            changingDim = 'Z';
        }
        
        unsigned char newData[(int)thisSegment.x_end - (int)thisSegment.x_start +1];
        
        //Get the image data
        
        int pos = 0;
        if(changingDim == 'X'){
            for(int x = thisSegment.x_start; x <= thisSegment.x_end; x++){
                int offsetXYZ = x + xDim*thisSegment.originaly1 + xDim*yDim*thisSegment.originalz1;
                newData[pos] = image1d[offsetXYZ];
                pos++;
            }
            
        }
        else if(changingDim == 'Y'){
            for(int y = thisSegment.x_start; y <= thisSegment.x_end; y++){
                int offsetXYZ = thisSegment.originalx1 + xDim*y + xDim*yDim*thisSegment.originalz1;
                newData[pos] = image1d[offsetXYZ];
                pos++;
            }
        }
        
        else if(changingDim == 'Z'){
            for(int z = thisSegment.x_start; z <= thisSegment.x_end; z++){
                int offsetXYZ = thisSegment.originalx1 + xDim*thisSegment.originaly1 + xDim*yDim*z;
                newData[pos] = image1d[offsetXYZ];
                pos++;
            }
        }
        
        else{
            return 0;
        }
        
        
        //Have the data for the sub-image. Now resize so it will be able to be fed into the 1D classifier
        
        double ratiox = ((double)((int)thisSegment.x_end-(int)thisSegment.x_start +1))/((double)scaledX1D);
        int newXDim = thisSegment.x_end - thisSegment.x_start +1;
        int posCounter = 0;
        
        int newCount = 0;
        unsigned char resizedData2[scaledX1D];
        
        
        
        for(int i = 0; i <= scaledX1D-1; i++){
            int i2low = floor(i*ratiox);
            int i2high = floor((i+1)*ratiox-1);
            if(i2high < i2low){
                i2high = i2low;
            }
            if(i2high > newXDim -1){
                i2high = newXDim -1;
            }
            int iw = i2high - i2low +1;
            double cubeVolume = iw;
            int s = 0;
            
            for(int i1 = i2low; i1<=i2high; i1++){
                int pos = i1;
                s += (unsigned int)newData[pos];
                //    cout << "s = " << s << " pos = " << pos << endl;
            }
            V3DLONG result = s/(cubeVolume);
            //   cout << "Cube Volume = " << cubeVolume << endl;
            //  cout << "result (Calc average)" << result << endl;
            resizedData2[posCounter] = (unsigned char) result;
            posCounter++;
        }
        
        float *ClassifyArray = new float [scaledX1D];
        
        
        
        for(int q = 0; q < scaledX1D; q++){
            ClassifyArray[q] = resizedData2[q];
        }
        
        
        int classifierResult = OneDClassifier->classifyASample(ClassifyArray, scaledX1D, pmodel1);
       // cout << endl << "The Result After classification :: " << classifierResult << endl << endl;
        
        if(classifierResult == 0){
            return false;
        }
        else{
            return true;
        }
        
    }
    
    else if((thisSegment.originalx1 == thisSegment.originalx2) || (thisSegment.originaly1 == thisSegment.originaly2) || (thisSegment.originalz1 == thisSegment.originalz2)){
        
        //For 2D we need to find what the classifier SHOULD return. If classifier returns any other value, return false.
        
        //First thing I will do is load the correct Classifier from disk. This is a .txt file.
        //STILL NEED TO THINK OF A WAY WHERE THE FILE NAME IS DYNAMIC, NOT JUST HARD CODED
        
        std::stringstream str;
        
     //   cout << "TWO D CASE " << endl;
        
        str << "TwoDClassifier.txt";
        const char * TwoDClassifierName = str.str().c_str();
        SVMClassifier* TwoDClassifier = new SVMClassifier();
        
        struct svm_model* pmodel2;
        
        pmodel2 = TwoDClassifier->loadSVMModel(TwoDClassifierName);
        
        //After the SVM Model is loaded, load the file with the dimension info
        
        fstream myfile("TwoDClassifierInfo.txt", std::ios_base::in);
        
        int scaledX2D, scaledY2D;
        
        myfile >> scaledX2D >> scaledY2D;
        
        int returnValue = prePredictedValue(thisSegment);
        
        char constDim = '0';
        thisSegment.z_start =0;
        thisSegment.z_end = 0;
        
        if(thisSegment.originalx1 == thisSegment.originalx2){
            
            thisSegment.x_start = min(thisSegment.originaly1, thisSegment.originaly2);
            thisSegment.x_end = ceil(max(thisSegment.originaly1, thisSegment.originaly2));
            thisSegment.y_start = min(thisSegment.originalz1, thisSegment.originalz2);
            thisSegment.y_end = ceil(max(thisSegment.originalz1, thisSegment.originalz2));
            
            constDim = 'X';
            
        }
        else if(thisSegment.originaly1 == thisSegment.originaly2){
            
            thisSegment.x_start = min(thisSegment.originalx1, thisSegment.originalx2);
            thisSegment.x_end = ceil(max(thisSegment.originalx1, thisSegment.originalx2));
            thisSegment.y_start = min(thisSegment.originalz1, thisSegment.originalz2);
            thisSegment.y_end = ceil(max(thisSegment.originalz1, thisSegment.originalz2));
            
            constDim = 'Y';
            
        }
        
        else if(thisSegment.originalz1 == thisSegment.originalz2){
            
            thisSegment.x_start = min(thisSegment.originalx1, thisSegment.originalx2);
            thisSegment.x_end = ceil(max(thisSegment.originalx1, thisSegment.originalx2));
            thisSegment.y_start = min(thisSegment.originaly1, thisSegment.originaly2);
            thisSegment.y_end = ceil(max(thisSegment.originaly1, thisSegment.originaly2));
            
            constDim = 'Z';
        }
        
        //Extract the data from the original image in 2D
        
        unsigned char newData[((int)thisSegment.x_end - (int)thisSegment.x_start +1)*((int)thisSegment.y_end - (int)thisSegment.y_start + 1)];
        int pos = 0;
        if(constDim == 'X'){
            
            for(int z = thisSegment.y_start; z <= thisSegment.y_end; z++){
                int offSetZ = xDim*yDim*z;
                for(int y = thisSegment.x_start; y <= thisSegment.x_end; y++){
                    int offSetXYZ = thisSegment.originalx1 + y*xDim + offSetZ;
                    newData[pos] = image1d[offSetXYZ];
                    pos++;
                }
            }
            
            
        }
        else if(constDim == 'Y'){
            for(int z = thisSegment.y_start; z <= thisSegment.y_end; z++){
                int offSetZ = xDim*yDim*z;
                for(int x = thisSegment.x_start; x <= thisSegment.x_end; x++){
                    int offSetXYZ = x + xDim*thisSegment.originaly1 + offSetZ;
                    newData[pos] = image1d[offSetXYZ];
                    pos++;
                }
            }
            
            
        }
        else if(constDim == 'Z'){
            for(int y = thisSegment.y_start; y <= thisSegment.y_end; y++){
                int offSetY = xDim*y;
                for(int x = thisSegment.x_start; x <= thisSegment.x_end; x++){
                    int offSetXYZ = x + offSetY + xDim*yDim*thisSegment.originalz1;
                    newData[pos] = image1d[offSetXYZ];
                    pos++;
                }
            }
            
        }
        
        double ratiox = ((double)((int)thisSegment.x_end-(int)thisSegment.x_start+1))/((double)scaledX2D);
        double ratioy = ((double)((int)thisSegment.y_end-(int)thisSegment.y_start+1))/((double)scaledY2D);
        int newCount = 0;
        int newXDim = thisSegment.x_end - thisSegment.x_start +1;
        int newYDim = thisSegment.y_end - thisSegment.y_start +1;
        int posCounter = 0;
        unsigned char resizedData[scaledX2D*scaledY2D];
        
        for(int j = 0; j <= scaledY2D -1; j++){
            int j2low = floor(j*ratioy);
            int j2high = floor((j+1)*ratioy-1);
            if(j2high < j2low){
                j2high = j2low;
            }
            if(j2high > newYDim -1){
                j2high = newYDim -1;
            }
            int jw = j2high - j2low + 1;
            
            for(int i = 0; i <= scaledX2D -1; i++){
                int i2low = floor(i*ratiox);
                int i2high = floor((i+1)*ratiox -1);
                
                if(i2high < i2low){
                    i2high = i2low;
                }
                if(i2high > newXDim -1){
                    i2high = newXDim -1;
                }
                int iw = i2high - i2low +1;
                
                double cubeVolume = double(jw)*iw;
                int s = 0;
                
                for(int j1 = j2low; j1 <= j2high; j1++){
                    for(int i1 = i2low; i1 <= i2high; i1++){
                        int pos = j1*newXDim + i1;
                        s += (unsigned int)newData[pos];
                        //cout << "s = " << s << " pos = " << pos << endl;
                    }
                }
                V3DLONG result = s/cubeVolume;
                // cout << "Cube volume :: " << cubeVolume << endl;
                // cout << "Results :: " << result << endl;
                resizedData[posCounter] = (unsigned char) result;
                posCounter++;
            }
        }
        
        float *ClassifyArray = new float [scaledX2D*scaledY2D];
        
        for(int q = 0; q < scaledX2D*scaledY2D; q++){
            //   cout << (int)resizedData[q] << endl;
            ClassifyArray[q] = resizedData[q];
            // cout << ClassifyArray[q] << endl;
        }
        //cout << endl;
        
        int classifierResult = TwoDClassifier->classifyASample(ClassifyArray, scaledX2D*scaledY2D, pmodel2);
//        if (classifierResult == returnValue){
        if(classifierResult != 0){
            return true;
        }
        else{
            return false;
        }
        
    }
    
    //three-d case
    
    else{
        
        //First thing I will do is load the correct Classifier from disk. This is a .txt file.
        //STILL NEED TO THINK OF A WAY WHERE THE FILE NAME IS DYNAMIC, NOT JUST HARD CODED
    
        std::stringstream str;
        
     //   cout << "Three D CASE " << endl;
        
        str << "ThreeDClassifier.txt";
        const char * ThreeDClassifierName = str.str().c_str();
        SVMClassifier* ThreeDClassifier = new SVMClassifier();
        
        struct svm_model* pmodel3;
        
        pmodel3 = ThreeDClassifier->loadSVMModel(ThreeDClassifierName);
        
        //After the SVM Model is loaded, load the file with the dimension info
        
        fstream myfile("ThreeDClassifierInfo.txt", std::ios_base::in);
        
        int scaledX3D, scaledY3D, scaledZ3D;
        
        myfile >> scaledX3D >> scaledY3D >> scaledZ3D;
        
        int returnValue = prePredictedValue(thisSegment);
        
        thisSegment.x_start = min(thisSegment.originalx1, thisSegment.originalx2);
        thisSegment.x_end = ceil(max(thisSegment.originalx1, thisSegment.originalx2));
        thisSegment.y_start = min(thisSegment.originaly1, thisSegment.originaly2);
        thisSegment.y_end = ceil(max(thisSegment.originaly1, thisSegment.originaly2));
        thisSegment.z_start = min(thisSegment.originalz1, thisSegment.originalz2);
        thisSegment.z_end = ceil(max(thisSegment.originalz1, thisSegment.originalz2));
        unsigned char newData[((int)thisSegment.x_end - (int)thisSegment.x_start +1)*((int)thisSegment.y_end - (int)thisSegment.y_start + 1)* ((int)thisSegment.z_end - (int)thisSegment.z_start + 1)];
        int pos = 0;
        for(int z = thisSegment.z_start; z <= thisSegment.z_end; z++){
            int offSetZ = xDim*yDim*z;
            for(int y = thisSegment.y_start; y <= thisSegment.y_end; y++){
                int offSetYZ = xDim*y + offSetZ;
                for(int x = thisSegment.x_start; x <= thisSegment.x_end; x++){
                    int offSetXYZ = x + offSetYZ;
                    newData[pos] = image1d[offSetXYZ];
                    pos++;
                }
            }
        }
        
        int newXDim = thisSegment.x_end-thisSegment.x_start + 1;
        int newYDim = thisSegment.y_end-thisSegment.y_start + 1;
        int newZDim = thisSegment.z_end-thisSegment.z_start + 1;
        int posCounter = 0;
        
        double ratiox = ((double)((int)thisSegment.x_end-(int)thisSegment.x_start +1))/((double) scaledX3D );
        double ratioy = ((double)((int)thisSegment.y_end-(int)thisSegment.y_start +1))/((double) scaledY3D);
        double ratioz = ((double)((int)thisSegment.z_end-(int)thisSegment.z_start +1))/((double) scaledZ3D);
        
        unsigned char resizedData2[scaledZ3D*scaledY3D*scaledX3D];
        for(int k = 0; k <= scaledZ3D-1; k++){
            
            int k2low = floor(k*ratioz);// + it->z_start;
            int k2high = floor((k+1)*ratioz-1);// + it->z_start;
            
            if(k2high < k2low){
                k2high = k2low;
            }
            
            if(k2high > newZDim - 1){
                k2high = newZDim - 1;
            }
            
            int kw = k2high - k2low +1;
            
            for(int j = 0; j <= scaledY3D-1; j++){
                
                int j2low = floor(j*ratioy);// + it->y_start;
                int j2high = floor((j+1)*ratioy-1);// + it->y_start;
                
                if(j2high < j2low){
                    j2high = j2low;
                }
                if(j2high > newYDim - 1){
                    j2high = newYDim -1 ;
                }
                
                int jw = j2high - j2low + 1;
                
                for(int i = 0; i <= scaledX3D-1; i++){
                    
                    int i2low = floor(i*ratiox);// + it->x_start;
                    int i2high = floor((i+1)*ratiox - 1);// + it->x_start;
                    
                    if(i2high < i2low){
                        i2high = i2low;
                    }
                    
                    if(i2high > newXDim - 1){
                        i2high = newXDim -1;
                    }
                    
                    int iw = i2high - i2low + 1;
                    
                    double cubeVolume = double(kw)*jw*iw;
                    int s = 0;
                    
                    for(int k1 = k2low; k1<= k2high; k1++){
                        for(int j1 = j2low; j1<=j2high; j1++){
                            for(int i1 = i2low; i1<=i2high; i1++){
                                int pos = k1*(newXDim*newYDim) + j1*newXDim+ i1;
                                s += (unsigned int)newData[pos];
                                //cout << "s = " << s << "  pos = " << pos << endl;
                            }
                        }
                    }
                    //V3DLONG adjustedS = s;// + (it->z_start*(xdim*ydim)+it->y_start*xdim+it->x_start);
                    //if(cubeVolume == 0) cubeVolume = 1;
                    V3DLONG result = s/(cubeVolume);
                    //cout << "cube volume :: " << cubeVolume << endl;
                    //cout << "result (Calc Average) :: " << result << endl;
                    //cout << "Counter check :: " << posCounter << endl;
                    resizedData2[posCounter] = (unsigned char) result;
                    posCounter++;
                    
                }
            }
        }
        
    //    cout << "The data is :: " << endl;
        
        float *ClassifyArray = new float [scaledX3D*scaledY3D*scaledZ3D];
        
        for(int q = 0; q < scaledX3D*scaledY3D*scaledZ3D; q++){
            // cout << (int)resizedData2[q] << endl;
            ClassifyArray[q] = resizedData2[q];
            // cout << ClassifyArray[q] << endl;
        }
        //cout << endl;
        
        int classifierResult = ThreeDClassifier->classifyASample(ClassifyArray, scaledX3D*scaledY3D*scaledZ3D, pmodel3);
//        if(classifierResult == returnValue){
        if(classifierResult !=0){
            return true;
        }
        else{
            return false;
        }
        
    }
    
    
}


int prePredictedValue(ROISegment thisSegment){
    
    //find what case it classifies as first
    
    //this is a 2D case
    
    //this neuron is original1
    //that neuron is original2
    if(thisSegment.originalx1 == thisSegment.originalx2 || thisSegment.originaly1 == thisSegment.originaly2 || thisSegment.originalz1 == thisSegment.originalz2){
        
        //Need to remap the coordinates so that the resutls are consistent
        
        if(thisSegment.originalx1 - thisSegment.originalx2 == 0){
            if(((thisSegment.originaly1 - thisSegment.originaly2 > 0) && (thisSegment.originalz1 - thisSegment.originalz2 > 0)) || ((thisSegment.originaly1 - thisSegment.originaly2 < 0) && (thisSegment.originalz1 - thisSegment.originalz2 < 0)))    //Case with increasing slope (from left to right)
            {
                return 1;
            }
            else{
                return 2;
            }
        }
        else if(thisSegment.originaly1 - thisSegment.originaly2 == 0){
            if(((thisSegment.originalx1 - thisSegment.originalx2 > 0) && (thisSegment.originalz1 - thisSegment.originalz2 > 0)) || ((thisSegment.originalx1 - thisSegment.originalx2 < 0) && (thisSegment.originalz1 - thisSegment.originalz2 < 0)))
            {
                return 1;
                
            }
            else{
                return 2;
            }
        }
        else{
            if(((thisSegment.originalx1 - thisSegment.originalx2 > 0) && (thisSegment.originaly1 - thisSegment.originaly2 > 0)) || ((thisSegment.originalx1 - thisSegment.originalx2 < 0) && (thisSegment.originaly1 - thisSegment.originaly2 < 0))){
                return 1;
            }
            else{
                return 2;
            }
        }
        
    }
    
    else{
        
        if((thisSegment.originalz1-thisSegment.originalz2 < 0 && thisSegment.originaly1-thisSegment.originaly2 > 0 && thisSegment.originalx1 - thisSegment.originalx2 < 0) || (thisSegment.originalz1-thisSegment.originalz2 > 0 && thisSegment.originaly1-thisSegment.originaly2 < 0 && thisSegment.originalx1 - thisSegment.originalx2 > 0)){
            return 1;
        }
        else if((thisSegment.originalz1-thisSegment.originalz2 < 0 && thisSegment.originaly1-thisSegment.originaly2 < 0 && thisSegment.originalx1 - thisSegment.originalx2 < 0) || (thisSegment.originalz1-thisSegment.originalz2 > 0 && thisSegment.originaly1-thisSegment.originaly2 > 0 && thisSegment.originalx1 - thisSegment.originalx2 > 0)){
            return 2;
        }
        
        else if((thisSegment.originalz1-thisSegment.originalz2 < 0 && thisSegment.originaly1-thisSegment.originaly2 < 0 && thisSegment.originalx1 - thisSegment.originalx2 > 0) || (thisSegment.originalz1-thisSegment.originalz2 > 0 && thisSegment.originaly1-thisSegment.originaly2 > 0 && thisSegment.originalx1 - thisSegment.originalx2 < 0)){
            return 3;
        }
        else{
            return 4;
        }
        
    }
    
}


