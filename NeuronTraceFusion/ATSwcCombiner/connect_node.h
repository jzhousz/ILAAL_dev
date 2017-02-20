/**

  Jie Zhou Oct 9th 2016

  Efficient location-based connection of swc nodes.

  Feb 2017: Add machine learning based validation before connecting the nodes.

  **/


#ifndef __CONNECT_NODE_H_
#define __CONNECT_NODE_H_

#include <QtGlobal>
#include <math.h>
#include "basic_surf_objs.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <iterator> 
#include "../ConnectOrNot.h"

using namespace std;

#define defaultValue 0   //not found in the map (contructor default value)
#define  NODENOTFOUND -999999


//build the location hash using the unique hashmpap
//this version builds from a unique node map (instead of the passed node list)
QHash<V3DLONG, V3DLONG> getUniqueLUT(QHash<V3DLONG, NeuronSWC> &nodeMap, int width, int height)
{
	QHash<V3DLONG,V3DLONG> LUT;

        QHash<V3DLONG, NeuronSWC>::iterator it;
        for (it = nodeMap.begin();it != nodeMap.end();it++)
	{
            //swc coordinates are 0-based and some may be float
            //negtive coordiniates (e.g. from neuTube) need to be set to 0
            //Without the following conversion, the location-based hash matching will not work

            int k = (int) (it.value().z);  if (k < 0) k = 0;
            int j = (int) (it.value().y);  if (j < 0) j = 0;
            int i = (int) (it.value().x);  if (i < 0) i = 0;
            //V3DLONG loc = (it.value().z)*(width*height)  + (it.value().y)*width + it.value().x;

            V3DLONG loc = k*(width*height)  + j*width + i;

            LUT.insert(loc, it.key());
	}
	return (LUT);
}

//convert a list to a hashamp to store id and node structure: key: node_id; value: the node strucure
QHash<V3DLONG, NeuronSWC> getNodeMap(QList<NeuronSWC> &neurons)
{
	QHash<V3DLONG,NeuronSWC> nodeMap;

	for (V3DLONG i=0;i<neurons.size();i++)
	{
	    //if there is no one there already, overwrite unless the previous one already has connection to a parent;
		NeuronSWC oldone = nodeMap.value(neurons.at(i).n);
                //if (oldone == defaultValue || oldone.pn == -1)
		    nodeMap.insert(neurons.at(i).n, neurons.at(i));
	}
	return (nodeMap);
}

//search the neigh
NeuronSWC  searchNearest(long centeri,long centerj, long centerk, int threshold, QHash<V3DLONG, NeuronSWC> nodeMap, QHash<V3DLONG, V3DLONG> LUT, int width, int height, int depth)
{
	//check the neighbors with growing radius; 
	//ignore z-direction resolution difference?
    NeuronSWC result;
	for(int r = 0 ;  r < threshold; r++)
	{
	  //for radius r
	  for (int i = centeri - r; i < centeri + r;  i++)
            for (int j = centerj - r; j <centerj + r; j ++)
                  for (int k = centerk -r ; k < centerk + r; k++)
		  {
                     //at least one coordniate must be equal to r. Otherwise, it is an inside voxel that has been processed. //only check the circumvent
                     if ( abs(i-centeri) !=r && abs(j-centerj) != r && abs(k-centerk) != r ) continue;
                         if (i < 0 || i >= width) continue;
                         if (j < 0 || j >= height) continue;
                         if (k < 0  || k >= depth) continue;
			 
			long loc = k*(width*height) + j*width + i;
                        long neuronid = LUT.value(loc);

			if (neuronid != defaultValue)  // THERE IS ONE in the map
                        {

				return nodeMap.value(neuronid);
                        }
                        //else
                         //   cout << "Hasmap returns 0. THERE IS NO NODE AT THIS LOCATION !!" << endl;
  
		 }
       }

      //not found within neighbor
      result.pn = -999999; //indicator
      return result;

}
	 
