# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./yade
# Target is an application:  

HEADERS += Body.hpp \
           BoundingVolumeFactory.hpp \
           BoundingVolume.hpp \
           BroadCollider.hpp \
           BVAssembly.hpp \
           BVHierarchy.hpp \
           Chrono.hpp \
           ClassFactory.hpp \
           CollisionGeometryFactory.hpp \
           CollisionGeometry.hpp \
           ConnexBody.hpp \
           DynamicEngine.hpp \
           DynLibManager.hpp \
           GeometricalModelFactory.hpp \
           GeometricalModel.hpp \
           IOManager.hpp \
           KinematicEngine.hpp \
           NarrowCollider.hpp \
           NonConnexBody.hpp \
           Omega.hpp \
           Singleton.hpp \
           Tree.hpp \
           Types.hpp \
           Indexable.hpp \
           MultiMethodsManager.hpp \
           CollisionFunctor.hpp \
           Factorable.hpp \
           InteractionModel.hpp \
           Interaction.hpp \
           Contact.hpp \
           FrontEnd.hpp \
           FactoryExceptions.hpp \
           IOManagerExceptions.hpp \
           MultiMethodsManagerExceptions.hpp \
           MultiMethodsManager.tpp \
           CollisionMultiMethodsManager.hpp 
SOURCES += Body.cpp \
           BoundingVolume.cpp \
           BoundingVolumeFactory.cpp \
           BroadCollider.cpp \
           BVAssembly.cpp \
           BVHierarchy.cpp \
           Chrono.cpp \
           ClassFactory.cpp \
           CollisionGeometry.cpp \
           CollisionGeometryFactory.cpp \
           ConnexBody.cpp \
           DynamicEngine.cpp \
           DynLibManager.cpp \
           GeometricalModel.cpp \
           GeometricalModelFactory.cpp \
           IOManager.cpp \
           KinematicEngine.cpp \
           NarrowCollider.cpp \
           NonConnexBody.cpp \
           Omega.cpp \
           yade.cpp \
           Factorable.cpp \
           InteractionModel.cpp \
           Interaction.cpp \
           Contact.cpp \
           FrontEnd.cpp \
           FactoryExceptions.cpp \
           IOManagerExceptions.cpp \
           MultiMethodsManagerExceptions.cpp \
           CollisionMultiMethodsManager.cpp 
LIBS += -lM3D \
-lConstants \
-lSerialization \
-lboost_date_time \
-lglut \
-lQGLViewer \
-rdynamic
INCLUDEPATH = ../toolboxes/Math/M3D \
../toolboxes/Math/Constants \
../toolboxes/Libraries/Serialization \
$(YADECOMPILATIONPATH)
MOC_DIR = $(YADECOMPILATIONPATH)
UI_DIR = $(YADECOMPILATIONPATH)
OBJECTS_DIR = $(YADECOMPILATIONPATH)
QMAKE_LIBDIR = ../toolboxes/Math/M3D/$(YADEDYNLIBPATH) \
../toolboxes/Math/Constants/$(YADEDYNLIBPATH) \
../toolboxes/Libraries/Serialization/$(YADEDYNLIBPATH) \
$(YADEDYNLIBPATH)
DESTDIR = $(YADEBINPATH)
CONFIG += debug \
warn_on
TEMPLATE = app
QtGeneratedFrontEnd.ui.target = QtGeneratedFrontEnd.ui
