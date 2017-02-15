
TEMPLATE	= lib
CONFIG	+= qt plugin warn_off
#CONFIG	+= x86_64
VAA3DPATH = /Users/randallsuvanto/v3d_external/v3d_main
INCLUDEPATH	+= /Users/randallsuvanto/v3d_external/v3d_main/basic_c_fun/

HEADERS += ConnectOrNot.h
HEADERS	+= Training_Collection_plugin.h
HEADERS += basic_surf_objs.h
HEADERS += my_surf_objs.h
HEADERS += svm.h
HEADERS += SVMClassifier.h
SOURCES += ConnectOrNot.cpp
SOURCES += svm.cpp
SOURCES += SVMClassifier.cpp
SOURCES	+= Training_Collection_plugin.cpp
SOURCES += basic_surf_objs.cpp
SOURCES += my_surf_objs.cpp
SOURCES	+= /Users/randallsuvanto/v3d_external/v3d_main/basic_c_fun/v3d_message.cpp

TARGET	= $$qtLibraryTarget(Training_Collection)
DESTDIR	= /Applications/Vaa3d_V3.100_MacOSX10.9_64bit/plugins/GetTrainingSamples
