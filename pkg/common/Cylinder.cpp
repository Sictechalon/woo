#include "Cylinder.hpp"
#include<yade/pkg-common/Sphere.hpp>
#ifdef YADE_OPENGL
	#include<yade/lib-opengl/OpenGLWrapper.hpp>
#endif
#include<yade/pkg-common/Aabb.hpp>

Cylinder::~Cylinder(){}
ChainedCylinder::~ChainedCylinder(){}
ChainedState::~ChainedState(){}
CylScGeom::~CylScGeom(){}


YADE_PLUGIN(
	(Cylinder)(ChainedCylinder)(ChainedState)(CylScGeom)(Ig2_Sphere_ChainedCylinder_CylScGeom)(Ig2_ChainedCylinder_ChainedCylinder_ScGeom)
	#ifdef YADE_OPENGL
		(Gl1_Cylinder)(Gl1_ChainedCylinder)
	#endif
	(Bo1_Cylinder_Aabb)(Bo1_ChainedCylinder_Aabb)
);

vector<vector<int> > ChainedState::chains;
unsigned int ChainedState::currentChain=0;

//!##################	IG FUNCTORS   #####################


//!Sphere-cylinder or cylinder-cylinder not implemented yet, see Ig2_ChainedCylinder_ChainedCylinder_ScGeom and test/chained-cylinder-spring.py
#ifdef YADE_DEVIRT_FUNCTORS
bool Ig2_Sphere_ChainedCylinder_CylScGeom::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){ throw runtime_error("Do not call Ig2_Sphere_ChainedCylinder_CylScGeom::go, use getStaticFunctorPtr and call that function instead."); }
bool Ig2_Sphere_ChainedCylinder_CylScGeom::goStatic(IGeomFunctor* _self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
	const Ig2_Sphere_ChainedCylinder_CylScGeom* self=static_cast<Ig2_Sphere_ChainedCylinder_CylScGeom*>(_self);
	const Real& interactionDetectionFactor=self->interactionDetectionFactor;
#else
bool Ig2_Sphere_ChainedCylinder_CylScGeom::go(	const shared_ptr<Shape>& cm1,
							const shared_ptr<Shape>& cm2,
							const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
							const shared_ptr<Interaction>& c)
{
#endif
// 	cerr<<"Ig2_Sphere_ChainedCylinder_CylScGeom::go"<<endl;
	const State* sphereSt=YADE_CAST<const State*>(&state1);
	const ChainedState* cylinderSt=YADE_CAST<const ChainedState*>(&state2);
	ChainedCylinder *cylinder=YADE_CAST<ChainedCylinder*>(cm2.get());
	Sphere *sphere=YADE_CAST<Sphere*>(cm1.get());
	assert(sphereSt && cylinderSt && cylinder && sphere);
	if (cylinderSt->chains[cylinderSt->chainNumber].size()==(cylinderSt->rank+1)) {cerr << "last cylinder - ignored"<<endl; return false;}

// 	int i1=cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
// 	cerr << "i1 : "<<i1<< " i2: "<<cylinderSt->rank<<" pos: "<<Body::byId(cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1],scene)->state->pos<< " - "<<cylinderSt->pos<<endl;

	//FIXME : definition of segment in next line breaks periodicity
	Vector3r segment = Body::byId(cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1],scene)->state->pos-cylinderSt->pos;
	Vector3r branch = sphereSt->pos-cylinderSt->pos-shift2;
	bool isNew = !c->geom;

	//Check position of projection on cylinder axis
// 	if ((segment.dot(branch)>(segment.dot(segment)/*+interactionDetectionFactor*cylinder->radius*/) && !c->isReal())) return (false);//position _after_ end of cylinder
	if ((segment.dot(branch)>(segment.dot(segment)/*+interactionDetectionFactor*cylinder->radius*/) && isNew)) return (false);//position _after_ end of cylinder
// 	if ((segment.dot(branch)>(segment.dot(segment)/*+interactionDetectionFactor*cylinder->radius*/) && c->isReal())) cerr<<"pb1"<<endl;

	Real length = segment.norm();
	Vector3r direction = segment/length;
	Real dist = direction.dot(branch);
// 	if ((dist<-interactionDetectionFactor*cylinder->radius) && !c->isReal()) return (false);//position _before_ start of cylinder
	if ((dist<-interactionDetectionFactor*cylinder->radius) && isNew) return (false);//position _before_ start of cylinder
		
	//Check sphere-cylinder distance
	Vector3r projectedP = cylinderSt->pos+shift2 + direction*dist;
	branch = projectedP-sphereSt->pos;
// 	if ((branch.squaredNorm()>(pow(sphere->radius+cylinder->radius,2))) && !c->isReal()) return (false);
	if ((branch.squaredNorm()>(pow(sphere->radius+cylinder->radius,2))) && isNew) return (false);

	shared_ptr<CylScGeom> scm;
	if(!isNew) scm=YADE_PTR_CAST<CylScGeom>(c->geom);
	else { scm=shared_ptr<CylScGeom>(new CylScGeom()); c->geom=scm; }
	
	scm->radius1=sphere->radius;
	scm->radius2=cylinder->radius;
	scm->id3=cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
	scm->start=cylinderSt->pos+shift2; scm->end=scm->start+segment;

	//FIXME : there should be other checks without distanceFactor?
	if (dist<0.0) {//We have sphere-node contact
		Vector3r normal=(cylinderSt->pos+shift2)-sphereSt->pos;
		Real norm=normal.norm();
		scm->onNode=true; scm->relPos=0;
		scm->penetrationDepth=sphere->radius+cylinder->radius-norm;
		scm->contactPoint=sphereSt->pos+(sphere->radius-0.5*scm->penetrationDepth)*normal;
		scm->precompute(state1,state2,scene,c,normal/norm,isNew,shift2,true);//use sphere-sphere precompute (a node is a sphere)
	} else {//we have sphere-cylinder contact
		scm->onNode=false; scm->relPos=dist/length;
		Real norm=branch.norm();
		Vector3r normal=branch/norm;
		scm->penetrationDepth= sphere->radius+cylinder->radius-norm;
		if (dist>length) {
			scm->penetrationDepth=sphere->radius+cylinder->radius-(cylinderSt->pos+segment-sphereSt->pos).norm();
			//FIXME : handle contact jump on next element
		}
		scm->contactPoint = sphereSt->pos+scm->normal*(sphere->radius-0.5*scm->penetrationDepth);

		//FIXME : replace the block below with smart use of shift2=shift2 + cylinder_spin.cross(branch) - doesn't compile currently

		//precompute stuff manually (sphere-sphere precompute doesn't apply here if we want to avoid ratcheting
		/*
		if(!isNew) {
			scm->orthonormal_axis = scm->normal.cross(normal);
			Real angle = scene->dt*0.5*scm->normal.dot(sphereSt->angVel + cylinderSt->angVel);
			scm->twist_axis = angle*normal;}
		else scm->twist_axis=scm->orthonormal_axis=Vector3r::Zero();
		//Update contact normal
		scm->normal=normal;
		Vector3r c1x =  sphere->radius*normal;
		Vector3r c2x = -cylinder->radius*normal;
		Vector3r relativeVelocity = (cylinderSt->vel+cylinderSt->angVel.cross(c2x)) - (sphereSt->vel+sphereSt->angVel.cross(c1x));
		//keep the shear part only
		relativeVelocity = relativeVelocity-normal.dot(relativeVelocity)*normal;
		scm->shearInc = relativeVelocity*scene->dt;
		*/

	}
	return true;
}


