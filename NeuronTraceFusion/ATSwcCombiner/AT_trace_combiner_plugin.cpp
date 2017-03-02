/* AT_trace_combiner_plugin.cpp
 * Merge and improve swcs for AT images
 * This plugin connects nodes in a swc file.
 * The input is one swc file that contains nodes from multiple (meanshifted) swc structures.
 * 2016-7-28 : by Jie Zhou
 * 2016-10-9 : Revised.
 * Usage ./vaa3d64 -x AT_trace_combiner -f combine_swc -i meanshifted.swc -p 20
 *
 * 2017-2-13:  Revised.  Add the raw image so that machine learning is used to validate the decision for connection.
 *
 * Usage ./vaa3d64 -x AT_trace_combiner -f combine_swc -i meanshifted.swc -p 20 image.tiff
 *
 * 2017-2.24:  Add the call to the mean-shifting code so that the pipeline takes in an ANO file, mean-shift, before connect.
 *
 * Usage ./vaa3d64 -x AT_trace_combiner -f combine_swc -i input.ano -p 20 image.tiff
 *
 */

#include "v3d_message.h"
#include <vector>
#include <iostream>
#include "AT_trace_combiner_plugin.h"
#include "connect_node.h"
#include "../Mean_Shift/Mean_Shift.h"

using namespace std;
vector<NeuronTree> load_linker_file(QString qs_linker);
void saveTREES(QList<NeuronSWC> list);
Q_EXPORT_PLUGIN2(AT_trace_combiner, ATTraceCombinerPlugin);

struct input_PARA
{
    QString inano_file;
    QString inimg_file;
    V3DLONG channel = 1;
    int threshold = 0 ;
    int w;
    int h;
    int d;
};

//added fucntions
void print_help();
bool traceCombiner_func(const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 &callback);

QStringList ATTraceCombinerPlugin::menulist() const
{
    return QStringList()
    <<tr("combine_swc")
    <<tr("about");
}

QStringList ATTraceCombinerPlugin::funclist() const
{
    return QStringList()
    <<tr("combine_swc")
    <<tr("help");
}

void ATTraceCombinerPlugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("combine_swc"))
    {
        v3d_msg(" To be implemented");
        //traceCombiner(callback, parent);
    }
    else
    {
        v3d_msg(tr("Merge and improve swcs for AT images. "
                   "Developed by Jie Zhou, 2016-7-28"));
    }
}

bool ATTraceCombinerPlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
    vector<char*> infiles, inparas, outfiles;
    
    if (func_name == tr("combine_swc"))
    {
        input_PARA PARA;
        
        vector<char*> * pinfiles = (input.size() >= 1) ? (vector<char*> *) input[0].p : 0;
        vector<char*> * pparas = (input.size() >= 2) ? (vector<char*> *) input[1].p : 0;
        vector<char*> infiles = (pinfiles != 0) ? * pinfiles : vector<char*>();
        vector<char*> paras = (pparas != 0) ? * pparas : vector<char*>();
        
        if(infiles.empty())
        {
            fprintf (stderr, "Need input swc. \n");
            return false;
        }
        else
        {
            PARA.inano_file = infiles[0];
        }
        int k=0;
        PARA.threshold = (paras.size() >= k+1) ? atoi(paras[k]) : 998;  k++;
        PARA.inimg_file = (paras.size() >= k+1) ? paras[k] : "FileNotExist";  k++; //use parameter to get the 2cond file
        
        
        /////////////////////////////////////// CODE START HERE ///////
        
        //1. load the linker file to get vector<NeuronTree>
        vector<NeuronTree> nt_list = load_linker_file(PARA.inano_file);
        if (nt_list.size() == 0)  return false;
        
        //2. load the image
        cout << "load the image ... "  << endl;
        V3DLONG in_sz[4];
        int datatype = 0;
        unsigned char * data1d = 0;
        //need to get from ANO file. Change later.
        if (!simple_loadimage_wrapper(callback,PARA.inimg_file.toStdString().c_str(), data1d, in_sz, datatype))
        {
            fprintf (stderr, "Error happens in reading the subject file [%s]. Exit. \n",PARA.inimg_file.toStdString().c_str());
            return false;
        }
        
        //3. call meanshifting on each neuron tree, combine the meanshifted ones into one long list
        QList<NeuronSWC> nodeList= meanShift(nt_list, data1d, in_sz);
        
        saveTREES(nodeList);
        
        //4. call connect_node() to get the nodes to get a new swc file
        //QList<NeuronSWC> nodeList = readSWC_file(PARA.inswc_file).listNeuron;
        QHash<V3DLONG, NeuronSWC> nodeMap = connect_node(nodeList, PARA.threshold, in_sz, data1d);
        
        // save to a new swc file, the input file is for information only
        save_swc(nodeMap, "output_connect_node.swc", PARA.inano_file);
        cout << "output saved to output_connect_node.swc" << endl;
        
    }
    else if (func_name == tr("help"))
    {
        
        print_help(); //provide usage
    }
    else return false;
    
    return true;
}

