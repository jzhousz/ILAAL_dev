/* AT_trace_combiner_plugin.h
 * Merge and improve swcs for AT images
 * 2016-7-28 : by Jie Zhou
 */
 
#ifndef __AT_TRACE_COMBINER_PLUGIN_H__
#define __AT_TRACE_COMBINER_PLUGIN_H__

#include <QtGui>
#include <v3d_interface.h>

class ATTraceCombinerPlugin : public QObject, public V3DPluginInterface2_1
{
	Q_OBJECT
	Q_INTERFACES(V3DPluginInterface2_1);

public:
	float getPluginVersion() const {return 1.1f;}

	QStringList menulist() const;
	void domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent);

	QStringList funclist() const ;
	bool dofunc(const QString &func_name, const V3DPluginArgList &input, V3DPluginArgList &output, V3DPluginCallback2 &callback, QWidget *parent);
};

#endif