bool Ig2_Sphere_ChainedCylinder_CylScGeom::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
 	cerr<<"Ig2_Sphere_ChainedCylinder_CylScGeom::goReverse"<<endl;
	c->swapOrder();
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}


#ifdef YADE_DEVIRT_FUNCTORS
bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){ throw runtime_error("Do not call Ig2_Sphere_ChainedCylinder_CylScGeom::go, use getStaticFunctorPtr and call that function instead."); }
bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom::goStatic(IGeomFunctor* _self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
	const Ig2_ChainedCylinder_ChainedCylinder_ScGeom* self=static_cast<Ig2_ChainedCylinder_ChainedCylinder_ScGeom*>(_self);
	const Real& interactionDetectionFactor=self->interactionDetectionFactor;
#else
bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom::go(	const shared_ptr<Shape>& cm1,
							const shared_ptr<Shape>& cm2,
							const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
							const shared_ptr<Interaction>& c)
{
#endif
	const ChainedState *pChain1, *pChain2;
	pChain1=YADE_CAST<const ChainedState*>(&state1);
	pChain2=YADE_CAST<const ChainedState*>(&state2);
	if (!pChain1 || !pChain2) {
		cerr <<"cast failed8567"<<endl;
	}
	const bool revert = ((int) pChain2->rank- (int) pChain1->rank == -1);
	const ChainedState& bchain1 = revert? *pChain2 : *YADE_CAST<const ChainedState*>(&state1);
	const ChainedState& bchain2 = revert? *pChain1 : *pChain2;
	if (bchain2.rank-bchain1.rank != 1) {/*cerr<<"Mutual contacts in same chain between not adjacent elements, not handled*/ return false;}
	if (pChain2->chainNumber!=pChain1->chainNumber) {cerr<<"PROBLEM0124"<<endl; return false;}
	
	ChainedCylinder *bs1=static_cast<ChainedCylinder*>(revert? cm2.get():cm1.get());
	
	shared_ptr<ScGeom6D> scm;
	bool isNew = !c->geom;
	if(!isNew) scm=YADE_PTR_CAST<ScGeom6D>(c->geom);
	else { scm=shared_ptr<ScGeom6D>(new ScGeom6D()); c->geom=scm; }
	Real length=(bchain2.pos-bchain1.pos).norm();
	Vector3r segt =pChain2->pos-pChain1->pos;
	if(isNew) {/*scm->normal=scm->prevNormal=segt/length;*/bs1->initLength=length;}
	scm->radius1=revert ? 0:bs1->initLength;
	scm->radius2=revert ? bs1->initLength:0;
	scm->penetrationDepth=bs1->initLength-length;
	scm->contactPoint=bchain2.pos;
	//bs1->segment used for fast BBs and projections + display
	bs1->segment= bchain2.pos-bchain1.pos;
#ifdef YADE_OPENGL 
	//bs1->length and s1->chainedOrientation used for display only, 
	bs1->length=length;
	bs1->chainedOrientation.setFromTwoVectors(Vector3r::UnitZ(),bchain1.ori.conjugate()*segt);
#endif
	scm->precompute(state1,state2,scene,c,segt/length,isNew,shift2,true);
	scm->precomputeRotations(state1,state2,isNew,false);
	//Set values that will be considered in Ip2 functor, geometry (precomputed) is really defined with values above
	scm->radius1 = scm->radius2 = bs1->initLength*0.5;
	return true;
}

bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}

