 /* Training_Collection_plugin.cpp
 * Gather training data for nueron reconstruction
 * 2016-10-25 : by Randall W. Suvanto
 */
 
#include "v3d_message.h"
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>
#include "my_surf_objs.h"
#include "Training_Collection_plugin.h"
#include "basic_surf_objs.h"
#include "basic_4dimage.h"
#include "SVMClassifier.h"
#include <time.h>
#include <fstream>


//void SVMClassifier::train(float*, int*, int, int);
using namespace std;
//NeuronTree readSWC_file(const QString& filename);

Q_EXPORT_PLUGIN2(Training_Collection, Training);
int startPlugin(V3DPluginCallback2 &callback, QWidget *parent);


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

bool setUpTrainingData(vector<Image4DSimple*> OneDImageData, vector<Image4DSimple*> TwoDImageData, vector<Image4DSimple*> ThreeDImageData, int averagex1D, int averagex2D, int averagey2D,int xDim, int yDim, int zDim, int numeberOfSamples, int numberOfSamples1D, int numberofSamples2D, vector<ROISegment> ROIs1D, vector<ROISegment> ROIs2D, vector<ROISegment> ROIs3D, vector<Image4DSimple*> Negative1DImageData, vector<Image4DSimple*> Negative2DImageData, vector<Image4DSimple*> Negative3DImageData, int maxNumberOf2DSamples, int maxNumberOf3DSamples, V3DPluginCallback2 &callback, QWidget *parent);

QStringList Training::menulist() const
{
	return QStringList() 
		<<tr("Gather Training_Data")
		<<tr("about this plugin");
}

QStringList Training::funclist() const
{
	return QStringList()
		<<tr("func1")
		<<tr("func2")
		<<tr("help");
}

void Training::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
	if (menu_name == tr("Gather Training_Data"))
	{
        startPlugin(callback, parent);
	}
	else
	{
		QMessageBox::information(parent, "Version Info", "Gather Training Data for 3D neuron reconstruction");
	}
}

bool Training::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
	vector<char*> infiles, inparas, outfiles;
	if(input.size() >= 1) infiles = *((vector<char*> *)input.at(0).p);
	if(input.size() >= 2) inparas = *((vector<char*> *)input.at(1).p);
	if(output.size() >= 1) outfiles = *((vector<char*> *)output.at(0).p);

	if (func_name == tr("func1"))
	{
		v3d_msg("To be implemented.");
	}
	else if (func_name == tr("func2"))
	{
		v3d_msg("To be implemented.");
	}
	else if (func_name == tr("help"))
	{
		v3d_msg("To be implemented.");
	}
	else return false;

	return true;
}

