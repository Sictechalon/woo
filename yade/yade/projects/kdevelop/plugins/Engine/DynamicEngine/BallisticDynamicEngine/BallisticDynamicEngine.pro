# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./plugins/Engine/DynamicEngine/BallisticDynamicEngine
# Target is a library:  

LIBS += -lRigidBody \
        -lSerialization \
        -lMath \
        -lBody \
        -lEngine \
        -lGeometry \
        -lInteraction \
        -lMultiMethods \
        -rdynamic 
INCLUDEPATH = $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../../plugins/Body/RigidBody/$(YADEDYNLIBPATH) \
               ../../../../toolboxes/Libraries/Serialization/$(YADEDYNLIBPATH) \
               ../../../../toolboxes/Libraries/Math/$(YADEDYNLIBPATH) \
               ../../../../yade/Body/$(YADEDYNLIBPATH) \
               ../../../../yade/Engine/$(YADEDYNLIBPATH) \
               ../../../../yade/Geometry/$(YADEDYNLIBPATH) \
               ../../../../yade/Interaction/$(YADEDYNLIBPATH) \
               ../../../../toolboxes/Libraries/MultiMethods/$(YADEDYNLIBPATH) \
               $(YADEDYNLIBPATH) 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
DESTDIR = $(YADEDYNLIBPATH) 
CONFIG += debug \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += BallisticDynamicEngine.hpp 
SOURCES += BallisticDynamicEngine.cpp 