#ifdef YADE_OPENGL
//!##################	RENDERING   #####################

bool Gl1_Cylinder::wire;
bool Gl1_Cylinder::glutNormalize;
int  Gl1_Cylinder::glutSlices;
int  Gl1_Cylinder::glutStacks;
int Gl1_Cylinder::glCylinderList=-1;

void Gl1_Cylinder::out( Quaternionr q )
{
	AngleAxisr aa(q);
	std::cout << " axis: " <<  aa.axis()[0] << " " << aa.axis()[1] << " " << aa.axis()[2] << ", angle: " << aa.angle() << " | ";
}

void Gl1_Cylinder::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<Cylinder*>(cm.get()))->radius;
	Real length=(static_cast<Cylinder*>(cm.get()))->length;
	//glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->color[0],cm->color[1],cm->color[2]));
	glColor3v(cm->color);
	if(glutNormalize) glPushAttrib(GL_NORMALIZE);
// 	glPushMatrix();
	Quaternionr shift = (static_cast<ChainedCylinder*>(cm.get()))->chainedOrientation;
	if (wire || wire2) drawCylinder(true, r,length,shift);
	else drawCylinder(false, r,length,shift);
	if(glutNormalize) glPopAttrib();
// 	glPopMatrix();
	return;
}

void Gl1_ChainedCylinder::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& state,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<ChainedCylinder*>(cm.get()))->radius;
	Real length=(static_cast<ChainedCylinder*>(cm.get()))->length;
	Quaternionr shift;// = (static_cast<ChainedCylinder*>(cm.get()))->chainedOrientation;
	shift.setFromTwoVectors(Vector3r::UnitZ(),state->ori.conjugate()*(static_cast<ChainedCylinder*>(cm.get()))->segment);
	glColor3v(cm->color);
	if(glutNormalize) glPushAttrib(GL_NORMALIZE);
	if (wire || wire2) drawCylinder(true, r,length,shift);
	else drawCylinder(false, r,length,shift);
	if(glutNormalize) glPopAttrib();
	return;
}

void Gl1_Cylinder::drawCylinder(bool wire, Real radius, Real length, const Quaternionr& shift) const
{
//    GLERROR;
/*	if (glCylinderList<0) {
		glCylinderList = glGenLists(1);
		glNewList(glCylinderList,GL_COMPILE);*/
   glPushMatrix();
   GLUquadricObj *quadObj = gluNewQuadric();
   gluQuadricDrawStyle(quadObj, (GLenum) (wire ? GLU_SILHOUETTE : GLU_FILL));
   gluQuadricNormals(quadObj, (GLenum) GLU_SMOOTH);
   gluQuadricOrientation(quadObj, (GLenum) GLU_OUTSIDE);
//     glTranslatef(0.0,0.0,-length*0.5);
   //scaling needs to adapt spheres or they will be elipsoids. They actually seem to disappear when commented glList code is uncommented, the cylinders are displayed correclty.
//    glScalef(1,length,1);
   AngleAxisr aa(shift);
   glRotatef(aa.angle()*180.0/Mathr::PI,aa.axis()[0],aa.axis()[1],aa.axis()[2]);
   gluCylinder(quadObj, radius, radius, length, glutSlices,glutStacks);
   gluQuadricOrientation(quadObj, (GLenum) GLU_INSIDE);
   glutSolidSphere(radius,glutSlices,glutStacks);
   glTranslatef(0.0,0.0,length);

   glutSolidSphere(radius,glutSlices,glutStacks);
//    gluDisk(quadObj,0.0,radius,glutSlices,_loops);
   gluDeleteQuadric(quadObj);
   glPopMatrix();
//    GLERROR;

// 	glEndList();}
// 	glCallList(glCylinderList);

}