//QHash<V3DLONG, NeuronSWC> connect_node(QList<NeuronSWC> & neurons, double thres,  int width, int height, int depth)
QHash<V3DLONG, NeuronSWC> connect_node(QList<NeuronSWC> & neurons, double thres,  V3DLONG size[4], unsigned char * data1d)
{
    int width = size[0];
    int height = size[1];
    int depth = size[2];

     //build two hashmap to improve the efficiency of searching for nodes in an image
    QHash<V3DLONG, NeuronSWC> nodeMap = getNodeMap(neurons);  //a hashamp to store id and node structure: key: node_id; value: the node structure
    QHash<V3DLONG,V3DLONG> LUT = getUniqueLUT(nodeMap, width, height); //a hashmap to store location and neuron id: key: x,y,z combined; value: node_id.

    cout << "Done building up hashmaps. Size of nodeMap:" << nodeMap.size() << endl;
    cout << "                           Size of LUT:" << LUT.size() << endl;

   //Connect the nodes that are close enough:
   //Scan through the entire image.
   //For the node in question, search in its surrounding in a gradually increasing radius
   //Until find a node, and it is within a set threshold

	for(long i = 0;  i < width; i++)
        {
          //cout << "at x " << i << endl ;
	  for (long j = 0; j < height; j++)
          {
               for (long k =0; k < depth; k++)
		 {
                        //for location (i,j, k), check if there is any node there
			V3DLONG loc = k*(width*height) + j*width + i;

			int neuronid = LUT.value(loc);
			//if there is one there

                        if (neuronid != defaultValue)  //there is one
			{
			   //get the node
			   NeuronSWC node = nodeMap.value(neuronid);
			   
			   //search surrounding
               NeuronSWC neighborNode = searchNearest(i,j, k, thres, nodeMap, LUT, width, height, depth);

                           //connect the two nodes:
			   //Do not care about who is the parent for now but give the preference to the one what has a parent already to promote connectivity
                           if (neighborNode.pn != NODENOTFOUND)
			   {
                                //   cout << "Found candidates to connect!!! " << endl;
                               //    cout   << node.n << ":" << node.x << " " << node.y << " " << node.z << " pn:" << node.pn << endl;
                                //   cout   << neighborNode.n << ":" << neighborNode.x << " " << neighborNode.y << " " << neighborNode.z <<" pn:" << neighborNode.pn << endl;

                               //Call Machine Learning module
                               cout << "start calling connectOrNot .." << endl;
                               bool MLDecision = connectOrNot(node, neighborNode, data1d, size);
                               cout << "done calling connectOrNot .." << endl;
                                //if (connectOrNot(node, neighborNode, data1d, size) == false)
                               if (MLDecision == false)
                                       continue; //not connect

                                if (node.pn == -1 && neighborNode.pn  != -1) //node in search has no pn
                                {
                                            node.pn = neighborNode.n;
                                            //cout << "before" << nodeMap.value(node.n).pn;
                                            nodeMap.insert(node.n, node); //update the hashmap value;
                                            //cout << " after" << nodeMap.value(node.n).pn << endl;
                                 }
                                 else if (node.pn != -1 && neighborNode.pn  == -1)
                                 {
                                            neighborNode.pn = node.n;
                                              //  cout << "before" << nodeMap.value(neighborNode.n).pn;
                                                nodeMap.insert(neighborNode.n, neighborNode);
                                                // cout << " after" << nodeMap.value(neighborNode.n).pn << endl;
                                 }
                                 else if (node.pn == -1 && neighborNode.pn  == -1)//both are -1
                                 {
                                        node.pn = neighborNode.n;
                                        //  cout << "before" << nodeMap.value(node.n).pn;
                                        nodeMap.insert(node.n, node); //update the hashmap value;
                                        //cout << " after" << nodeMap.value(node.n).pn << endl;
                                 }
                                 else //neither is -1. --> both have parents already (pass?)
                                 {
					     // do nothing for now
                                 }
			   }
                        }
                  } //end of k
                } //end of j
             } //end of is
		
             //convert the nodeMap back to swc file and save to the swc file
             return nodeMap;
}	


	 
//convert a swc node hash to a swc file.	 
void save_swc(QHash<V3DLONG, NeuronSWC> nodeMap, QString fileSaveName, QString fileOpenName)
{
        QFile file(fileSaveName);

	if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        {
                cout << "cannot save to swc file";
                return;
        }
	QTextStream myfile(&file);
	myfile<<"# generated by Vaa3D Plugin connect_node"<<endl;
	myfile<<"# source file(s): "<<fileOpenName<<endl;
	myfile<<"# id,type,x,y,z,r,pid"<<endl;

        for (QHash<V3DLONG, NeuronSWC>::iterator it = nodeMap.begin(); it != nodeMap.end();it++)
        {
          NeuronSWC node = it.value();
          myfile << node.n <<" " << node.type << " "<< node.x <<" "<< node.y << " "<< node.z << " "<< node.r << " " << node.pn << "\n";

        }

	file.close();
	cout<<"swc file written: "<<fileSaveName.toStdString()<<endl;
        return;

}
	 
	 
#endif
	 
	 