int startPlugin(V3DPluginCallback2 &callback, QWidget *parent)
{
    
    cout << "starting plugin.." << endl;
    
    bool ok1 = false;
    bool ok2;
    QStringList items;
    QVector<QString> markedImageNames(1);
    QString markedImageName;
    v3dhandle win = NULL;
    v3dhandleList winList = callback.getImageWindowList();
    if(winList.size() == 0)
    {
        QMessageBox::information(0, "", "You don't have any image open in the main window.");
        return -1;
    }
    for(int winIndex =0;  winIndex < winList.size(); winIndex ++)
    {
        items << QObject::tr(callback.getImageName(winList.at(winIndex)).toAscii());
    }
    
    int number = 0;
    //a loop to get multiple images.
    while(!ok1)
    {
        markedImageName = QInputDialog::getItem(parent, QObject::tr("Pick an image to get training data from"),
                                                QObject::tr("Select the image to use: "), items, 0, false, &ok1);
        if(ok1)
        {
            markedImageNames.append(markedImageName);


        }
        else
        {
            if (number ==0)
            { //didn't choose anything
                cout << "Did not choose anything....quittting plugin.." << endl;
                return -1;
            }
            else
            {
                break;
            }
        }
        
    }
    
    //Next is to get the SWC File data from the image and store it in a SWC Node list data structure
    
    //virtual NeuronTree getSWC(v3dhandle image_window) = 0;
    QString inputFileName = QFileDialog::getOpenFileName(parent, QObject::tr("Choose the swc file to get training set from "), QDir::currentPath(),  QObject::tr("*.swc"));
    win = callback.currentImageWindow();
    cout << "Name of Image: " << callback.getImageName(win).toUtf8().constData() << endl;
    NeuronTree neuron1;
    neuron1 = readSWC_file(inputFileName);
    
    cout << endl << "Should have the tree now " << endl;
    cout << "The tree size: " << neuron1.listNeuron.size() << endl;
    cout << "SWC FILE NAME = " << inputFileName.toUtf8().constData() << endl;
    
    //After we have the swc file stored in a NeuronTree data structure, get segments -> if a node has a parent then find the parent and get the subimage based on the current node coodrinates and the Parent coordinates.
    vector<ROISegment> ROIs;
    vector<ROISegment> ROIs2D;
    vector<ROISegment> ROIs1D;
    int sumx = 0;
    int sumy = 0;
    int sumz = 0;
    
    int sumx2D = 0;
    int sumy2D = 0;
    
    int sumx1D = 0;
    
    int total3D = 0;
    int total2D = 0;
    int total1D = 0;
    cout << "Starting Sum :: " << sumy2D << endl;
    
    //Added January 17th, 2017
    // First scan the neuron segments so that we have an even distribution of segments later on
    
    int Sum2DCase1 = 0;
    int Sum2DCase2 = 0;
    int Sum3DCase1 = 0;
    int Sum3DCase2 = 0;
    int Sum3DCase3 = 0;
    int Sum3DCase4 = 0;
    
    int total0DConnections = 0;
    int total1DConnections = 0;
    int total2DConnections = 0;
    int total3DConnections = 0;
    
    for(int i = 0; i < neuron1.listNeuron.size(); i++){
        
        NeuronSWC thisNeuron = neuron1.listNeuron.at(i);
        V3DLONG NodeParent = thisNeuron.pn;
        if(NodeParent == -1)
        {
            continue;               //ThisNeuron = 1     ThatNeuron = 2
        }
        else{
            for(int j = 0; j < neuron1.listNeuron.size(); j++){
                V3DLONG thisID = neuron1.listNeuron.at(j).n;
                if(thisID == NodeParent){
                    ROISegment NewSegment;
                    NeuronSWC thatNeuron = neuron1.listNeuron.at(j);
                    
                    int nodeOneX = thisNeuron.x;
                    int nodeOneY = thisNeuron.y;
                    int nodeOneZ = thisNeuron.z;
                    
                    int nodeTwoX = thatNeuron.x;
                    int nodeTwoY = thatNeuron.y;
                    int nodeTwoZ = thatNeuron.z;
                    
                    if((nodeOneX == nodeTwoX) && (nodeOneY == nodeTwoY) && (nodeOneZ == nodeTwoZ)){
                        total0DConnections++;
                        continue;       //Case if Nodes are in the same position and connected (CAN HAPPEN)
                        
                    }
                     else if((nodeOneX == nodeTwoX && nodeOneY == nodeTwoY) || (nodeOneX == nodeTwoX && nodeOneZ == nodeTwoZ) || (nodeOneY == nodeTwoY && nodeOneZ == nodeTwoZ)){
                                    //1D case. Yes or no problem. Dont need to do anything... 1 positive case
                         total1DConnections++;
                         continue;
                         
                     }
                     else if((nodeOneX == nodeTwoX) || (nodeOneY == nodeTwoY) || (nodeOneZ == nodeTwoZ)){
                         total2DConnections++;
                         //2D case. 2 Different classes + negative
                         if(nodeOneX - nodeTwoX == 0){
                             if(((thisNeuron.y - thatNeuron.y > 0) && (thisNeuron.z - thatNeuron.z > 0)) || ((thisNeuron.y - thatNeuron.y < 0) && (thisNeuron.z - thatNeuron.z < 0)))    //Case with increasing slope (from left to right)
                             {
                                 Sum2DCase1++;
                             }
                             else{
                                 Sum2DCase2++;
                             }
                         }
                         else if(nodeOneY - nodeTwoY == 0){
                             if(((thisNeuron.x - thatNeuron.x > 0) && (thisNeuron.z - thatNeuron.z > 0)) || ((thisNeuron.x - thatNeuron.x < 0) && (thisNeuron.z - thatNeuron.z < 0)))
                             {
                                 Sum2DCase1++;
                                 
                             }
                             else{
                                 Sum2DCase2 ++;
                             }
                         }
                         else{
                             if(((thisNeuron.x - thatNeuron.x > 0) && (thisNeuron.y - thatNeuron.y > 0)) || ((thisNeuron.x - thatNeuron.x < 0) && (thisNeuron.y - thatNeuron.y < 0))){
                                 Sum2DCase1++;
                             }
                             else{
                                 Sum2DCase2++;
                             }
                         }
                     }
                     else{
                         total3DConnections++;
                         if((thisNeuron.z-thatNeuron.z < 0 && thisNeuron.y-thatNeuron.y > 0 && thisNeuron.x - thatNeuron.x < 0) || (thisNeuron.z-thatNeuron.z > 0 && thisNeuron.y-thatNeuron.y < 0 && thisNeuron.x - thatNeuron.x > 0)){
                             Sum3DCase1++;
                         }
                        else if((thisNeuron.z-thatNeuron.z < 0 && thisNeuron.y-thatNeuron.y < 0 && thisNeuron.x - thatNeuron.x < 0) || (thisNeuron.z-thatNeuron.z > 0 && thisNeuron.y-thatNeuron.y > 0 && thisNeuron.x - thatNeuron.x > 0)){
                            Sum3DCase2++;
                        }
                         
                        else if((thisNeuron.z-thatNeuron.z < 0 && thisNeuron.y-thatNeuron.y < 0 && thisNeuron.x - thatNeuron.x > 0) || (thisNeuron.z-thatNeuron.z > 0 && thisNeuron.y-thatNeuron.y > 0 && thisNeuron.x - thatNeuron.x < 0)){
                            Sum3DCase3++;
                        }
                        else{
                            Sum3DCase4++;
                        }
                     }
                
                }
            }
        }
    }
    
    int maxNumberOf2DSamples = min(Sum2DCase1, Sum2DCase2);
    int placeHolder1 = min(Sum3DCase1, Sum3DCase2);
    int placeHolder2 = min(placeHolder1, Sum3DCase3);
    int maxNumberOf3DSamples = min(placeHolder2, Sum3DCase4);
    
    cout << "The Mins are " << maxNumberOf2DSamples << ", " << maxNumberOf3DSamples << endl;
    //exit(0);
    
    cout << "Total In 0DCases  :: " << total0DConnections << endl << endl;
    cout << "Total In 1DCases  :: " << total1DConnections << endl << endl;
    cout << "Total In 2DCase 1 :: " <<Sum2DCase1 << endl << "Total In 2DCase 2 :: " << Sum2DCase2 << endl << "Total2D Cases :: " << total2DConnections << endl << endl;
        
    cout << "Total In 3DCase 1 :: " << Sum3DCase1 << endl << "Total In 3DCase 2:: " << Sum3DCase2 << endl << "Total In 3DCase 3 :: " << Sum3DCase3 << endl << "Total In 3DCase 4 :: " << Sum3DCase4 << endl << "Total 3DCases :: " << total3DConnections << endl;
        
        //End of section added January 17th, 2017
    
    int AmountOf2DCase1 = 0;
    int AmountOf2DCase2 = 0;
    int AmountOf3DCase1 = 0;
    int AmountOf3DCase2 = 0;
    int AmountOf3DCase3 = 0;
    int AmountOf3DCase4 = 0;
    
   // exit(0);
    
    for(int i = 0; i < neuron1.listNeuron.size(); i++)
    {
        //cout << neuron1.listNeuron.at(i).pn << endl;
        NeuronSWC thisNeuron = neuron1.listNeuron.at(i);
        V3DLONG NodeParent = thisNeuron.pn;
        //cout << "This node ID " << thisNeuron.n << endl;
        if(NodeParent == -1)
        {
            continue;
        }
        else
        {
            //check if 1D, 2D, or 3D and add to the appropriate vector
            for(int j = 0; j < neuron1.listNeuron.size(); j++){
                V3DLONG thisID = neuron1.listNeuron.at(j).n;
                
                
                
                if(thisID == NodeParent){
                    ROISegment NewSegment;
                    NeuronSWC thatNeuron = neuron1.listNeuron.at(j);
                    
                    
                    int nodeOneX = thisNeuron.x;
                    int nodeOneY = thisNeuron.y;
                    int nodeOneZ = thisNeuron.z;
                    
                    int nodeTwoX = thatNeuron.x;
                    int nodeTwoY = thatNeuron.y;
                    int nodeTwoZ = thatNeuron.z;
                    
                    //may have a case where the nodes are at the same point....skip this for now...
                    if((nodeOneX == nodeTwoX) && (nodeOneY == nodeTwoY) && (nodeOneZ == nodeTwoZ)){
                        
                        continue;
                        
                    }
                    
                    //1D case...only 1 dimension changes
                    else if((nodeOneX == nodeTwoX && nodeOneY == nodeTwoY) || (nodeOneX == nodeTwoX && nodeOneZ == nodeTwoZ) || (nodeOneY == nodeTwoY && nodeOneZ == nodeTwoZ)){
                        //only Z has a Dimension
                        cout << "1D Case :: " << thisNeuron.x << ", " << thisNeuron.y << ", " << thisNeuron.z << " to " << thatNeuron.x << ", " << thatNeuron.y << ", " << thatNeuron.z << endl;
                        cout << "Translated to :: " << nodeOneX << ", " << nodeOneY << ", " << nodeOneZ << " to " << nodeTwoX << ", " << nodeTwoY << ", " << nodeTwoZ << endl;
                        if(nodeOneX == nodeTwoX && nodeOneY == nodeTwoY){
                            NewSegment.x_start = min(thisNeuron.x, thatNeuron.x);
                            NewSegment.x_end = ceil(max(thisNeuron.x, thatNeuron.x));
                            sumx1D += (NewSegment.z_end - NewSegment.z_start) +1;
                        }
                        //only Y has a dimension
                        else if((nodeOneX == nodeTwoX && nodeOneZ == nodeTwoZ)){
                            NewSegment.x_start = min(thisNeuron.y, thatNeuron.y);
                            NewSegment.x_end = ceil(max(thisNeuron.y, thatNeuron.y));
                            sumx1D += (NewSegment.y_end - NewSegment.y_start) +1;
                        }
                        //only X has a dimension
                        else{
                            NewSegment.x_start = min(thisNeuron.x, thatNeuron.x);
                            NewSegment.x_end = ceil(max(thisNeuron.x, thatNeuron.x));
                            sumx1D += (NewSegment.x_end - NewSegment.x_start) +1;
                        }
                        NewSegment.originalx1 = thisNeuron.x;
                        NewSegment.originaly1 = thisNeuron.y;
                        NewSegment.originalz1 = thisNeuron.z;
                        NewSegment.originalx2 = thatNeuron.x;
                        NewSegment.originaly2 = thatNeuron.y;
                        NewSegment.originalz2 = thatNeuron.z;
                        
                        NewSegment.y_start = 0; //Set to 0 since 1D
                        NewSegment.y_end = 0;
                        NewSegment.z_start = 0;
                        NewSegment.z_end = 0;
                        
                        total1D++;
                        ROIs1D.push_back(NewSegment);
            
                    }
                    
                    //2D case
                    else if((nodeOneX == nodeTwoX) || (nodeOneY == nodeTwoY) || (nodeOneZ == nodeTwoZ)){
                        NewSegment.originalx1 = thisNeuron.x;
                        NewSegment.originaly1 = thisNeuron.y;
                        NewSegment.originalz1 = thisNeuron.z;
                        NewSegment.originalx2 = thatNeuron.x;
                        NewSegment.originaly2 = thatNeuron.y;
                        NewSegment.originalz2 = thatNeuron.z;
                        bool shouldAdd = true;
                        cout << "2D Case :: " << thisNeuron.x << ", " << thisNeuron.y << ", " << thisNeuron.z << " to " << thatNeuron.x << ", " << thatNeuron.y << ", " << thatNeuron.z << endl;
                        cout << "Translated to :: " << nodeOneX << ", " << nodeOneY << ", " << nodeOneZ << " to " << nodeTwoX << ", " << nodeTwoY << ", " << nodeTwoZ << endl;
                        //Y and Z have a Dimension
                        if(nodeOneX == nodeTwoX){
                            
                            if(((NewSegment.originaly1 - NewSegment.originaly2 > 0) && (NewSegment.originalz1 - NewSegment.originalz2 > 0)) || ((NewSegment.originaly1 - NewSegment.originaly2 < 0) && (NewSegment.originalz1 - NewSegment.originalz2 < 0)))    //Case with increasing slope (from left to right)
                            {
                                if(AmountOf2DCase1 >= maxNumberOf2DSamples){
                                    shouldAdd = false;
                                }
                                else{
                                    AmountOf2DCase1++;
                                }
                            }
                            else{
                                if(AmountOf2DCase2 >= maxNumberOf2DSamples){
                                    shouldAdd = false;
                                }
                                else{
                                    AmountOf2DCase2++;
                                }
                            }
                            if(shouldAdd == true){
                                NewSegment.x_start = min(thisNeuron.y, thatNeuron.y);
                                NewSegment.x_end = ceil(max(thisNeuron.y, thatNeuron.y));
                                
                                NewSegment.y_start = min(thisNeuron.z, thatNeuron.z);
                                NewSegment.y_end = ceil(max(thisNeuron.z, thatNeuron.z));
                                
                                
                                
    //                            sumx2D += (max(nodeOneY, nodeTwoY) - min(nodeOneY, nodeTwoY)) +1;
    //                            sumy2D += (max(nodeOneZ, nodeTwoZ) - min(nodeOneZ, nodeTwoZ)) +1;
                                
                                sumx2D += (NewSegment.x_end - NewSegment.x_start) +1;
                                sumy2D += (NewSegment.y_end - NewSegment.y_start) +1;
                            }
                        }
                        //X and Z have a dimension
                        else if(nodeOneY == nodeTwoY){
                            
                            if(((NewSegment.originalx1 - NewSegment.originalx2 > 0) && (NewSegment.originalz1 - NewSegment.originalz2 > 0)) || ((NewSegment.originalx1 - NewSegment.originalx2 < 0) && (NewSegment.originalz1 - NewSegment.originalz2 < 0)))
                            {
                                if(AmountOf2DCase1 >= maxNumberOf2DSamples){
                                    shouldAdd = false;
                                }
                                else{
                                    AmountOf2DCase1++;
                                }
                            }
                            else{
                                if(AmountOf2DCase2 >= maxNumberOf2DSamples){
                                    shouldAdd = false;
                                }
                                else{
                                    AmountOf2DCase2++;
                                }
                               
                            }
                            if(shouldAdd == true){
                                NewSegment.x_start = min(thisNeuron.x, thatNeuron.x);
                                NewSegment.x_end = ceil(max(thisNeuron.x, thatNeuron.x));
                                
                                NewSegment.y_start = min(thisNeuron.z, thatNeuron.z);
                                NewSegment.y_end = ceil(max(thisNeuron.z, thatNeuron.z));
    //                            sumx2D += (max(nodeOneX, nodeTwoX) - min(nodeOneX, nodeTwoX)) +1;
    //                            sumy2D += (max(nodeOneY, nodeTwoY) - min(nodeOneY, nodeTwoY)) +1;
                                sumx2D += (NewSegment.x_end - NewSegment.x_start) +1;
                                sumy2D += (NewSegment.y_end - NewSegment.y_start) +1;
                            }
                        }
                        //X and Y have a dimension
                        else{
                            
                            if(((NewSegment.originalx1 - NewSegment.originalx2 > 0) && (NewSegment.originaly1 - NewSegment.originaly2 > 0)) || ((NewSegment.originalx1 - NewSegment.originalx2 < 0) && (NewSegment.originaly1 - NewSegment.originaly2 < 0)))
                            {
                                if(AmountOf2DCase1 >= maxNumberOf2DSamples){
                                    shouldAdd = false;
                                }
                                else{
                                    AmountOf2DCase1++;
                                }
                            }
                            else{
                                if(AmountOf2DCase2 >= maxNumberOf2DSamples){
                                    shouldAdd = false;
                                }
                                else{
                                    AmountOf2DCase2++;
                                }
                                
                            }
                            if(shouldAdd == true){
                                NewSegment.x_start = min(thisNeuron.x, thatNeuron.x);
                                NewSegment.x_end = ceil(max(thisNeuron.x, thatNeuron.x));
                                
                                NewSegment.y_start = min(thisNeuron.y, thatNeuron.y);
                                NewSegment.y_end = ceil(max(thisNeuron.y, thatNeuron.y));
    //                            sumx2D += ((max(nodeOneX, nodeTwoX) - min(nodeOneX, nodeTwoX)) +1;
    //                            sumy2D += (max(nodeOneZ, nodeTwoZ) - min(nodeOneZ, nodeTwoZ)) +1;
                                sumx2D += (NewSegment.x_end - NewSegment.x_start) +1;
                                sumy2D += (NewSegment.y_end - NewSegment.y_start) +1;
                            }
                        }
                        if(shouldAdd == true){
                            cout << "Added " << (NewSegment.x_end - NewSegment.x_start) +1 << " " << (NewSegment.y_end - NewSegment.y_start) +1 << " " << (NewSegment.z_end - NewSegment.z_start) +1 << endl;
                            cout << "From :: " << NewSegment.x_end << " - " << NewSegment.x_start << ", " << NewSegment.y_end << " - " << NewSegment.y_start << " + 1, " << NewSegment.z_start << " - " << NewSegment.z_end << " + 1" << endl;
    //                        sumx += (NewSegment.x_end - NewSegment.x_start) +1;
    //                        sumy += (NewSegment.y_end - NewSegment.y_start) +1;

                            
                            NewSegment.z_start = 0; //Just set to 0 since 2D
                            NewSegment.z_end = 0;
                            cout << "New Sum :: " << sumy2D << endl;
                            total2D++;
                            
                            ROIs2D.push_back(NewSegment);
                        }
                    }
                    //3D case
                    else{
                        NewSegment.originalx1 = thisNeuron.x;
                        NewSegment.originaly1 = thisNeuron.y;
                        NewSegment.originalz1 = thisNeuron.z;
                        NewSegment.originalx2 = thatNeuron.x;
                        NewSegment.originaly2 = thatNeuron.y;
                        NewSegment.originalz2 = thatNeuron.z;
                        if((NewSegment.originalz1-NewSegment.originalz2 < 0 && NewSegment.originaly1-NewSegment.originaly2 > 0 && NewSegment.originalx1 - NewSegment.originalx2 < 0) || (NewSegment.originalz1-NewSegment.originalz2 > 0 && NewSegment.originaly1-NewSegment.originaly2 < 0 && NewSegment.originalx1 - NewSegment.originalx2 > 0)){
                        
                            if(AmountOf3DCase1 >= maxNumberOf3DSamples){
                                continue;
                            }
                            AmountOf3DCase1++;
                            }
                        
                        else if((NewSegment.originalz1-NewSegment.originalz2 < 0 && NewSegment.originaly1-NewSegment.originaly2 < 0 && NewSegment.originalx1 - NewSegment.originalx2 < 0) || (NewSegment.originalz1-NewSegment.originalz2 > 0 && NewSegment.originaly1-NewSegment.originaly2 > 0 && NewSegment.originalx1 - NewSegment.originalx2 > 0)){
                         
                            if(AmountOf3DCase2 >= maxNumberOf3DSamples){
                                continue;
                            }
                            AmountOf3DCase2++;
                        }
                
                        else if((NewSegment.originalz1-NewSegment.originalz2 < 0 && NewSegment.originaly1-NewSegment.originaly2 < 0 && NewSegment.originalx1 - NewSegment.originalx2 > 0) || (NewSegment.originalz1-NewSegment.originalz2 > 0 && NewSegment.originaly1-NewSegment.originaly2 > 0 && NewSegment.originalx1 - NewSegment.originalx2 < 0)){
                         
                            if(AmountOf3DCase3 >= maxNumberOf3DSamples){
                                continue;
                            }
                            AmountOf3DCase3++;
                        }
                    
        
                        else{
                            if(AmountOf3DCase4 >= maxNumberOf3DSamples){
                                continue;
                            }
                            AmountOf3DCase4++;
                        }
                        
                        cout << "3D Case :: " << thisNeuron.x << ", " << thisNeuron.y << ", " << thisNeuron.z << " to " << thatNeuron.x << ", " << thatNeuron.y << ", " << thatNeuron.z << endl;
                        cout << "Translated to :: " << nodeOneX << ", " << nodeOneY << ", " << nodeOneZ << " to " << nodeTwoX << ", " << nodeTwoY << ", " << nodeTwoZ << endl;
                        NewSegment.x_start = min(thisNeuron.x, thatNeuron.x);
                        NewSegment.x_end = ceil(max(thisNeuron.x, thatNeuron.x));
                        
                        sumx += (NewSegment.x_end - NewSegment.x_start) +1;
                        
                        NewSegment.y_start = min(thisNeuron.y, thatNeuron.y);
                        NewSegment.y_end = ceil(max(thisNeuron.y, thatNeuron.y));
                        
                        sumy += (NewSegment.y_end - NewSegment.y_start) +1;
                        
                        NewSegment.z_start = min(thisNeuron.z, thatNeuron.z);
                        NewSegment.z_end = ceil(max(thisNeuron.z, thatNeuron.z));
                        
                        sumz += (NewSegment.z_end - NewSegment.z_start) +1;
                        
                        
                        total3D++;
                        
                        ROIs.push_back(NewSegment);
                    }
                }
            }
        }
        
    }
    cout << "The amount of 2D Neuron Segements Case 1 Afer Assigning is :: " << AmountOf2DCase1 << endl;
    cout << "The amount of 2D Neuron Segements Case 2 Afer Assigning is :: " << AmountOf2DCase2 << endl;
    cout << "The amount of 3D Neuron Segements Case 1 Afer Assigning is :: " << AmountOf3DCase1 << endl;
    cout << "The amount of 3D Neuron Segements Case 2 Afer Assigning is :: " << AmountOf3DCase2 << endl;
    cout << "The amount of 3D Neuron Segements Case 3 Afer Assigning is :: " << AmountOf3DCase3 << endl;
    cout << "The amount of 3D Neuron Segements Case 4 Afer Assigning is :: " << AmountOf3DCase4 << endl;
    
    cout << "THE SUM IN 2D Y IS " << sumy2D << endl << "AND THE TOTAL IS " << total2D << endl;

    int averagex1D = sumx1D/total1D;
    
    int averagex2D = sumx2D/total2D;
    int averagey2D = sumy2D/total2D;
    
    
    cout << "AND THE AVERAGE IS " << averagey2D << endl;
    
    
    int averagex3D = sumx/total3D;  //Training data will have this x-dimension for 3D data
    int averagey3D = sumy/total3D;  //Training data will have this y-dimension for 3D data
    int averagez3D = sumz/total3D;  //Training data will have this z-dimension for 3D data
    
    cout << "Size of the 3D vector: " << ROIs.size() << " should be = " << total3D << endl;
    cout << "Size of the 2D vector: " << ROIs2D.size() << "Sould be = " << total2D << endl;
    cout << "Size of the 1D vector: " << ROIs1D.size() << "Sould be = " << total1D << endl;
    cout << "Found the segments, will now generate subimages based on these sets" << endl;
    cout << "The traversed Dimensions will be: " << averagex3D << " by " << averagey3D << " by " << averagez3D << endl;
    
    
    Image4DSimple* tImage = callback.getImage(win);
    unsigned char* image1d = tImage->getRawData();
    
    int xdim = tImage->getXDim();
    int ydim = tImage->getYDim();
    int zdim = tImage->getZDim();
    int ImageNumber = 0;    //3D
    int ImageNumber1D = 0;  //1D
    int ImageNumber2D = 0;  //2D
    vector<Image4DSimple*> ThreeDImageData;
    vector<Image4DSimple*> TwoDImageData;
    vector<Image4DSimple*> OneDImageData;
    
    int num_elements = sizeof(image1d) / sizeof(image1d[0]);
    cout << "The array is " << num_elements <<  " large" << endl;
    
    int count = 0;
    
    for(vector<ROISegment>::iterator it = ROIs1D.begin(); it != ROIs1D.end(); ++it){
//        int xdim = it->x_end - it->x_start + 1;
//        int ydim = 1;
//        int zdim = 1;
        count++;
        cout << "count :: " << count << endl;
        cout << "Iterating through 1D set :: " << endl << it->x_start << " to " << it->x_end << endl;
        int newX = 0;
        unsigned char newData[(int)it->x_end - (int)it->x_start +1];
        
        if((int)it->y_start - (int)it->y_end == 0 && (int)it->z_start - (int)it->z_end == 0){
            int offSetYZ = it->originalz1*ydim*xdim + it->originaly1*xdim;
            for(int i = (int)it->x_start; i <= (int)it->x_end; i++){    //1D data --> Only populate the X-Dim
                int totalOffset = i + offSetYZ;
                cout << "Grabbing data from positions :: " << totalOffset << endl;
                newData[newX] = image1d[totalOffset];
                newX++;
            }
        }
        
        if((int)it->x_start - (int)it->x_end == 0 && (int)it->z_start - (int)it->z_end == 0){
            int offSetXZ = it->originalz1*ydim*xdim + it->originalx1;
            for(int i = (int)it->x_start; i <= (int)it->x_end; i++){    //1D data --> Only populate the X-Dim
                int totalOffset = i*xdim + offSetXZ;
                cout << "Grabbing data from positions :: " << totalOffset << endl;
                newData[newX] = image1d[totalOffset];
                newX++;
            }
        }
        
        if((int)it->x_start - (int)it->x_end == 0 && (int)it->y_start - (int)it->y_end == 0){
            int offSetXY = it->originaly1*xdim + it->originalx1;
            for(int i = (int)it->z_start; i <= (int)it->z_end; i++){    //1D data --> Only populate the X-Dim
                int totalOffset = i*ydim*xdim + offSetXY;
                cout << "Grabbing data from positions :: " << totalOffset << endl;
                newData[newX] = image1d[totalOffset];
                newX++;
            }
        }
        cout << "Size :: " << newX << endl;
        
        //Linear Interp
        double ratiox = ((double)((int)it->x_end-(int)it->x_start +1))/((double)averagex1D);
        int newXDim = it->x_end - it->x_start +1;
        int posCounter = 0;
        
        int newCount = 0;
        unsigned char resizedData2[averagex1D];
        for(int i = 0; i <= averagex1D-1; i++){
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
                cout << "s = " << s << " pos = " << pos << endl;
            }
            V3DLONG result = s/(cubeVolume);
            cout << "Cube Volume = " << cubeVolume << endl;
            cout << "result (Calc average)" << result << endl;
            resizedData2[posCounter] = (unsigned char) result;
            posCounter++;
        }
        
        Image4DSimple* newImage = new Image4DSimple;
        int xdim = (int)it->x_end-(int)it->x_start+1;
        int ydim = (int)it->y_end-(int)it->y_start+1;
        int zdim = (int)it->z_end-(int)it->z_start+1;
        
        //cout << "Data2: " << newData << endl;
        newImage->setData(newData, (int)it->x_end-(int)it->x_start+1, (int)it->y_end-(int)it->y_start+1, (int)it->z_end-(int)it->z_start+1, tImage->getCDim(), tImage->getDatatype());
        cout << "data is set" << endl;
        stringstream ss;
        ss << it->originalx1 << "_" << it->originaly1 << "_" << it->originalz1 << "_" << it->originalx2 << "_" << it->originaly2 << "_" << it->originalz2;
        string str = ss.str();
        string fileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages/TrainingImage" + str + ".v3draw";
        if(callback.saveImage(newImage,(char*)fileName.c_str())) {
            cout << "Image Saved" << endl;
        }
        
        Image4DSimple* resizedImage = new Image4DSimple;
        int resizedX = averagex1D;
        
        resizedImage->setData(resizedData2, averagex1D, 1, 1, tImage->getCDim(), tImage->getDatatype());
        cout << "Resized data is set" << endl;
        string resizedFileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages2/1DResizedTrainingImage" + str + ".v3draw";
        if(callback.saveImage(resizedImage, (char*)resizedFileName.c_str())){
            cout << "Resized Image Saved" << endl;
            OneDImageData.push_back(resizedImage);
            ImageNumber1D++;
        }
    }
    
    //2D cases
    
    for(vector<ROISegment>::iterator it = ROIs2D.begin(); it!=ROIs2D.end(); ++it){
//        int xdim = it->x_end - it->x_start +1;
//        int yDim = it->y_end - it->y_start +1;
//        int zdim = 1;
        
        cout << "Iterating though 2D set :: " << endl << it->x_start << " to " << it->x_end << endl << it->y_start << " to " << it->y_end << endl;
        int newX = 0;
        int newY = 0;
        unsigned char newData[(((int)it->x_end - (int)it->x_start) + 1)*(((int)it->y_end - (int)it->y_start) + 1)];
        
        //Modified January 18th, 2017
        //Previously did not take into consideration that the xz or yz plane would be the roi. Added Conditionals and modified the respective offsets
        
        //X-Y Plane
        
        if((int)it->originalz1 - (int)it->originalz2 == 0){
            for(int i = (int)it->y_start; i <= (int)it->y_end; i++){
                int offsety = i*xdim;
                for(int j = (int)it->x_start; j <= (int)it->x_end; j++){
                    int totalOffset = it->originalz1*xdim*ydim + offsety + j;
                    cout << "Grabbing data from the positions: " << totalOffset << endl;
                    newData[newX] = image1d[totalOffset];
                    newX++;
                }
            }
        }
        
        // Y-Z Plane
        else if((int)it->originalx1 - (int)it->originalx2 == 0){
            for(int i = (int)it->y_start; i <= (int)it->y_end; i++){
                int offsetz = i*xdim*ydim;
                for(int j = (int)it->x_start; j <= (int)it->x_end; j++){
                    int totalOffset = offsetz + j*xdim + it->originalx1;
                    cout << "Grabbing data from the positions: " << totalOffset << endl;
                    newData[newX] = image1d[totalOffset];
                    newX++;
                }
            }
        }
        
        //X-Z plane
        else{
            for(int i = (int)it->y_start; i <= (int)it->y_end; i++){
                int offsetz = i*xdim*ydim;
                for(int j = (int)it->x_start; j <= (int)it->x_end; j++){
                    int totalOffset = offsetz + it->originaly1*xdim + j;
                    cout << "Grabbing data from the positions: " << totalOffset << endl;
                    newData[newX] = image1d[totalOffset];
                    newX++;
                }
            }
        }
        cout << "SIZE :: " << newX << endl;
        double ratiox = ((double)((int)it->x_end-(int)it->x_start+1))/((double)averagex2D);
        double ratioy = ((double)((int)it->y_end-(int)it->y_start+1))/((double)averagey2D);
        cout << "Average x2D :: " << averagex2D << endl << "Average y2D :: " << averagey2D << endl;
        cout << "Sum2Dx :: " << sumx2D << endl << "Sum 2Dy :: " << sumy2D << endl << "Total 2Dx :: " << total2D << endl;
        cout << "Ratios in 2D: " << ratiox << ", " << ratioy << endl;
        
        unsigned char resizedData[averagex2D*averagey2D];
        
        int newCount = 0;
        for(int j = 0; j <= averagey2D -1; j++){
            for(int i = 0; i <= averagex2D -1; i++){
                int oldXID = ceil(ratiox*i + it->x_start);
                int oldYID = ceil(ratioy*j + it->y_start);
                int newOffset = oldYID*xdim + oldXID;
                resizedData[newCount] = image1d[newOffset];
                newCount++;
            }
        }
        
        int newXDim = it->x_end - it->x_start +1;
        int newYDim = it->y_end - it->y_start +1;
        int posCounter = 0;
        unsigned char resizedData2[averagex2D*averagey2D];
        for(int j = 0; j <= averagey2D -1; j++){
            int j2low = floor(j*ratioy);
            int j2high = floor((j+1)*ratioy-1);
            if(j2high < j2low){
                j2high = j2low;
            }
            if(j2high > newYDim -1){
                j2high = newYDim -1;
            }
            int jw = j2high - j2low + 1;
            
            for(int i = 0; i <= averagex2D -1; i++){
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
                        cout << "s = " << s << " pos = " << pos << endl;
                    }
                }
                V3DLONG result = s/cubeVolume;
                cout << "Cube volume :: " << cubeVolume << endl;
                cout << "Results :: " << result << endl;
                resizedData2[posCounter] = (unsigned char) result;
                posCounter++;
            }
        }
        Image4DSimple* newImage = new Image4DSimple;
        int xdim = (int)it->x_end-(int)it->x_start+1;
        int ydim = (int)it->y_end-(int)it->y_start+1;
        int zdim = (int)it->z_end-(int)it->z_start+1;
        
        //cout << "Data2: " << newData << endl;
        newImage->setData(newData, (int)it->x_end-(int)it->x_start+1, (int)it->y_end-(int)it->y_start+1, (int)it->z_end-(int)it->z_start+1, tImage->getCDim(), tImage->getDatatype());
        cout << "data is set" << endl;
        stringstream ss;
        ss << it->originalx1 << "_" << it->originaly1 << "_" << it->originalz1 << "_" << it->originalx2 << "_" << it->originaly2 << "_" << it->originalz2;
        string str = ss.str();
        string fileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages/TrainingImage" + str + ".v3draw";
        if(callback.saveImage(newImage,(char*)fileName.c_str())) {
            cout << "Image Saved" << endl;
        }
        
        Image4DSimple* resizedImage = new Image4DSimple;
        int resizedX = averagex2D;
        int resizedY = averagey2D;
        
        resizedImage->setData(resizedData2, averagex2D, averagey2D, 1, tImage->getCDim(), tImage->getDatatype());
        cout << "Resized data is set" << endl;
        string resizedFileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages2/2DResizedTrainingImage" + str + ".v3draw";
        if(callback.saveImage(resizedImage, (char*)resizedFileName.c_str())){
            cout << "Resized Image Saved" << endl;
            TwoDImageData.push_back(resizedImage);
            ImageNumber2D++;
        }
        
        
    }
    
    for(vector<ROISegment>::iterator it = ROIs.begin(); it != ROIs.end(); ++it){
        //Allocate space for each training image
        cout << "Iterating through coordinates: " << endl << it->x_start << " to " << it->x_end << endl << it->y_start << " to " << it->y_end << endl << it->z_start << " to " << it->z_end << endl;
        int newx = 0;
        int newy = 0;
        int newz = 0;
        unsigned char newData[((((int)it->x_end-(int)it->x_start)+1)*(((int)it->y_end-(int)it->y_start)+1)*(((int)it->z_end-(int)it->z_start))+1)]; //store each subImage Data Here
        for(int i = (int)it->z_start; i <= (int)it->z_end; i++)
        {
            int offsetz = i*xdim*ydim;
            for(int j = (int)it->y_start; j <= (int)it->y_end; j++)
            {
                int offsety = j*xdim;
                for(int k = (int)it->x_start; k <= (int)it->x_end; k++)
                {
                    int offsetTot = offsetz + offsety + k;
                    cout << "Grabbing data from the postions: " << offsetTot << endl;
                    newData[newx] = image1d[offsetTot];
                    newx++; //Added 11/12 (Makes more sense)
                }
            }
        }
        cout << "SIZE :: " << newx << endl;
        
        /*
         Code for resizing the training images follows
            Added November 12th, 2016
         */
        
        //If upsampling...may not matter
        
        //MAY REMOVE FOR THE METHOD THAT FOLLOWS THIS
        double ratiox = ((double)((int)it->x_end-(int)it->x_start +1))/((double) averagex3D);
        double ratioy = ((double)((int)it->y_end-(int)it->y_start +1))/((double) averagey3D);
        double ratioz = ((double)((int)it->z_end-(int)it->z_start +1))/((double) averagez3D);
        cout << "Ratio's " << ratiox << ", " << ratioy << ", " << ratioz << endl;
        
        unsigned char resizedData[averagex3D*averagey3D*averagez3D];  //Resized Training data
        int newCount = 0;
        for(int k = 0; k <= averagez3D -1; k++){
            for(int j = 0; j <= averagey3D -1; j++){
                for(int i = 0; i <= averagex3D -1; i++){
                    int oldXID = ceil(ratiox*i + it->x_start);
                    int oldYID = ceil(ratioy*j + it->y_start);
                    int oldZID = ceil(ratioz*k + it->z_start);
                    int newOffset = oldZID*(xdim*ydim) + oldYID*xdim + oldXID;
                    resizedData[newCount] = image1d[newOffset];
                    newCount++;
                }
            }
        }
        int newXDim = it->x_end-it->x_start + 1;
        int newYDim = it->y_end-it->y_start + 1;
        int newZDim = it->z_end-it->z_start + 1;
        int posCounter = 0;
        //Added November 15th, 2017 Linear Interp
        unsigned char resizedData2[averagex3D*averagey3D*averagez3D];
        for(int k = 0; k <= averagez3D-1; k++){
            
            int k2low = floor(k*ratioz);// + it->z_start;
            int k2high = floor((k+1)*ratioz-1);// + it->z_start;
            
            if(k2high < k2low){
                k2high = k2low;
            }
            
            if(k2high > newZDim - 1){
                k2high = newZDim - 1;
            }
            
            int kw = k2high - k2low +1;
            
            for(int j = 0; j <= averagey3D-1; j++){
                
                int j2low = floor(j*ratioy);// + it->y_start;
                int j2high = floor((j+1)*ratioy-1);// + it->y_start;
                
                if(j2high < j2low){
                    j2high = j2low;
                }
                if(j2high > newYDim - 1){
                    j2high = newYDim -1 ;
                }
                
                int jw = j2high - j2low + 1;
                
                for(int i = 0; i <= averagex3D-1; i++){
                    
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
                                cout << "s = " << s << "  pos = " << pos << endl;
                            }
                        }
                    }
                    //V3DLONG adjustedS = s;// + (it->z_start*(xdim*ydim)+it->y_start*xdim+it->x_start);
                    //if(cubeVolume == 0) cubeVolume = 1;
                    V3DLONG result = s/(cubeVolume);
                    cout << "cube volume :: " << cubeVolume << endl;
                    cout << "result (Calc Average) :: " << result << endl;
                    //cout << "Counter check :: " << posCounter << endl;
                    resizedData2[posCounter] = (unsigned char) result;
                    posCounter++;
                    
                }
            }
        }
        
        //cout << "Data: " << newData << endl;
        Image4DSimple* newImage = new Image4DSimple;
        int xdim = (int)it->x_end-(int)it->x_start+1;
        int ydim = (int)it->y_end-(int)it->y_start+1;
        int zdim = (int)it->z_end-(int)it->z_start+1;

        //cout << "Data2: " << newData << endl;
        newImage->setData(newData, (int)it->x_end-(int)it->x_start+1, (int)it->y_end-(int)it->y_start+1, (int)it->z_end-(int)it->z_start+1, tImage->getCDim(), tImage->getDatatype());
        cout << "data is set" << endl;
        stringstream ss;
        ss << it->originalx1 << "_" << it->originaly1 << "_" << it->originalz1 << "_" << it->originalx2 << "_" << it->originaly2 << "_" << it->originalz2;
        string str = ss.str();
        string fileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages/TrainingImage" + str + ".v3draw";
        if(callback.saveImage(newImage,(char*)fileName.c_str())) {
            cout << "Image Saved" << endl;
        }
        
        Image4DSimple* resizedImage = new Image4DSimple;
        int resizedX = averagex3D;
        int resizedY = averagey3D;
        int resizedZ = averagez3D;
        
        resizedImage->setData(resizedData2, averagex3D, averagey3D, averagez3D, tImage->getCDim(), tImage->getDatatype());
        cout << "Resized data is set" << endl;
        string resizedFileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages2/ResizedTrainingImage" + str + ".v3draw";
        if(callback.saveImage(resizedImage, (char*)resizedFileName.c_str())){
            cout << "Resized Image Saved" << endl;
            ThreeDImageData.push_back(resizedImage);
            ImageNumber++;
        }

    }

    
    cout << "The average x: " << averagex3D << endl << "The average y: " << averagey3D << endl << "The average z: " << averagez3D << endl;
    
    //Added November 9th, 2016
    //Need various classes. And in those various classes positive and negative training data.
    //Classes can be implemented later on, now the focus is to train with positive and negative samples
    //First, generate a list of positive and negative training data.
    //Each Image that was previously generated will be assigned as either positive or negative
    //Put these in different classes later on
    
    //Added December 5th, 2016
    //Setting up the negative training data
    //Seperate loops for 3D, 2D and 1D
    vector<Image4DSimple *> Negative1DImageData;
    vector<Image4DSimple *> Negative2DImageData;
    vector<Image4DSimple *> Negative3DImageData;
    
    vector<ROISegment> Negative1D;
    vector<ROISegment> Negative2D;
    vector<ROISegment> Negative3D;
    
    srand(time(NULL));
    for(int i = 0; i < OneDImageData.size(); i++){
        bool isNegativeSample = false; //If a random sample is part of positive data set then find new random samples
        while(isNegativeSample == false){
            ROISegment newNegativeSegment;
            int coordRand = rand() % 3; //0 for changing x, 1 for changing y, and 2 for changing z;
            if(coordRand == 0){ //X changes
                //find a random y and z position that are constant
                int yCoord = rand()%ydim;
                int zCoord = rand()%zdim;
                
                int startingX = rand()%(xdim - averagex1D); //if the start position is on the edge then rest will be out of bounds
                newNegativeSegment.x_start = startingX;
                newNegativeSegment.x_end = startingX + averagex1D;
                newNegativeSegment.y_start = yCoord;
                newNegativeSegment.y_end = yCoord;
                newNegativeSegment.z_start = zCoord;
                newNegativeSegment.z_end = zCoord;
                
            }
            else if(coordRand == 1){    //y changes
                //find a random x and z position that are different
                int xCoord = rand()%xdim;
                int zCoord = rand()%zdim;
                
                int startingY = rand()%(ydim - averagex1D); //if the start position is on the edge then rest will be out of bounds
                newNegativeSegment.x_start = xCoord;
                newNegativeSegment.x_end = xCoord;
                newNegativeSegment.y_start = startingY;
                newNegativeSegment.y_end = startingY + averagex1D;
                newNegativeSegment.z_start = zCoord;
                newNegativeSegment.z_end = zCoord;
            }
            else if(coordRand == 2){    //z changes
                //find a random x and y position that are different
                int xCoord = rand()%xdim;
                int yCoord = rand()%ydim;
                
                int startingZ = rand()%(zdim - averagex1D);
                newNegativeSegment.x_start = xCoord;
                newNegativeSegment.x_end = xCoord;
                newNegativeSegment.y_start = yCoord;
                newNegativeSegment.y_end = yCoord;
                newNegativeSegment.z_start = startingZ;
                newNegativeSegment.z_end = startingZ + averagex1D;
                
            }
            bool foundCopy = false;
            for(vector<ROISegment>::iterator it = Negative1D.begin(); it != Negative1D.end(); ++it){    //Just look to see if the random image already exists in our data
                if(newNegativeSegment.x_start == it->x_start && newNegativeSegment.x_end == it->x_end && newNegativeSegment.y_start == it->y_start && newNegativeSegment.y_end == it->y_end && newNegativeSegment.z_start == it->z_start && newNegativeSegment.z_end == it->z_end){
                    foundCopy = true;
                    break;
                }
            }
            
            for(vector<ROISegment>::iterator it = ROIs1D.begin(); it != ROIs1D.end(); ++it){
                if(newNegativeSegment.x_start == it->originalx1 && newNegativeSegment.x_end == it->originalx2 && newNegativeSegment.y_start == it->originaly1 && newNegativeSegment.y_end == it->originaly2 && newNegativeSegment.z_start == it->originalz1 && newNegativeSegment.z_end == it->originalz2){
                    foundCopy = true;
                    break;
                }
            }
            
            if(foundCopy == false){
                isNegativeSample = true;
                Negative1D.push_back(newNegativeSegment);
                cout << "Added Negative sample: " << endl << newNegativeSegment.x_start << " " << newNegativeSegment.x_end << endl << newNegativeSegment.y_start << " " << newNegativeSegment.y_end << endl << newNegativeSegment.z_start << " " << newNegativeSegment.z_end << endl << endl;
                //Pull the image data from the image itself and store it in the vector on negative 1D ROI's
                
                //Depending on what dimension is the one that changes, grabbing the pixel data will be different.
                
                Image4DSimple* newNegativeImage = new Image4DSimple;
                unsigned char newData[averagex1D + 1];
                
                if(newNegativeSegment.y_start == newNegativeSegment.y_end && newNegativeSegment.z_start == newNegativeSegment.z_end){   //If the x dimension changes
                    int pos = 0;
                    int offsetZ = xdim*ydim*newNegativeSegment.z_start;
                    int offsetY = xdim*newNegativeSegment.y_start;
                    for(int x = newNegativeSegment.x_start; x <= newNegativeSegment.x_end; x++){
                        newData[pos] = image1d[offsetY + offsetZ + x];
                        pos++;
                        cout << "Added" << endl;
                    }
                }
                
                
                else if(newNegativeSegment.x_start == newNegativeSegment.x_end && newNegativeSegment.z_start == newNegativeSegment.z_end){ //If the y dimension changes
                    int pos = 0;
                    int offSetX = newNegativeSegment.x_start;
                    int offSetZ = xdim*ydim*newNegativeSegment.z_start;
                    for(int y = newNegativeSegment.y_start; y <= newNegativeSegment.y_end; y++){
                        newData[pos] = image1d[offSetZ + offSetX + xdim*y];
                        pos++;
                        cout << "Added" << endl;
                    }
                }
                
                else{   //Case where the z dimension changes.
                    int pos = 0;
                    int offSetX = newNegativeSegment.x_start;
                    int offSetY = xdim*newNegativeSegment.y_start;
                    for(int z = newNegativeSegment.z_start; z <= newNegativeSegment.z_end; z++){
                        newData[pos] = image1d[offSetX + offSetY + xdim*ydim*z];
                        pos++;
                        cout << "Added" << endl;
                    }
                }
                newNegativeImage->setData(newData, (int)newNegativeSegment.x_end-(int)newNegativeSegment.x_start+1, (int)newNegativeSegment.y_end-(int)newNegativeSegment.y_start+1, (int)newNegativeSegment.z_end-(int)newNegativeSegment.z_start+1, tImage->getCDim(), tImage->getDatatype());
                
                cout << "Dimensions :: " << (int)newNegativeSegment.x_end-(int)newNegativeSegment.x_start+1 << ", " << (int)newNegativeSegment.y_end-(int)newNegativeSegment.y_start+1 << ", " << (int)newNegativeSegment.z_end-(int)newNegativeSegment.z_start+1 << endl << "Size of data array :: " << sizeof( newData ) / sizeof( newData[0] ) << endl;
                
                Negative1DImageData.push_back(newNegativeImage);
                
                cout << "Saving Negative Training Sample To Disk" << endl;
                
                stringstream ss;
                ss << newNegativeSegment.x_start << "_" << newNegativeSegment.y_start << "_" << newNegativeSegment.z_start << "_" << newNegativeSegment.x_end << "_" << newNegativeSegment.y_end << "_" << newNegativeSegment.z_end;
                
                string str = ss.str();
                
                string resizedFileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages2/Negative_Training_Image" + str + ".v3draw";
                if(callback.saveImage(newNegativeImage, (char*)resizedFileName.c_str())){
                    cout << "Resized Image Saved" << endl;
                }
            }
        }
    }
    for(int i = 0; i < maxNumberOf2DSamples; i++){
        bool isNegativeSample = false; //If a random sample is part of positive data set then find new random samples
        ROISegment newNegativeSegment;
        while(isNegativeSample == false){
            ROISegment newNegativeSegment;
            int coordRand = rand() % 3; //Which coordinate does NOT change 0=x, 1=y, 2=z (since we are dealing with 2D images in this case)
            if(coordRand == 0){     //xcoord is constant
                
                int xCoord = rand()%xdim;   //x coord which is constant
                int startingY = rand()%(ydim - averagex2D);
                int startingZ = rand()%(zdim - averagey2D);
                newNegativeSegment.x_start = xCoord;
                newNegativeSegment.x_end = xCoord;
                newNegativeSegment.y_start = startingY;
                newNegativeSegment.y_end = startingY + averagex2D;
                newNegativeSegment.z_start = startingZ;
                newNegativeSegment.z_end = startingZ + averagey2D;
            }
            
            else if(coordRand == 1){ //y coord is constant
                
                int yCoord = rand()%ydim; //y dim stays the same
                int startingX = rand()%(xdim - averagex2D);
                int startingZ = rand()%(zdim - averagey2D);
                newNegativeSegment.x_start = startingX;
                newNegativeSegment.x_end = startingX + averagex2D;
                newNegativeSegment.y_start = yCoord;
                newNegativeSegment.y_end = yCoord;
                newNegativeSegment.z_start = startingZ;
                newNegativeSegment.z_end = startingZ + averagey2D;
            }
            
            else if(coordRand == 2){    //z coord is constant
                int zCoord = rand()%zdim;
                int startingX = rand()%(xdim - averagex2D);
                int startingY = rand()%(ydim - averagey2D);
                newNegativeSegment.x_start = startingX;
                newNegativeSegment.x_end = startingX + averagex2D;
                newNegativeSegment.y_start = startingY;
                newNegativeSegment.y_end = startingY + averagey2D;
                newNegativeSegment.z_start = zCoord;
                newNegativeSegment.z_end = zCoord;
            }
        bool foundCopy = false;
        for(vector<ROISegment>::iterator it = Negative2D.begin(); it != Negative2D.end(); ++it){    //Just look to see if the random image already exists in our data
            if(newNegativeSegment.x_start == it->x_start && newNegativeSegment.x_end == it->x_end && newNegativeSegment.y_start == it->y_start && newNegativeSegment.y_end == it->y_end && newNegativeSegment.z_start == it->z_start && newNegativeSegment.z_end == it->z_end){
                foundCopy = true;
                break;
            }
        }
            
        for(vector<ROISegment>::iterator it = ROIs2D.begin(); it != ROIs2D.end(); ++it){
            if(newNegativeSegment.x_start == it->originalx1 && newNegativeSegment.x_end == it->originalx2 && newNegativeSegment.y_start == it->originaly1 && newNegativeSegment.y_end == it->originaly2 && newNegativeSegment.z_start == it->originalz1 && newNegativeSegment.z_end == it->originalz2){
                foundCopy = true;
                break;
            }
        }
        if(foundCopy == false){
            isNegativeSample = true;
            Negative2D.push_back(newNegativeSegment);
            cout << "Added Negative sample (2D) : " << endl << newNegativeSegment.x_start << " " << newNegativeSegment.x_end << endl << newNegativeSegment.y_start << " " << newNegativeSegment.y_end << endl << newNegativeSegment.z_start << " " << newNegativeSegment.z_end << endl << endl;
            
            //get the image data from the image based on random sampling ROI segments
            Image4DSimple* newNegativeImage = new Image4DSimple;
            unsigned char newData[(averagex2D + 1)* (averagey2D + 1)];
            
            //If the xDim does not change (y & z does)
            if(newNegativeSegment.x_start == newNegativeSegment.x_end && newNegativeSegment.y_start != newNegativeSegment.y_end && newNegativeSegment.z_start != newNegativeSegment.z_end){
                cout << "XDIM DOESNT CHANGE IN 2D NEGATIVE TRAINNG SAMPLE" << endl;
                int pos = 0;
                for(int z = newNegativeSegment.z_start; z <= newNegativeSegment.z_end; z++){
                    int offsetZ = xdim*ydim*z;
                    for(int y = newNegativeSegment.y_start; y <= newNegativeSegment.y_end; y++){
                        int offsetYZ = xdim*y + offsetZ;
                        int offSetXYZ = offsetYZ + newNegativeSegment.x_start;
                        newData[pos] = image1d[offSetXYZ];
                        pos++;
                    }
                    
                }
            }
        
            //if the ydim does not change (x and z do)
            else if(newNegativeSegment.x_start != newNegativeSegment.x_end && newNegativeSegment.y_start == newNegativeSegment.y_end && newNegativeSegment.z_start != newNegativeSegment.z_end){
                cout << "yDIM DOESNT CHANGE IN 2D NEGATIVE TRAINNG SAMPLE" << endl;
                int pos = 0;
                for(int z = newNegativeSegment.z_start; z <= newNegativeSegment.z_end; z++){
                    int offsetZ = xdim*ydim*z;
                    for(int x = newNegativeSegment.x_start; x <= newNegativeSegment.x_end; x++){
                        int offsetXZ = x + offsetZ;
                        int offSetXYZ = offsetXZ + newNegativeSegment.y_start*xdim;
                        newData[pos] = image1d[offSetXYZ];
                        pos++;
                    }
                    
                    
                }
            }
            
            //if the zdim does not change (x and y do)
            else{
                cout << "ZDIM DOESNT CHANGE IN 2D NEGATIVE TRAINNG SAMPLE" << endl;
                int pos = 0;
                for(int y = newNegativeSegment.y_start; y <= newNegativeSegment.y_end; y++){
                    int offsetY = xdim*y;
                    for(int x = newNegativeSegment.x_start; x <= newNegativeSegment.x_end; x++){
                        int offsetXY = x + offsetY;
                        int offSetXYZ = offsetXY + newNegativeSegment.z_start*xdim*ydim;
                        newData[pos] = image1d[offSetXYZ];

                        pos++;
                    }
                    
                }
            }
            //FINISH THIS
            
            newNegativeImage->setData(newData, (int)newNegativeSegment.x_end-(int)newNegativeSegment.x_start+1, (int)newNegativeSegment.y_end-(int)newNegativeSegment.y_start+1, (int)newNegativeSegment.z_end-(int)newNegativeSegment.z_start+1, tImage->getCDim(), tImage->getDatatype());
            
            Negative2DImageData.push_back(newNegativeImage);
            
            cout << "Saving Negative Training Sample To Disk" << endl;
            
            stringstream ss;
            ss << newNegativeSegment.x_start << "_" << newNegativeSegment.y_start << "_" << newNegativeSegment.z_start << "_" << newNegativeSegment.x_end << "_" << newNegativeSegment.y_end << "_" << newNegativeSegment.z_end;
            
            string str = ss.str();
            
            string resizedFileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages2/Negative_Training_Image" + str + ".v3draw";
            cout << endl << "The data being written: " << endl;
            for(int i = 0; i < sizeof(newData)/sizeof(newData[0]); i++){
                cout << (int)newData[i] << ", ";
            }
            cout << endl;
            
            if(callback.saveImage(newNegativeImage, (char*)resizedFileName.c_str())){
                cout << "Resized Image Saved" << endl;
            }
            
            }
        }
    }
    for(int i = 0; i < maxNumberOf3DSamples; i++){
        //All dimensions change in the 3D case. No Constants.
        bool isNegativeSample = false;
        ROISegment newNegativeSegment;
        while (isNegativeSample == false) {
            
            int startingX = rand()%(xdim - averagex3D);
            int startingY = rand()%(ydim - averagey3D);
            int startingZ = rand()%(zdim - averagez3D);
            newNegativeSegment.x_start = startingX;
            newNegativeSegment.x_end = startingX + averagex3D;
            newNegativeSegment.y_start = startingY;
            newNegativeSegment.y_end = startingY + averagey3D;
            newNegativeSegment.z_start = startingZ;
            newNegativeSegment.z_end = startingZ + averagez3D;
            
            bool foundCopy = false;
             for(vector<ROISegment>::iterator it = Negative3D.begin(); it != Negative3D.end(); ++it){    //Just look to see if the random image already exists in our data
                 if(newNegativeSegment.x_start == it->x_start && newNegativeSegment.x_end == it->x_end && newNegativeSegment.y_start == it->y_start && newNegativeSegment.y_end == it->y_end && newNegativeSegment.z_start == it->z_start && newNegativeSegment.z_end == it->z_end){
                     foundCopy = true;
                     break;
                 }
             }
            for(vector<ROISegment>::iterator it = ROIs.begin(); it != ROIs.end(); ++it){
                if(newNegativeSegment.x_start == it->originalx1 && newNegativeSegment.x_end == it->originalx2 && newNegativeSegment.y_start == it->originaly1 && newNegativeSegment.y_end == it->originaly2 && newNegativeSegment.z_start == it->originalz1 && newNegativeSegment.z_end == it->originalz2){
                    foundCopy = true;
                    break;
                }
            }
            
            if(foundCopy == false){
                isNegativeSample = true;
                Negative3D.push_back(newNegativeSegment);
                cout << "Added Negative sample (3D) : " << endl << newNegativeSegment.x_start << " " << newNegativeSegment.x_end << endl << newNegativeSegment.y_start << " " << newNegativeSegment.y_end << endl << newNegativeSegment.z_start << " " << newNegativeSegment.z_end << endl << endl;
                
                Image4DSimple* newNegativeImage = new Image4DSimple;
                unsigned char newData[(averagex3D + 1)* (averagey3D + 1)*(averagez3D + 1)];
                
                int pos = 0;
                for(int k = newNegativeSegment.z_start; k <= newNegativeSegment.z_end; k++){
                    int offSetZ = xdim*ydim*k;
                    for(int j = newNegativeSegment.y_start; j <= newNegativeSegment.y_end; j++){
                        int offSetYZ = offSetZ + j*xdim;
                        for(int i = newNegativeSegment.x_start; i <= newNegativeSegment.x_end; i++){
                            int offSetXYZ = offSetYZ + i;
                            newData[pos] = image1d[offSetXYZ];
                            pos++;
                        }
                    }
                }
                
                newNegativeImage->setData(newData, (int)newNegativeSegment.x_end-(int)newNegativeSegment.x_start+1, (int)newNegativeSegment.y_end-(int)newNegativeSegment.y_start+1, (int)newNegativeSegment.z_end-(int)newNegativeSegment.z_start+1, tImage->getCDim(), tImage->getDatatype());
                
                Negative3DImageData.push_back(newNegativeImage);
                
                cout << "Saving Negative Training Sample To Disk" << endl;
                
                stringstream ss;
                ss << newNegativeSegment.x_start << "_" << newNegativeSegment.y_start << "_" << newNegativeSegment.z_start << "_" << newNegativeSegment.x_end << "_" << newNegativeSegment.y_end << "_" << newNegativeSegment.z_end;
                
                string str = ss.str();
                
                string resizedFileName = "/Volumes/Mac-Backup/AllenInstituteResearch/TrainingImages2/Negative_Training_Image" + str + ".v3draw";
                cout << endl << "The data being written: " << endl;
                for(int i = 0; i < sizeof(newData)/sizeof(newData[0]); i++){
                    cout << (int)newData[i] << ", ";
                }
                cout << endl;
                if(callback.saveImage(newNegativeImage, (char*)resizedFileName.c_str())){
                    cout << "Resized Image Saved" << endl;
                }
    
            }
        }
        
    }
    

    
    cout << "Will now set up training.." << endl;
    cout << "The sizes of the images should be :: " << endl << "For one 1D Classifier :: " << averagex1D << endl << "For the 2D classifier :: " << averagex2D << ", " << averagey2D << endl << "For the 3D classifier :: " << averagex3D << ", " << averagey3D << ", " << averagez3D << endl;
    setUpTrainingData(OneDImageData, TwoDImageData, ThreeDImageData, averagex1D, averagex2D, averagey2D, averagex3D, averagey3D, averagez3D, ImageNumber,ImageNumber1D, ImageNumber2D, ROIs1D, ROIs2D ,ROIs, Negative1DImageData, Negative2DImageData, Negative3DImageData, maxNumberOf2DSamples, maxNumberOf3DSamples, callback, parent);
    
    return 0;
}

