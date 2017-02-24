//
//  Mean_Shift.cpp
//  
//
//  Created by Randall Suvanto on 2/23/17.
//
//

#include "Mean_Shift.h"

//Input the vector of neuron trees, and the image itself

QList<NeuronSWC> meanShift(vector<NeuronTree> trees, Image4DSimple * tImage){

    QList<NeuronSWC> returnList;
    LandmarkList ExtractedMarkers;
    for(vector<NeuronTree>::iterator it = trees.begin(); it != trees.end(); ++it){  //Iterate through the traces
        for(int i = 0; i < it->listNeuron.size(); i++){   //Iterate through each SWC node in the traces
            LocationSimple mrk;
            mrk.x = it->listNeuron.at(i).x;
            mrk.y = it->listNeuron.at(i).y;
            mrk.z = it->listNeuron.at(i).z;
            mrk.radius = it->listNeuron.at(i).r;
            ExtractedMarkers.append(mrk);
        }
    }
    
    LandmarkList meanShiftedList = mean_shift_center(tImage->getRawData(), ExtractedMarkers);
    return(ReMapMarkersToSWC(meanShiftedList, trees));
}

QList<NeuronSWC> ReMapMarkersToSWC(LandmarkList inputLList, vector<NeuronTree> inputTree){
    
    QList<NeuronSWC> returnTree;
    int pos = 0;
    int maxIDOffset;
    int offSetID = 0;
    for(vector<NeuronTree>::iterator it = inputTree.begin(); it != inputTree.end(); ++it){
        int pos2 = 0;
        for(int i = 0; i < it->listNeuron.size(); i++){
            NeuronSWC newNode;
            newNode.x = inputLList.at(pos).x;
            newNode.y = inputLList.at(pos).y;
            newNode.z = inputLList.at(pos).z;
            newNode.radius = inputLList.at(pos).radius;
            if(it->listNeuron.at(pos2).pn == -1){
                newNode.pn = -1;
            }
            else{
                newNode.pn = int(it->listNeuron.at(pos2).pn) + offSetID;
            }
            newNode.n = int(it->listNeuron.at(pos2).n) + offSetID;
            returnTree.append(newNode);
            pos++;
            pos2++;
            if((int)newNode.n > maxIDOffset){
                maxIDOffset = newNode.n;
            }
        }
        offSetID = maxIDOffset + 1;
        
    }
    
    return returnTree;
}

LandmarkList mean_shift_center(unsigned char * image_data, LandmarkList LList)
{
    
    mean_shift_fun fun_obj;
    
    //check parameter
    int windowradius=15;
    int intype = 1;     //??????
    //load image and markers
    
    V3DLONG sz_img[4];
    
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

    if (image_data!=0) {delete []image_data; image_data=0;}
    return LList_new_center;
}


