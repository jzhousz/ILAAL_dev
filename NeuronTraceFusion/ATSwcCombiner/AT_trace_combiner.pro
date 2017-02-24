
TEMPLATE	= lib
CONFIG	+= qt plugin warn_off
#CONFIG	+= x86_64
VAA3DPATH = ../../../../../v3d_external
VAA3DMAINPATH = ../../../../../v3d_external/v3d_main
INCLUDEPATH	+= $$VAA3DPATH/v3d_main/basic_c_fun

HEADERS	+= AT_trace_combiner_plugin.h
HEADERS += $$VAA3DMAINPATH/basic_c_fun/v3d_message.h
HEADERS += $$VAA3DMAINPATH/basic_c_fun/basic_surf_objs.h
HEADERS += connect_node.h
### headers related to ML ###
HEADERS += ../DataClassifierInterface.h
HEADERS += ../SVMClassifier.h
HEADERS += ../svm.h
HEADERS += ../ConnectOrNot.h
### headers realted to meanshift ###
HEADERS += ../Mean_Shift/mean_shift_fun.h
HEADERS += ../Mean_Shift/mean_shift_extr_template.h
HEADERS += ../Mean_Shift/mean_shift_dialog.h
HEADERS += ../Mean_Shift/heap.h
HEADERS += ../Mean_Shift/fastmarching_dt.h
HEADERS += ../Mean_Shift/Mean_Shift.h

SOURCES	+= AT_trace_combiner_plugin.cpp
SOURCES	+= $$VAA3DMAINPATH/basic_c_fun/v3d_message.cpp
SOURCES	+= $$VAA3DMAINPATH/basic_c_fun/basic_surf_objs.cpp
### sources related to ML ###
SOURCES	+= ../SVMClassifier.cpp
SOURCES	+= ../svm.cpp
SOURCES += ../ConnectOrNot.cpp
### sources related to meanshift ###
SOURCES += ../Mean_Shift/Mean_Shift.cpp
SOURCES += ../Mean_Shift/mean_shift_fun.cpp

TARGET	= $$qtLibraryTarget(AT_trace_combiner)
DESTDIR	= $$VAA3DPATH/bin/plugins/AT_trace_combiner/
