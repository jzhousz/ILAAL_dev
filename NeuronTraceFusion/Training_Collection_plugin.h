/* Training_Collection_plugin.h
 * Gather training data for nueron reconstruction
 * 2016-10-25 : by Randall W. Suvanto
 */
 
#ifndef __TRAINING_COLLECTION_PLUGIN_H__
#define __TRAINING_COLLECTION_PLUGIN_H__

#include <QtGui>
#include <v3d_interface.h>
#include <iostream>
#include <string>


class Training : public QObject, public V3DPluginInterface2_1
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