// void Gl1_Cylinder::drawCylinder(bool wire, Real radius, Real length, const Quaternionr& shift) const
// {
// //    GLERROR;
// /*	if (glCylinderList<0) {
// 		glCylinderList = glGenLists(1);
// 		glNewList(glCylinderList,GL_COMPILE);*/
//    glPushMatrix();
//    GLUquadricObj *quadObj = gluNewQuadric();
//    gluQuadricDrawStyle(quadObj, (GLenum) (wire ? GLU_SILHOUETTE : GLU_FILL));
//    gluQuadricNormals(quadObj, (GLenum) GLU_SMOOTH);
//    gluQuadricOrientation(quadObj, (GLenum) GLU_OUTSIDE);
// //     glTranslatef(0.0,0.0,-length*0.5);
//    //scaling needs to adapt spheres or they will be elipsoids. They actually seem to disappear when commented glList code is uncommented, the cylinders are displayed correclty.
// //    glScalef(1,length,1);
//    gluCylinder(quadObj, radius, radius, length, glutSlices,glutStacks);
//    gluQuadricOrientation(quadObj, (GLenum) GLU_INSIDE);
//    glutSolidSphere(radius,glutSlices,glutStacks);
//    glTranslatef(0.0,0.0,length);
//    AngleAxisr aa(shift);
//    glRotatef(aa.angle(),aa.axis()[0],aa.axis()[1],aa.axis()[2]);
//    glutSolidSphere(radius,glutSlices,glutStacks);
// //    gluDisk(quadObj,0.0,radius,glutSlices,_loops);
//    gluDeleteQuadric(quadObj);
//    glPopMatrix();
// //    GLERROR;
// 	
// // 	glEndList();}
// // 	glCallList(glCylinderList);
// 
// }

//!##################	BOUNDS FUNCTOR   #####################

#endif

void Bo1_Cylinder_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Cylinder* cylinder = static_cast<Cylinder*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	if(!scene->isPeriodic){
		const Vector3r& O = se3.position;
		Vector3r O2 = se3.position+se3.orientation*cylinder->segment;
		aabb->min=aabb->max=O;
		for (int k=0;k<3;k++){
			aabb->min[k]=min(aabb->min[k],min(O[k],O2[k])-cylinder->radius);
			aabb->max[k]=max(aabb->max[k],max(O[k],O2[k])+cylinder->radius);
		}
		return;
	}
	// adjust box size along axes so that cylinder doesn't stick out of the box even if sheared (i.e. parallelepiped)
// 	if(scene->cell->hasShear()) {
// 		Vector3r refHalfSize(minkSize);
// 		const Vector3r& cos=scene->cell->getCos();
// 		for(int i=0; i<3; i++){
// 			//cerr<<"cos["<<i<<"]"<<cos[i]<<" ";
// 			int i1=(i+1)%3,i2=(i+2)%3;
// 			minkSize[i1]+=.5*refHalfSize[i1]*(1/cos[i]-1);
// 			minkSize[i2]+=.5*refHalfSize[i2]*(1/cos[i]-1);
// 		}
// 	}
// 	//cerr<<" || "<<halfSize<<endl;
// 	aabb->min = scene->cell->unshearPt(se3.position)-minkSize;
// 	aabb->max = scene->cell->unshearPt(se3.position)+minkSize;
}


void Bo1_ChainedCylinder_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Cylinder* cylinder = static_cast<Cylinder*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	if(!scene->isPeriodic){
		const Vector3r& O = se3.position;
		Vector3r O2 = se3.position+cylinder->segment;
		aabb->min=aabb->max=O;
		for (int k=0;k<3;k++){
			aabb->min[k]=min(aabb->min[k],min(O[k],O2[k])-cylinder->radius);
			aabb->max[k]=max(aabb->max[k],max(O[k],O2[k])+cylinder->radius);
		}
		return;
	}
}
