# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./yade/Geometry
# Target is a library:  

LIBS += -lSerialization \
        -rdynamic 
INCLUDEPATH = ../../yade/yade \
              ../../yade/MultiMethods \
              ../../yade/Factory \
              ../../toolboxes/Math \
              ../../toolboxes/Libraries/Serialization 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../toolboxes/Libraries/Serialization/$(YADEDYNLIBPATH) 
DESTDIR = $(YADECOMPILATIONPATH) 
CONFIG += debug \
          warn_on \
          staticlib 
TEMPLATE = lib 
HEADERS += BoundingVolume.hpp \
           BoundingVolumeAssembly.hpp \
           BoundingVolumeFactory.hpp \
           BoundingVolumeHierarchy.hpp \
           CollisionGeometry.hpp \
           CollisionGeometryFactory.hpp \
           GeometricalModel.hpp \
           GeometricalModelFactory.hpp 
SOURCES += BoundingVolume.cpp \
           BoundingVolumeAssembly.cpp \
           BoundingVolumeFactory.cpp \
           BoundingVolumeHierarchy.cpp \
           CollisionGeometry.cpp \
           CollisionGeometryFactory.cpp \
           GeometricalModel.cpp \
           GeometricalModelFactory.cpp 
