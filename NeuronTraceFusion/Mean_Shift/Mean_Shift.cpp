//
//  Mean_Shift.cpp
//  
//
//  Created by Randall Suvanto on 2/23/17.
//
//

#include "Mean_Shift.h"

//Input the vector of neuron trees, and the image itself, plus image dimensions


//MeanShift Each individual tree FIRST and then merge them.
QList<NeuronSWC> meanShift(vector<NeuronTree> trees, unsigned char * image_data, V3DLONG in_sz[4]){

    QList<LandmarkList> meanShiftedLists;
    
    for(vector<NeuronTree>::iterator it = trees.begin(); it != trees.end(); ++it){  //Iterate through the traces
        LandmarkList ExtractedMarkers;
        for(int i = 0; i < it->listNeuron.size(); i++){   //Iterate through each SWC node in the traces
            LocationSimple mrk;
            mrk.x = it->listNeuron.at(i).x;
            mrk.y = it->listNeuron.at(i).y;
            mrk.z = it->listNeuron.at(i).z;
            mrk.radius = it->listNeuron.at(i).r;
            ExtractedMarkers.append(mrk);
        }
        LandmarkList meanShiftedList = mean_shift_center(image_data, ExtractedMarkers, in_sz);
        meanShiftedLists.push_back(ExtractedMarkers);
    }
    if (image_data!=0) {delete []image_data; image_data=0;} //Deallocate memory for image. Not needed anymore
    return(ReMapMarkersToSWC(meanShiftedLists, trees));
}

//Remap the markers to swc format.
QList<NeuronSWC> ReMapMarkersToSWC(QList<LandmarkList> inputQLList, vector<NeuronTree> inputTree){
    
    QList<NeuronSWC> returnTree;    //Define the tree we will return
    int maxIDOffset = 0;                //maxOffsetID. Keep a max value so we know how much we will have to shift each node ID by to avoid duplicates
    int offSetID = 0;               //Final offSet that we will offset each node by. 0 in beginning since no offset yet.
    
    int listNumber = 0; //Which list we are looking at will correlate to which tree we are looking at
    
    for(vector<NeuronTree>::iterator it = inputTree.begin(); it != inputTree.end(); ++it){      //Cycle through the original trees
        int pos2 = 0;               //Define position for
        LandmarkList inputLList = inputQLList.at(listNumber);   //Landmark list at [listnumber] will correlate to the input trees.
        for(int i = 0; i < it->listNeuron.size(); i++){         //Cycle though the Neurons in the original trees
            NeuronSWC newNode;                                  //Define a SWC node
            newNode.x = inputLList.at(pos2).x;                   //Initialize x,y,z coords from the mean shifted LList
            newNode.y = inputLList.at(pos2).y;
            newNode.z = inputLList.at(pos2).z;
            newNode.radius = inputLList.at(pos2).radius;
            if(it->listNeuron.at(pos2).pn == -1){               //if the input tree's node is -1 (no parent - root) then same is true in mean shifted restult
                newNode.pn = -1;
            }
            else{                           //Else the new node parent ID will be equal to the original plus the offset
                newNode.pn = int(it->listNeuron.at(pos2).pn) + offSetID;
            }
            newNode.n = int(it->listNeuron.at(pos2).n) + offSetID;  //Original ID equal to the original ID plus the offset (combining multiple)
            returnTree.append(newNode);                             //Append the node with the new information.
            pos2++;
            if((int)newNode.n > maxIDOffset){   //Update the offset ID for the next list iteration if the new node's ID is greater that the previously initalized
                maxIDOffset = newNode.n;
            }
        }
        listNumber++;   //Next list
        offSetID = maxIDOffset + 1; //set offset ID to be +1 just to be safe
        
    }
    
    return returnTree;
}

LandmarkList mean_shift_center(unsigned char * image_data, LandmarkList LList, V3DLONG in_sz[4])
{
    mean_shift_fun fun_obj;
    
    //check parameter
    int windowradius=15;
    int intype = 1;
    //load image and markers
    
    V3DLONG sz_img[4];
    sz_img[0] = in_sz[0];
    sz_img[1] = in_sz[1];
    sz_img[2] = in_sz[2];
    sz_img[3] = in_sz[3];
    
    V3DLONG size_tmp=sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
    if(intype==1)
    {
        fun_obj.pushNewData<unsigned char>((unsigned char*)image_data, sz_img);
    }
    else if (intype == 2) //V3D_UINT16;
    {
        fun_obj.pushNewData<unsigned short>((unsigned short*)image_data, sz_img);
        convert2UINT8((unsigned short*)image_data, image_data, size_tmp);
    }
    else if(intype == 4) //V3D_FLOAT32;
    {
        fun_obj.pushNewData<float>((float*)image_data, sz_img);
        convert2UINT8((float*)image_data, image_data, size_tmp);
    }
    else
    {
        v3d_msg("Currently this program only supports UINT8, UINT16, and FLOAT32 data type.",0);
//        return;
    }
    
    vector<V3DLONG> poss_landmark;
    poss_landmark=landMarkList2poss(LList, sz_img[0], sz_img[0]*sz_img[1]);
    LandmarkList LList_new_center;
    LList_new_center.clear();
    vector<float> mass_center;
    for (int j=0;j<poss_landmark.size();j++)
    {
        //append the original marker in LList_new_center
        LList[j].name="ori";
        LList[j].color.r=255; LList[j].color.g=LList[j].color.b=0;
        LList_new_center.append(LList.at(j));
        
        mass_center=fun_obj.mean_shift_center(poss_landmark[j],LList.at(j).radius);
        LocationSimple tmp(mass_center[0]+1,mass_center[1]+1,mass_center[2]+1);
        tmp.color.r=170; tmp.color.g=0; tmp.color.b=255;
        tmp.name="ms";
        LList_new_center.append(tmp);
        
        
    }
    qDebug()<<"LList_new_center_size:"<<LList_new_center.size();

//    if (image_data!=0) {delete []image_data; image_data=0;}
    return LList_new_center;
}


