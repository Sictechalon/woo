# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./plugins/DynamicEngine/MassSpringBody2RigidBodyDynamicEngine
# Target is a library:  

LIBS += -lMassSpringBody \
        -lRigidBody \
        -lDistances \
        -lSimpleSpringDynamicEngine \
        -lMesh2D \
        -lClosestFeatures \
        -lSerialization \
        $(YADECOMPILATIONPATH)/libBody.a \
        $(YADECOMPILATIONPATH)/libEngine.a \
        $(YADECOMPILATIONPATH)/libGeometry.a \
        $(YADECOMPILATIONPATH)/libInteraction.a \
        $(YADECOMPILATIONPATH)/libMultiMethods.a \
        $(YADECOMPILATIONPATH)/libFactory.a \
        -rdynamic 
INCLUDEPATH = ../../../plugins/DynamicEngine/SimpleSpringDynamicEngine \
              ../../../plugins/Body/MassSpringBody \
              ../../../plugins/Body/RigidBody \
              ../../../plugins/GeometricalModel/Mesh2D \
              ../../../plugins/InteractionModel/ClosestFeatures \
              ../../../yade/yade \
              ../../../yade/Body \
              ../../../yade/Engine \
              ../../../yade/Geometry \
              ../../../yade/Interaction \
              ../../../yade/MultiMethods \
              ../../../yade/Factory \
              ../../../toolboxes/ComputationalGeometry/Distances \
              ../../../toolboxes/Math \
              ../../../toolboxes/Libraries/Serialization 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../plugins/Body/MassSpringBody/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/RigidBody/$(YADEDYNLIBPATH) \
               ../../../toolboxes/ComputationalGeometry/Distances/$(YADEDYNLIBPATH) \
               ../../../plugins/DynamicEngine/SimpleSpringDynamicEngine/$(YADEDYNLIBPATH) \
               ../../../plugins/GeometricalModel/Mesh2D/$(YADEDYNLIBPATH) \
               ../../../plugins/InteractionModel/ClosestFeatures/$(YADEDYNLIBPATH) \
               ../../../toolboxes/Libraries/Serialization/$(YADEDYNLIBPATH) \
               $(YADEDYNLIBPATH) 
DESTDIR = $(YADEDYNLIBPATH) 
CONFIG += debug \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += MassSpringBody2RigidBodyDynamicEngine.hpp 
SOURCES += MassSpringBody2RigidBodyDynamicEngine.cpp 
