# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./preprocessors/FileGenerator/BoxStack
# Target is a library:  

LIBS += -lSerialization \
        -lSAPCollider \
        -lBox \
        -lSphere \
        -lAABB \
        -lSimpleSpringDynamicEngine \
        -lMath \
        -lBody \
        -lEngine \
        -lInteraction \
        -lMultiMethods \
        -lRigidBodyParameters \
        -lInteractionSphere \
        -lInteractionBox \
        -lActionReset \
        -lInteractionDescriptionSet2AABBFunctor \
        -lTranslator \
        -lActionMomentumDamping \
        -lActionForceDamping \
        -rdynamic 
INCLUDEPATH = $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../toolboxes/Libraries/Serialization/$(YADEDYNLIBPATH) \
               ../../../plugins/Interaction/BroadInteractor/SAPCollider/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/GeometricalModel/Box/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/GeometricalModel/Sphere/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/BoundingVolume/AABB/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/DynamicEngine/SimpleSpringDynamicEngine/$(YADEDYNLIBPATH) \
               ../../../toolboxes/Libraries/Math/$(YADEDYNLIBPATH) \
               ../../../yade/Body/Body/$(YADEDYNLIBPATH) \
               ../../../yade/Engine/$(YADEDYNLIBPATH) \
               ../../../yade/Interaction/Interaction/$(YADEDYNLIBPATH) \
               ../../../toolboxes/Libraries/MultiMethods/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/BodyPhysicalParameters/RigidBodyParameters/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/InteractionDescription/InteractionSphere/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/InteractionDescription/InteractionBox/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/Action/ActionReset/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/BoundingVolumeFunctor/InteractionDescriptionSet2AABBFunctor/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/KinematicEngine/Translator/$(YADEDYNLIBPATH) \
               ../../../plugins/Geometry/GeometricalModel/Box/$(YADEDYNLIBPATH) \
               ../../../plugins/Geometry/GeometricalModel/Sphere/$(YADEDYNLIBPATH) \
               ../../../plugins/Geometry/BoundingVolume/AABB/$(YADEDYNLIBPATH) \
               ../../../yade/Body/$(YADEDYNLIBPATH) \
               ../../../yade/Geometry/$(YADEDYNLIBPATH) \
               ../../../yade/Interaction/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/BodyPhysicalParameters/RigidBody/$(YADEDYNLIBPATH) \
               ../../../plugins/Body/RigidBody/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/DynamicEngine/BallisticDynamicEngine/$(YADEDYNLIBPATH) \
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
HEADERS += BoxStack.hpp 
SOURCES += BoxStack.cpp 