bool setUpTrainingData(vector<Image4DSimple*> OneDImageData, vector<Image4DSimple*> TwoDImageData, vector<Image4DSimple*> ThreeDImageData, int averagex1D, int averagex2D, int averagey2D,int xDim, int yDim, int zDim, int numeberOfSamples, int numberOfSamples1D, int numberofSamples2D, vector<ROISegment> ROIs1D, vector<ROISegment> ROIs2D, vector<ROISegment> ROIs3D, vector<Image4DSimple*> Negative1DImageData, vector<Image4DSimple*> Negative2DImageData, vector<Image4DSimple*> Negative3DImageData, int maxNumberOf2DSamples, int maxNumberOf3DSamples, V3DPluginCallback2 &callback, QWidget *parent){
    
    //Extract Features
    //Define classifiers for each case
    ofstream outfile;
    
    outfile.open("/Users/randallsuvanto/Desktop/outPutTraining.txt");
    
    //Set up the output file
    
    outfile << "X1 \t Y1 \t Z1 \t X2 \t Y2 \t Z2 \t \t target\n";

    float *features1D = new float[2*numberOfSamples1D*averagex1D];
    float *features2D = new float[maxNumberOf2DSamples*averagex2D*averagey2D + numberofSamples2D*averagex2D*averagey2D];
    float *features3D = new float[maxNumberOf3DSamples*xDim*yDim*zDim + numeberOfSamples*xDim*yDim*zDim];
    
    int *targets1D = new int[2*OneDImageData.size()];
    int *targets2D = new int[maxNumberOf2DSamples+TwoDImageData.size()];
    int *targets3D = new int[maxNumberOf3DSamples+ThreeDImageData.size()];
    
    int oneDPos = 0;
    int oneDPos1 = 0;
    
    for(vector<Image4DSimple*>::iterator it = OneDImageData.begin(); it != OneDImageData.end(); ++it){
        Image4DSimple* thisImage = *it;
        const unsigned char *data = thisImage->getRawData();
        for(int i = 0; i < averagex1D; i++){
            int thisNum = (int)data[i];
            features1D[oneDPos] = (float)data[i];
            oneDPos++;
        }
        ROISegment thisSegment;
        thisSegment.x_start = (ROIs1D.begin()+oneDPos1)->x_start;
        thisSegment.x_end = (ROIs1D.begin() + oneDPos1)->x_end;
        thisSegment.y_start = (ROIs1D.begin() + oneDPos1)->y_start;
        thisSegment.y_end = (ROIs1D.begin() + oneDPos1)->y_end;
        thisSegment.originalx1 = (ROIs1D.begin()+oneDPos1)->originalx1;
        thisSegment.originalx2 = (ROIs1D.begin()+oneDPos1)->originalx2;
        thisSegment.originaly1 = (ROIs1D.begin()+oneDPos1)->originaly1;
        thisSegment.originaly2 = (ROIs1D.begin()+oneDPos1)->originaly2;
        thisSegment.originalz1 = (ROIs1D.begin()+oneDPos1)->originalz1;
        thisSegment.originalz2 = (ROIs1D.begin()+oneDPos1)->originalz2;
        
        //One case: x_start to x_end. The image data has been adjusted before so that all the info is stored in the 'x' variables
        //There are 2 cases: true or false. Still need a way to get false data where targets1D[oneDPos1] = 0
        targets1D[oneDPos1] = 1;
        
        oneDPos1++;
    }
    
    //Assign the negative data
    for(vector<Image4DSimple*>::iterator it = Negative1DImageData.begin(); it != Negative1DImageData.end(); ++it){
        Image4DSimple* thisImage = *it;
        const unsigned char *data = thisImage->getRawData();
        for(int i = 0; i < averagex1D; i++){
            int thisNum = (int)data[i];
            features1D[oneDPos] = (float)data[i];
            oneDPos++;
        }
        targets1D[oneDPos1] = 0;
        oneDPos1++;
    }
    
    
    int TwoDPos = 0;
    int TwoDPos1 = 0;
    for(vector<Image4DSimple*>::iterator it = TwoDImageData.begin(); it != TwoDImageData.end(); ++it){
        Image4DSimple * thisImage = *it;
        const unsigned char * data = thisImage->getRawData();
        for(int i = 0; i < averagex2D*averagey2D; i++){
            int thisNum = (int)data[i];
            features2D[TwoDPos] = (float)data[i];
            TwoDPos++;
        }
        ROISegment thisSegment;
        thisSegment.x_start = (ROIs2D.begin()+TwoDPos1)->x_start;
        thisSegment.x_end = (ROIs2D.begin() + TwoDPos1)->x_end;
        thisSegment.y_start = (ROIs2D.begin() + TwoDPos1)->y_start;
        thisSegment.y_end = (ROIs2D.begin() + TwoDPos1)->y_end;
        thisSegment.z_start = (ROIs2D.begin() + TwoDPos1)->z_start;
        thisSegment.z_end = (ROIs2D.begin() + TwoDPos1)->z_end;
        thisSegment.originalx1 = (ROIs2D.begin()+TwoDPos1)->originalx1;
        thisSegment.originalx2 = (ROIs2D.begin()+TwoDPos1)->originalx2;
        thisSegment.originaly1 = (ROIs2D.begin()+TwoDPos1)->originaly1;
        thisSegment.originaly2 = (ROIs2D.begin()+TwoDPos1)->originaly2;
        thisSegment.originalz1 = (ROIs2D.begin()+TwoDPos1)->originalz1;
        thisSegment.originalz2 = (ROIs2D.begin()+TwoDPos1)->originalz2;
        
        //3 total cases: Diagonal going from bottom left to top right or diagonal going from top left to bottom right plus 1 for negative
        
        //Need to look for which plane this segement is on first
        
        //Still Need Negative cases...
        
        //First of 3 cases : y,z plane

        if(thisSegment.originalx1 - thisSegment.originalx2 == 0){
            if(((thisSegment.originaly1 - thisSegment.originaly2 > 0) && (thisSegment.originalz1 - thisSegment.originalz2 > 0)) || ((thisSegment.originaly1 - thisSegment.originaly2 < 0) && (thisSegment.originalz1 - thisSegment.originalz2 < 0)))    //Case with increasing slope (from left to right)
            {
                
                cout << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
                cout << "Setting the target = 1" << endl << endl;
                targets2D[TwoDPos1] = 1;
            }
            else   //Case with decreasing slope from left to right
//                else if(((thisSegment.originaly1 - thisSegment.originaly2 > 0) && (thisSegment.originalz1 - thisSegment.originalz2 < 0)) || ((thisSegment.originaly1 - thisSegment.originaly2 < 0) && (thisSegment.originalz1 - thisSegment.originalz2 > 0))) 
            {
                cout << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
                cout << "Setting the target = 2" << endl;
                targets2D[TwoDPos1] = 2;
            }
        }
        
        //Second of 3 cases : x,z plane
        else if(thisSegment.originaly1 - thisSegment.originaly2 == 0){
            
            if(((thisSegment.originalx1 - thisSegment.originalx2 > 0) && (thisSegment.originalz1 - thisSegment.originalz2 > 0)) || ((thisSegment.originalx1 - thisSegment.originalx2 < 0) && (thisSegment.originalz1 - thisSegment.originalz2 < 0)))
            {
                cout << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
                cout << "Setting the target = 1" << endl;
                targets2D[TwoDPos1] = 1;
                
            }
            else
            //else if(((thisSegment.originalx1 - thisSegment.originalx2 > 0) && (thisSegment.originalz1 - thisSegment.originalz2 > 0)) || ((thisSegment.originalx1 - thisSegment.originalx2 < 0) && (thisSegment.originalz1 - thisSegment.originalz2 < 0)))
                
            {
                cout << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
                cout << "Setting the target = 2 "<< endl;
                targets2D[TwoDPos1] = 2;
            }
            
        }
        
        //Last case: in the x,y plane   6_50_7    7_55_7
        else{
            //else if(thisSegment.originalz1 - thisSegment.originalz2 == 0){
            if(((thisSegment.originalx1 - thisSegment.originalx2 > 0) && (thisSegment.originaly1 - thisSegment.originaly2 > 0)) || ((thisSegment.originalx1 - thisSegment.originalx2 < 0) && (thisSegment.originaly1 - thisSegment.originaly2 < 0)))
            {
                cout << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
                cout << "Setting the target = 1" << endl;
                targets2D[TwoDPos1] = 1;
                
            }
            else
//           else if(((thisSegment.originalx1 - thisSegment.originalx2 > 0) && (thisSegment.originaly1 - thisSegment.originaly2 > 0)) || ((thisSegment.originalx1 - thisSegment.originalx2 < 0) && (thisSegment.originaly1 - thisSegment.originaly2 < 0)))
            {
                cout << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
                cout << "Setting the target = 2" << endl;
                targets2D[TwoDPos1] = 2;
            }
            
        }
        int thisTarget = targets2D[TwoDPos1];
        
        
        outfile << thisSegment.originalx1 << " \t " << thisSegment.originaly1 << " \t " << thisSegment.originalz1 << " \t "  << thisSegment.originalx2 << " \t " << thisSegment.originaly2 << " \t " << thisSegment.originalz2 << " \t \t " << thisTarget << "\n";
        
        cout << thisSegment.originalx1 << "_" << thisSegment.originaly1 << "_" << thisSegment.originalz1 << "_" << thisSegment.originalx2 << "_" << thisSegment.y_end << "_" << thisSegment.originalz2 << " label is set to :: " << targets2D[TwoDPos1] << endl;
        TwoDPos1++;
    }
    
    for(vector<Image4DSimple*>::iterator it = Negative2DImageData.begin(); it != Negative2DImageData.end(); ++it){
        Image4DSimple* thisImage = *it;
        cout << "Data in the features array [2D] :: ";
        const unsigned char *data = thisImage->getRawData();
        for(int i = 0; i < averagex2D*averagey2D; i++){
            int thisNum = (int)data[i];
            features2D[TwoDPos] = (float)data[i];
            TwoDPos++;
            
            cout << endl << (int)features2D[TwoDPos] << endl;
        }
        targets2D[TwoDPos1] = 0;
        TwoDPos1++;
    }
    
    
    int pos = 0;
    int pos2 = 0;
    for(vector<Image4DSimple*>::iterator it = ThreeDImageData.begin(); it != ThreeDImageData.end(); ++it){
        Image4DSimple * thisImage = *it;
        const unsigned char * data = thisImage->getRawData();
        for(int i = 0; i < xDim*yDim*zDim; i++){
            int thisNum = (int)data[i];
            features3D[pos2] = (float)data[i];
            pos2++;
        }
        ROISegment thisSegment;
        thisSegment.x_start = (ROIs3D.begin()+pos)->x_start;
        thisSegment.x_end = (ROIs3D.begin()+pos)->x_end;
        thisSegment.y_start = (ROIs3D.begin()+pos)->y_start;
        thisSegment.y_end = (ROIs3D.begin()+pos)->y_end;
        thisSegment.z_start = (ROIs3D.begin()+pos)->z_start;
        thisSegment.z_end = (ROIs3D.begin()+pos)->z_end;
        
        thisSegment.originalx1 = (ROIs3D.begin()+pos)->originalx1;
        thisSegment.originalx2 = (ROIs3D.begin()+pos)->originalx2;
        thisSegment.originaly1 = (ROIs3D.begin()+pos)->originaly1;
        thisSegment.originaly2 = (ROIs3D.begin()+pos)->originaly2;
        thisSegment.originalz1 = (ROIs3D.begin()+pos)->originalz1;
        thisSegment.originalz2 = (ROIs3D.begin()+pos)->originalz2;
        
        //Total of 5 cases:
            //1: Upper left corner of facing plane to the back lower right plane
            //2: Upper right corner of facing plane to the back lower left plane
            //3: Bottom left corner of facing plane to the back upper right plane
            //4: Bottom right conter of facing plane to the back upper left plane.
            //5: Negative
        
        //first case:
        cout << "Differences :: " << endl << "X :: " << thisSegment.originalx1-thisSegment.originalx2 << endl << "Y :: " << thisSegment.originaly1 - thisSegment.originaly2 << endl << "Z :: " << thisSegment.originalz1 - thisSegment.originalz2 << endl;
        
        if((thisSegment.originalz1-thisSegment.originalz2 < 0 && thisSegment.originaly1-thisSegment.originaly2 > 0 && thisSegment.originalx1 - thisSegment.originalx2 < 0) || (thisSegment.originalz1-thisSegment.originalz2 > 0 && thisSegment.originaly1-thisSegment.originaly2 < 0 && thisSegment.originalx1 - thisSegment.originalx2 > 0)){
            
            cout << "3D Classifier " << endl << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
            cout << "Setting the target = 1" << endl;
            targets3D[pos] = 1;
            
        }
        
        //second case:
        else if((thisSegment.originalz1-thisSegment.originalz2 < 0 && thisSegment.originaly1-thisSegment.originaly2 < 0 && thisSegment.originalx1 - thisSegment.originalx2 < 0) || (thisSegment.originalz1-thisSegment.originalz2 > 0 && thisSegment.originaly1-thisSegment.originaly2 > 0 && thisSegment.originalx1 - thisSegment.originalx2 > 0)){
            
            cout << "3D Classifier " << endl << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
            cout << "Setting the target = 2" << endl;
            targets3D[pos] = 2;
            
        }
        
        //third case:
        else if((thisSegment.originalz1-thisSegment.originalz2 < 0 && thisSegment.originaly1-thisSegment.originaly2 < 0 && thisSegment.originalx1 - thisSegment.originalx2 > 0) || (thisSegment.originalz1-thisSegment.originalz2 > 0 && thisSegment.originaly1-thisSegment.originaly2 > 0 && thisSegment.originalx1 - thisSegment.originalx2 < 0)){
            
            cout << "3D Classifier " << endl << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
            cout << "Setting the target = 3" << endl;
            targets3D[pos] = 3;
            
        }
        
        //fourth case:
        else{
            
            cout << "3D Classifier " << endl << thisSegment.originalx1 << ", " << thisSegment.originaly1 << ", " << thisSegment.originalz1 << endl << " to"  << endl << thisSegment.originalx2 << ", " << thisSegment.originaly2 << ", " << thisSegment.originalz2 << endl;
            cout << "Setting the target = 4" << endl;
            targets3D[pos] = 4;
            
        }
        //else if((thisSegment.originalz1-thisSegment.originalz2 < 0 && thisSegment.originaly1-thisSegment.originaly2 > 0 && thisSegment.originalx1 - thisSegment.originalx2 > 0) || (thisSegment.originalz1-thisSegment.originalz2 > 0 && thisSegment.originaly1-thisSegment.originaly2 < 0 && thisSegment.originalx1 - thisSegment.originalx2 < 0))
        
        //Last case to be added: Negative samples

        pos++;
    }
    
    for(vector<Image4DSimple*>::iterator it = Negative3DImageData.begin(); it != Negative3DImageData.end(); ++it){
        Image4DSimple* thisImage = *it;
        const unsigned char *data = thisImage->getRawData();
        for(int i = 0; i < xDim*yDim*zDim; i++){
            int thisNum = (int)data[i];
            features3D[pos2] = (float)data[i];
            pos2++;
        }
        targets3D[pos] = 0;
        pos++;
    }
    
    int numberCubes1D = 2*OneDImageData.size();
    int numOfFeatures1D = averagex1D;
    
    int numberCubes2D = maxNumberOf2DSamples+TwoDImageData.size();
    int numOfFeatures2D = averagex2D*averagey2D;
    
    
    int numberCubes3D = maxNumberOf3DSamples+ThreeDImageData.size();
    int numOfFeatures3D = xDim*yDim*zDim;
    
    cout << "Number of 1D cubes: " << numberCubes1D << endl;
    cout << "Number of 2D cubes: " << numberCubes2D << endl;
    cout << "Number of 3D cubes: " << numberCubes3D << endl << "Number of Features: " << numOfFeatures3D << endl;
    cout << "Size of the 2D Positive Image Vector :: " << TwoDImageData.size() <<endl;
    cout << "Size of the 3D Positive Image Vector :: " << ThreeDImageData.size() <<endl;
    
    cout << "Size of the 2D Positive ROI Vector :: " << ROIs2D.size() << endl;
    cout << "Size of the 3D Positive ROI Vector :: " << ROIs3D.size() << endl;
    
   // exit(0);
    
    SVMClassifier* OneDClassifier = new SVMClassifier();
    SVMClassifier* TwoDClassifier = new SVMClassifier();
    SVMClassifier* ThreeDClassifier = new SVMClassifier();
    
    cout << "Training the 1D classifier" << endl;
    OneDClassifier->train(features1D, targets1D, numberCubes1D, numOfFeatures1D);
    cout << endl << endl;
    cout << "Training the 2D classifier" << endl;
    TwoDClassifier->train(features2D, targets2D, numberCubes2D, numOfFeatures2D);
    cout << endl << endl;
    cout << "Training the 3D classifier" << endl;
    ThreeDClassifier->train(features3D, targets3D, numberCubes3D, numOfFeatures3D);
    cout << endl;
    
}