void print_help(){
    cout<<"Example: v3d -x AT_trace_combiner -f combine_swc -i input.ano -p threshold \n"<<endl;
}

void saveTREES(QList<NeuronSWC> list){
    
    cout << "SAVING" << endl;
    QString fileSaveName = "/Applications/Vaa3d_V3.100_MacOSX10.9_64bit/Vaa3d.app/Contents/MacOS/meanshift.swc";
    QFile file(fileSaveName);
    
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        cout << "cannot save to swc file";
        exit(-1);
    }
    
    QTextStream myfile(&file);
    
    
    myfile<<"# generated by Vaa3D mean_Shift"<<endl;
    myfile<<"# id,type,x,y,z,r,pid"<<endl;
    
    for(int i = 0; i < list.size(); i++){
        NeuronSWC node = list.at(i);
        myfile << node.n <<" " << node.type << " "<< node.x <<" "<< node.y << " "<< node.z << " "<< node.r << " " << node.pn << "\n";
    }
    
    file.close();
    
}

vector<NeuronTree> load_linker_file(QString qs_linker)
{
    //read in ANO file which contains one image and multiple swc files
    vector<NeuronTree> nt_list;
    QStringList nameList;
    //QString qs_linker;
    char * dfile_result = NULL;
    V3DLONG neuronNum = 0;
    
    if (qs_linker.toUpper().endsWith(".ANO"))
    {
        cout<<"Read in a linker (ANO)) file."<<endl;
        P_ObjectFileType linker_object;
        if (!loadAnoFile(qs_linker,linker_object))
        {
            fprintf(stderr,"Error in reading the linker file.\n");
            return nt_list;
        }
        nameList = linker_object.swc_file_list; //get the swc list from ANO
        neuronNum += nameList.size();  //number of swc files
        for (V3DLONG i=0;i<neuronNum;i++)
        {
            NeuronTree tmp = readSWC_file(nameList.at(i)); //read each swc file here
            nt_list.push_back(tmp);  //build the internal list for neuron trees
        }
        cout << "ANO file successfully read!!\n";
    }else
    {
        cout << "Expect an ANO file!" << endl;
    }
    
    return nt_list;
    
}


/*
 bool load_linker_file(const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 &callback)
 {
 //get input: the image, and a list of swc files.
 if(input.size()==0 || output.size() != 1) return false;
 char * paras = NULL;
 //parsing input
 vector<char *> * inlist =  (vector<char*> *)(input.at(0).p);
 if (inlist->size()==0)
 {
 cerr<<"You must specify input linker or swc files"<<endl;
 return false;
 }
 //parsing output
 vector<char *> * outlist = (vector<char*> *)(output.at(0).p);
 if (outlist->size()>1)
 {
 cerr << "You cannot specify more than 1 output files"<<endl;
 return false;
 }
 //read in ANO file which contains one image and multiple swc files
 vector<NeuronTree> nt_list;
 QStringList nameList;
 QString qs_linker;
 char * dfile_result = NULL;
 V3DLONG neuronNum = 0;
 for (int i=0;i<inlist->size();i++)
 {
 qs_linker = QString(inlist->at(i));
 if (qs_linker.toUpper().endsWith(".ANO"))
 {
 cout<<"Read in a linker (ANO)) file."<<endl;
 P_ObjectFileType linker_object;
 if (!loadAnoFile(qs_linker,linker_object))
 {
 fprintf(stderr,"Error in reading the linker file.\n");
 return 1;
 }
 nameList = linker_object.swc_file_list; //get the swc list from ANO
 neuronNum += nameList.size();  //number of swc files
 for (V3DLONG i=0;i<neuronNum;i++)
 {
 NeuronTree tmp = readSWC_file(nameList.at(i)); //read each swc file here
 nt_list.push_back(tmp);  //build the internal list for neuron trees
 }
 }
 }
 cout << "ANO file successfully read!!\n";
 }
 void print_help(){
 cout<<"Example: v3d -x AT_trace_combiner -f combine_swc -i mylinker.ano -o combined.swc \n"<<endl;
 }
 */
