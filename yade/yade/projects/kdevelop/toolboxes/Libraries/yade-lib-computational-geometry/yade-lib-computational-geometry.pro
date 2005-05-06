# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./toolboxes/Libraries/yade-lib-computational-geometry
# Target is a library:  

HEADERS += Distances3D.hpp \
           Intersections2D.hpp \
           Intersections3D.hpp 
SOURCES += Distances3D.cpp \
           Intersections2D.cpp \
           Intersections3D.cpp 
LIBS += -lyade-lib-wm3-math 
INCLUDEPATH += $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../toolboxes/Libraries/yade-lib-wm3-math/$(YADEDYNLIBPATH) \
               $(YADEDYNLIBPATH) 
CONFIG += release \
          warn_on \
          dll 
TEMPLATE = lib 