bool classifySamples(SVMClassifier OneDClassifier, SVMClassifier TwoDClassifier, SVMClassifier ThreeDClassifier, int OneDImageArraySize, int TwoDImageArraySize, int ThreeDImageArraySize, int TwoDXDim, int TwoDYDim, int ThreeDXDim, int ThreeDYDim, int ThreeDZDim, V3DPluginCallback2 &callback,  QWidget *parent){
    
    v3dhandle win = NULL;
    
    QString inputFileName = QFileDialog::getOpenFileName(parent, QObject::tr("Choose the swc file to classify "), QDir::currentPath(),  QObject::tr("*.swc"));
    win = callback.currentImageWindow();
    
    QByteArray ba = inputFileName.toLatin1() + "outputFile.swc";
    char *outputFileName = ba.data();
    ofstream outputFile(outputFileName);
    outputFile << "#name vaa3d_traced_neuron" << endl << "#comment" << endl << "##n,type,x,y,z,radius,parent" << endl;
    
    Image4DSimple* tImage = callback.getImage(win);
    unsigned char* image1d = tImage->getRawData();
    
    int xDim = tImage->getXDim();
    int yDim = tImage->getYDim();
    int zDim = tImage->getZDim();

    NeuronTree neuron1;
    neuron1 = readSWC_file(inputFileName);
    
    vector<ROISegment> ROIs3D;
    vector<ROISegment> ROIs2D;  //May Delete these
    vector<ROISegment> ROIs1D;
    
    vector<Image4DSimple> OneDImages;
    vector<Image4DSimple> TwoDImages;       //May Delete these
    vector<Image4DSimple> ThreeDImages;
    
    for(int i = 0; i < neuron1.listNeuron.size(); i++){
        
        NeuronSWC thisNeuron = neuron1.listNeuron.at(i);
        V3DLONG NodeParent = thisNeuron.pn;
        if(NodeParent == -1)    //If there is no child
        {
            continue;               //ThisNeuron = 1     ThatNeuron = 2
        }
        
        else{
            for(int j = 0; j < neuron1.listNeuron.size(); j++){
                V3DLONG thisID = neuron1.listNeuron.at(j).n;
                 NeuronSWC thatNeuron = neuron1.listNeuron.at(j);
                if(thisID == NodeParent){   //If this is an swc segment
                    
                    Image4DSimple NewImage;
                    
                    ROISegment NewSegment;
                    NewSegment.originalx1 = thisNeuron.x;
                    NewSegment.originaly1 = thisNeuron.y;
                    NewSegment.originalz1 = thisNeuron.z;
                    NewSegment.originalx2 = thatNeuron.x;
                    NewSegment.originaly2 = thatNeuron.y;
                    NewSegment.originalz2 = thatNeuron.z;
                    
                    NewSegment.x_start = min(thisNeuron.x, thatNeuron.x);
                    NewSegment.x_end = ceil(max(thisNeuron.x, thatNeuron.x));
                    NewSegment.y_start = min(thisNeuron.y, thatNeuron.y);
                    NewSegment.y_end = ceil(max(thisNeuron.y, thatNeuron.y));
                    NewSegment.z_start = min(thisNeuron.z, thatNeuron.z);
                    NewSegment.z_end = ceil(max(thisNeuron.z, thatNeuron.z));
                    
                    int nodeOneX = thisNeuron.x;
                    int nodeOneY = thisNeuron.y;
                    int nodeOneZ = thisNeuron.z;
                    int nodeTwoX = thatNeuron.x;
                    int nodeTwoY = thatNeuron.y;
                    int nodeTwoZ = thatNeuron.z;
                    
                    
                    //check if 1D segment, 2D segment, or 3D segment
                    
                    //1D (One dimension is not constant)
                    if((nodeOneX == nodeTwoX && nodeOneY == nodeTwoY && nodeOneZ != nodeTwoZ) || (nodeOneX != nodeTwoX && nodeOneY == nodeTwoY && nodeOneZ == nodeTwoZ) || (nodeOneX == nodeTwoX && nodeOneY != nodeTwoY && nodeOneZ == nodeTwoZ)){
                        unsigned char newData[(NewSegment.x_end - NewSegment.x_start +1)*(NewSegment.y_end - NewSegment.y_start +1)*(NewSegment.z_end-NewSegment.z_start+1)];
                        //Look for specific case of which Dimension changes
                        //ZDim Changes
                        if((nodeOneX == nodeTwoX && nodeOneY == nodeTwoY)){
                            int pos = 0;
                            for (int i = NewSegment.z_start; i <= NewSegment.z_end; i++){
                                int offSet = nodeOneX + xDim*nodeOneY + xDim*yDim*i;
                                newData[pos] = image1d[offSet];
                                pos++;
                            }
                    
                            
                        }
                        //YDim Changes
                        else if(nodeOneX == nodeTwoX && nodeOneZ == nodeTwoZ){
                            int pos = 0;
                            for(int i = NewSegment.y_start; i <= NewSegment.y_end; i++){
                                int offSet = nodeOneX + xDim*i + xDim*yDim*nodeOneZ;
                                newData[pos] = image1d[offSet];
                                pos++;
                            }
                            
                        }
                        //XDim Changes
                        else if(nodeOneY == nodeTwoY && nodeOneZ == nodeTwoZ){
                            int pos = 0;
                            for(int i = NewSegment.x_start; i <= NewSegment.x_end; i++){
                                int offSet = i + xDim*nodeOneY + xDim*yDim*nodeOneZ;
                                newData[pos] = image1d[offSet];
                                pos++;
                            }
                        }
                        
                        unsigned char resizedData[OneDImageArraySize];
                        
                        //Linear Interpolation
                        
                        double ratiox = (((double)((int)NewSegment.x_end-(int)NewSegment.x_start +1))*((double)((int)NewSegment.y_end-(int)NewSegment.y_start +1))*((double)((int)NewSegment.z_end-(int)NewSegment.z_start +1)))/((double)OneDImageArraySize);
                        int newXDim = ((double)((int)NewSegment.x_end-(int)NewSegment.x_start +1))*((double)((int)NewSegment.y_end-(int)NewSegment.y_start +1))*((double)((int)NewSegment.z_end-(int)NewSegment.z_start +1));
                        int posCounter = 0;
                        //Always linear array, one dimension
                        int newCount = 0;
                        for(int i = 0; i <= OneDImageArraySize-1; i++){
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
                                cout << "s = " << s << " pos = " << pos << endl;
                            }
                            V3DLONG result = s/(cubeVolume);
                            cout << "Cube Volume = " << cubeVolume << endl;
                            cout << "result (Calc average)" << result << endl;
                            resizedData[posCounter] = (unsigned char) result;
                            posCounter++;
                        }
                        float *features1D = new float[OneDImageArraySize];
                        for(int i = 0; i <= OneDImageArraySize; i++){
                            features1D[i] = (float)resizedData[i];
                        }
                        int classifierResult = OneDClassifier.classifyASample(features1D, OneDImageArraySize);
                        cout << "The result after 1D classification :: " << classifierResult << endl;
                        
                        if(classifierResult == 1){
                            cout << "Connecting the segment " << endl;
                        }
                        else{
                            cout << "Not connecting the segment " << endl;
                        }
                        
                    }
                    
                    //2D (Two dimensions are not constant)
                    else if((nodeOneX == nodeTwoX && nodeOneY != nodeTwoY && nodeOneZ != nodeTwoZ) || (nodeOneX != nodeTwoX && nodeOneY == nodeTwoY && nodeOneZ != nodeTwoZ) || (nodeOneX != nodeTwoX && nodeOneY != nodeTwoY && nodeOneZ == nodeTwoZ)){
                        
                        unsigned char newData[(NewSegment.x_end - NewSegment.x_start +1)*(NewSegment.y_end - NewSegment.y_start +1)*(NewSegment.z_end-NewSegment.z_start+1)];
                        int startingX2DSample;
                        int startingY2DSample;
                        int endingX2DSample;
                        int endingY2DSample;
                        
                        //Y-Z Dim
                        if(nodeOneX == nodeTwoX){
                            //Keep the dimensions of the resized images constant (always in the XY plane)
                            startingX2DSample = NewSegment.y_start;
                            endingX2DSample = NewSegment.y_end;
                            startingY2DSample = NewSegment.z_start;
                            endingY2DSample = NewSegment.z_end;
                            
                            
                            int pos = 0;
                            for(int i = NewSegment.y_start; i <= NewSegment.y_end; i++){
                                int offSetXY = nodeOneX + i*xDim;
                                for(int j = NewSegment.z_start; j <= NewSegment.z_end; j++){
                                    int offSet = offSetXY + xDim*yDim*j;
                                    newData[pos] = image1d[offSet];
                                    pos++;
                                }
                            }
                        }
                        
                        //X-Z Dim
                        else if(nodeOneY == nodeTwoY){
                            //Keep the dimensions of the resized images constant (always in the XY plane)
                            startingX2DSample = NewSegment.x_start;
                            endingX2DSample = NewSegment.x_end;
                            startingY2DSample = NewSegment.z_start;
                            endingY2DSample = NewSegment.z_end;
                            
                            int pos = 0;
                            for(int k = NewSegment.z_start; k <= NewSegment.z_end; k++){
                                int offSetYZ = xDim*nodeOneY + xDim*yDim*k;
                                for(int i = NewSegment.x_start; i <= NewSegment.x_end; i++){
                                    int offSet = i + offSetYZ;
                                    newData[pos] = image1d[offSet];
                                    pos++;
                                }
                            }
                        }
                        
                        
                        
                        //X-Y Dim
                        else if(nodeOneZ == nodeTwoZ){
                            //Keep the dimensions of the resized images constant (always in the XY plane)
                            startingX2DSample = NewSegment.x_start;
                            endingX2DSample = NewSegment.x_end;
                            startingY2DSample = NewSegment.y_start;
                            endingY2DSample = NewSegment.y_end;
                            
                            int pos = 0;
                            for(int k = NewSegment.y_start; k <= NewSegment.y_end; k++){
                                int offSetYZ = xDim*k + xDim*yDim*nodeOneZ;
                                for(int i = NewSegment.x_start; i <= NewSegment.x_end; i++){
                                    int offSet = offSetYZ + i;
                                    newData[pos] = image1d[offSet];
                                    pos++;
                                }
                            }
                        }
                        
                        //Linear Interpolation
                        double ratiox = ((double)((int)endingX2DSample-(int)startingX2DSample+1))/((double)TwoDXDim);
                        double ratioy = ((double)((int)endingY2DSample-(int)startingY2DSample+1))/((double)TwoDYDim);
                        int newCount = 0;
                        
                        int newXDim = endingX2DSample - startingX2DSample +1;
                        int newYDim = endingY2DSample - startingY2DSample +1;
                        int posCounter = 0;
                        unsigned char resizedData[TwoDImageArraySize];
                        for(int j = 0; j <= TwoDYDim -1; j++){
                            int j2low = floor(j*ratioy);
                            int j2high = floor((j+1)*ratioy-1);
                            if(j2high < j2low){
                                j2high = j2low;
                            }
                            if(j2high > newYDim -1){
                                j2high = newYDim -1;
                            }
                            int jw = j2high - j2low + 1;
                            
                            for(int i = 0; i <= TwoDXDim -1; i++){
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
                                        cout << "s = " << s << " pos = " << pos << endl;
                                    }
                                }
                                V3DLONG result = s/cubeVolume;
                                cout << "Cube volume :: " << cubeVolume << endl;
                                cout << "Results :: " << result << endl;
                                resizedData[posCounter] = (unsigned char) result;
                                posCounter++;
                            }
                        }
                        float *features2D = new float[TwoDImageArraySize];
                        for(int i = 0; i <= TwoDImageArraySize; i++){
                            features2D[i] = (float)resizedData[i];
                        }
                        int classifierResult = TwoDClassifier.classifyASample(features2D, TwoDImageArraySize);
                        cout << "The result after 2D classification :: " << classifierResult << endl;
                        if(classifierResult == 1){
                            cout << "Case 1. Connecting from ____ to _____" << endl;
                        }
                        else if(classifierResult == 2){
                            cout << "Case 2. Connecting from ____ to _____" << endl;
                        }
                        else{
                            cout << "Case 0. Not Connecting" << endl;
                        }
                        
                    }
                    
                    //3D (All dimensions are not constant
                    else if((nodeOneX != nodeTwoX && nodeOneY != nodeTwoY && nodeOneZ != nodeTwoZ) || (nodeOneX != nodeTwoX && nodeOneY != nodeTwoY && nodeOneZ != nodeTwoZ) || (nodeOneX != nodeTwoX && nodeOneY != nodeTwoY && nodeOneZ != nodeTwoZ)){
                        
                        unsigned char newData[(NewSegment.x_end - NewSegment.x_start +1)*(NewSegment.y_end - NewSegment.y_start +1)*(NewSegment.z_end-NewSegment.z_start+1)];
                        int pos = 0;
                        for(int k = NewSegment.z_start; k <= NewSegment.z_end; k++){
                            int offSetZ = xDim*yDim*k;
                            for(int j = NewSegment.y_start; j <= NewSegment.y_end; j++){
                                int offSetYZ = offSetZ + xDim*j;
                                for(int i = NewSegment.x_start; i <= NewSegment.x_end; i++){
                                    int offSet = offSetYZ + i;
                                    newData[pos] = image1d[offSet];
                                    pos++;
                                }
                            }
                        }
                        
                        int newXDim = NewSegment.x_end-NewSegment.x_start + 1;
                        int newYDim = NewSegment.y_end-NewSegment.y_start + 1;
                        int newZDim = NewSegment.z_end-NewSegment.z_start + 1;
                        int posCounter = 0;
                        
                        double ratiox = ((double)((int)NewSegment.x_end-(int)NewSegment.x_start +1))/((double) ThreeDXDim);
                        double ratioy = ((double)((int)NewSegment.y_end-(int)NewSegment.y_start +1))/((double) ThreeDYDim);
                        double ratioz = ((double)((int)NewSegment.z_end-(int)NewSegment.z_start +1))/((double) ThreeDZDim);

                        unsigned char resizedData2[ThreeDImageArraySize];
                        for(int k = 0; k <= ThreeDZDim-1; k++){
                            
                            int k2low = floor(k*ratioz);// + it->z_start;
                            int k2high = floor((k+1)*ratioz-1);// + it->z_start;
                            
                            if(k2high < k2low){
                                k2high = k2low;
                            }
                            
                            if(k2high > newZDim - 1){
                                k2high = newZDim - 1;
                            }
                            
                            int kw = k2high - k2low +1;
                            
                            for(int j = 0; j <= ThreeDYDim-1; j++){
                                
                                int j2low = floor(j*ratioy);// + it->y_start;
                                int j2high = floor((j+1)*ratioy-1);// + it->y_start;
                                
                                if(j2high < j2low){
                                    j2high = j2low;
                                }
                                if(j2high > newYDim - 1){
                                    j2high = newYDim -1 ;
                                }
                                
                                int jw = j2high - j2low + 1;
                                
                                for(int i = 0; i <= ThreeDXDim-1; i++){
                                    
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
                                                cout << "s = " << s << "  pos = " << pos << endl;
                                            }
                                        }
                                    }
                                    //V3DLONG adjustedS = s;// + (it->z_start*(xdim*ydim)+it->y_start*xdim+it->x_start);
                                    //if(cubeVolume == 0) cubeVolume = 1;
                                    V3DLONG result = s/(cubeVolume);
                                    cout << "cube volume :: " << cubeVolume << endl;
                                    cout << "result (Calc Average) :: " << result << endl;
                                    //cout << "Counter check :: " << posCounter << endl;
                                    resizedData2[posCounter] = (unsigned char) result;
                                    posCounter++;
                                    
                                }
                            }
                        }
                        
                        float *features3D = new float[ThreeDImageArraySize];
                        for(int i = 0; i <= ThreeDImageArraySize; i++){
                            features3D[i] = (float)resizedData2[i];
                        }
                        int classifierResult = ThreeDClassifier.classifyASample(features3D, ThreeDImageArraySize);
                        cout << "The result after 3DClassification :: " << classifierResult << endl;
                        if(classifierResult == 1){
                            cout << "Case 1. Connecting from ____ to _____" << endl;
                        }
                        else if(classifierResult == 2){
                            cout << "Case 2. Connecting from ____ to _____" << endl;
                        }
                        else if(classifierResult == 3){
                            cout << "Case 3. Connecting from ____ to _____" << endl;
                        }
                        else if(classifierResult == 4){
                            cout << "Case 4. Connecting from ____ to _____" << endl;
                        }
                        else{
                            cout << "Case 0. Not Connecting" << endl;
                        }
                        
                        
                    }
                
                    
                    
                }
                
            }
        }
        
    }
    
    
}








