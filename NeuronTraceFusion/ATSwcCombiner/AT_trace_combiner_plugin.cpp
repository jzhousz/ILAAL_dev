/* AT_trace_combiner_plugin.cpp
 * Merge and improve swcs for AT images
 * This plugin connects nodes in a swc file.
 * The input is one swc file that contains nodes from multiple (meanshifted) swc structures.
 * 2016-7-28 : by Jie Zhou
 * 2016-10-9 : Revised.
 *
 *   Usage ./vaa3d64 -x AT_trace_combiner -f combine_swc -i meanshifted.swc -p 20
 *
 */
 
#include "v3d_message.h"
#include <vector>
#include <iostream>
#include "AT_trace_combiner_plugin.h"
#include "connect_node.h"

using namespace std;
Q_EXPORT_PLUGIN2(AT_trace_combiner, ATTraceCombinerPlugin);

struct input_PARA
{
    QString inswc_file;
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
            PARA.inswc_file = infiles[0];
        int k=0;
        PARA.threshold = (paras.size() >= k+1) ? atoi(paras[k]) : 998;  k++;

        /*
        V3DLONG N,M,P,sc,c;
        V3DLONG in_sz[4];

        int datatype = 0;
        if (!simple_loadimage_wrapper(callback,PARA.inimg_file.toStdString().c_str(), data1d, in_sz, datatype))
        {
            fprintf (stderr, "Error happens in reading the subject file [%s]. Exit. \n",PARA.inimg_file.toStdString().c_str());
            return;
        }
        if(PARA.channel < 1 || PARA.channel > in_sz[3])
        {
            fprintf (stderr, "Invalid channel number. \n");
            return;
        }

        N = in_sz[0];
        M = in_sz[1];
        P = in_sz[2];
        sc = in_sz[3];
         */
        /////////////////////////////////////// CODE START HERE ///////
        V3DLONG N = 1084, M = 782, P = 59;  //pass in later
        cout << "width: "  << N << endl;
        cout << "height: "  << M << endl;
        cout << "depth: "  << P << endl;


        QList<NeuronSWC> nodeList;

        nodeList = readSWC_file(PARA.inswc_file).listNeuron;

        cout << "calling connect node ...  " << endl;

        QHash<V3DLONG, NeuronSWC> nodeMap = connect_node(nodeList, PARA.threshold, N, M,  P);

        save_swc(nodeMap, "output_connect_node.swc", PARA.inswc_file);
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
   cout<<"Example: v3d -x AT_trace_combiner -f combine_swc -i input.swc -p threshold \n"<<endl;
}



