# encoding: utf-8

from woo.dem import *
import woo.core
import woo.dem
import woo.pyderived
import woo
import math
from minieigen import *
import numpy

class CylTriaxTest(woo.core.Preprocessor,woo.pyderived.PyWooObject):
	'Preprocessor for cylindrical triaxial test'
	_classTraits=None
	_PAT=woo.pyderived.PyAttrTrait # less typing
	_attrTraits=[
		# noGui would make startGroup being ignored
		_PAT(float,'isoStress',-1e4,unit='kPa',startGroup='General',doc='Confining stress (isotropic during compaction)'),
		_PAT(Vector3,'maxRates',(2e-1,5e-2,1.),'Maximum strain rates during the compaction phase (for all axes), and during the triaxial phase in axial sense and radial sense.'),
		_PAT(float,'massFactor',.5,'Multiply real mass of particles by this number to obtain the :obj:`woo.dem.WeirdTriaxControl.mass` control parameter'),
		_PAT([Vector2,],'psd',[(2e-3,0),(2.5e-3,.2),(4e-3,1.)],unit=['mm','%'],doc='Particle size distribution of particles; first value is diameter, scond is cummulative mass fraction.'),
		_PAT(float,'sigIso',-500e3,unit='Pa',doc='Isotropic compaction stress, and lateral stress during the triaxial phase'),
		_PAT(float,'stopStrain',-.2,doc='Goal value of axial deformation in the triaxial phase'),

		_PAT(Vector2,'radHt',Vector2(.02,.05),doc='Initial size of the cylinder (radius and height)'),

		_PAT(woo.dem.FrictMat,'parMat',FrictMat(young=0.3e9,ktDivKn=.2,tanPhi=.4,density=1e8),'Material of particles.'),
		_PAT(woo.dem.FrictMat,'memMat',FrictMat(young=1.1e6,ktDivKn=.2,tanPhi=.4,density=1e8),'Membrane material; if unspecified, particle material is used (with reduced friction during the compaction phase)'),
		_PAT(float,'suppTanPhi',float('nan'),'Friction at supports; if NaN, the same as for particles is used. Supports use the same material as particles otherwise.'),
		_PAT(float,'memThick',-1.0,'Membrane thickness; if negative, relative to largest particle diameter'),

		#	_PAT(woo.dem.ElastMat,'circMat',woo.dem.ElastMat(young=1e3,density=1.),'Material for circumferential trusses (simulating the membrane). If *None*, membrane will not be simulated. The membrane is only added after the compression phase.'),
		#_PAT(float,'circAvgThick',-.001,'Average thickness of circumferential membrane; if negative, relative to cylinder radius (``radHt[0]``).'),

		_PAT(str,'reportFmt',"/tmp/{tid}.xhtml",startGroup="Outputs",doc="Report output format; :obj:`Scene.tags <woo.core.Scene.tags>` can be used."),
		_PAT(str,'packCacheDir',".","Directory where to store pre-generated feed packings; if empty, packing wil be re-generated every time."),
		#_PAT(str,'saveFmt',"/tmp/{tid}-{stage}.bin.gz",'''Savefile format; keys are :obj:`Scene.tags <woo.core.Scene.tags>`; additionally ``{stage}`` will be replaced by
		#* ``init`` for stress-free but compact cloud,
		#* ``iso`` after isotropic compaction,
		#* ``backup-011234`` for regular backups, see :obj:`backupSaveTime`,
		# 'done' at the very end.
		#'''),
		#_PAT(int,'backupSaveTime',1800,doc='How often to save backup of the simulation (0 or negative to disable)'),
		_PAT(float,'pWaveSafety',.1,startGroup='Tunables',doc='Safety factor for :obj:`woo.utils.pWaveDt` estimation.'),
		_PAT(float,'cylDiv',40,'Number of segments for cylinder (first component)'),
		_PAT(float,'damping',.5,'Nonviscous damping'),
		_PAT(float,'maxUnbalanced',.05,'Maximum unbalanced force at the end of compaction'),
		_PAT(int,'vtkStep',0,'Periodicity of saving VTK exports'),
		_PAT(str,'vtkFmt','/tmp/{title}.{id}-','Prefix for VTK exports')
	]
	def __init__(self,**kw):
		woo.core.Preprocessor.__init__(self)
		self.wooPyInit(self.__class__,woo.core.Preprocessor,**kw)
	def __call__(self):
		# preprocessor builds the simulation when called
		return prepareCylTriax(self)


def mkFacetCyl(aabb,cylDiv,suppMat,sideMat,suppMask,sideMask,suppBlock,sideBlock,sideThick,mass,inertia):
	'Make closed cylinder from facets. Z is axis of the cylinder. The position is determined by aabb; the cylinder may be elliptical, if the x and y dimensions are different. Return list of particles and list of nodes. The first two nodes in the list are bottom central node and top central node. cylDiv is tuple specifying division in circumferential and axial direcrtion respectively.'
	r1,r2=.5*aabb.sizes()[0],.5*aabb.sizes()[1]
	C=aabb.center()
	zMin,zMax=aabb.min[2],aabb.max[2]

	centrals=[woo.core.Node(pos=Vector3(C[0],C[1],zMin)),woo.core.Node(pos=Vector3(C[0],C[1],zMax))]
	for c in centrals:
		c.dem=woo.dem.DemData()
		c.dem.mass=mass
		c.dem.inertia=inertia
		c.dem.blocked='xyzXYZ'

	retParticles=[]

	# nodes on the perimeter
	thetas=numpy.linspace(2*math.pi,0,num=cylDiv[0],endpoint=False)
	xxyy=[Vector2(r1*math.cos(th)+C[0],r2*math.sin(th)+C[1]) for th in thetas]
	zz=numpy.linspace(zMin,zMax,num=cylDiv[1],endpoint=True)
	nnn=[[woo.core.Node(pos=Vector3(xy[0],xy[1],z)) for xy in xxyy] for z in zz]
	for i,nn in enumerate(nnn):
		if i==0 or i==(len(nnn)-1): blocked=suppBlock
		else: blocked=sideBlock
		for n in nn:
			n.dem=woo.dem.DemData()
			n.dem.mass=mass
			n.dem.inertia=inertia
			n.dem.blocked=blocked
	def mkCap(nn,central,mask,mat):
		ret=[]
		for i in range(len(nn)):
			ret.append(woo.dem.Particle(material=mat,shape=Facet(nodes=[nn[i],nn[(i+1)%len(nn)],central]),mask=mask))
			nn[i].dem.parCount+=1
			nn[(i+1)%len(nn)].dem.parCount+=1
			central.dem.parCount+=1
		return ret
	retParticles+=mkCap(nnn[0],central=centrals[0],mask=suppMask,mat=suppMat)
	retParticles+=mkCap(list(reversed(nnn[-1])),central=centrals[-1],mask=suppMask,mat=suppMat) # reverse to have normals outside
	def mkAround(nnAC,nnBD,mask,mat,halfThick):
		ret=[]
		for i in range(len(nnAC)):
			A,B,C,D=nnAC[i],nnBD[i],nnAC[(i+1)%len(nnAC)],nnBD[(i+1)%len(nnBD)]
			ret+=[woo.dem.Particle(material=mat,shape=FlexFacet(nodes=fNodes,halfThick=halfThick),mask=mask) for fNodes in ((A,B,D),(A,D,C))]
			A.dem.parCount+=2
			D.dem.parCount+=2
			B.dem.parCount+=1
			C.dem.parCount+=1
		return ret
	for i in range(0,len(nnn)-1):
		retParticles+=mkAround(nnn[i],nnn[i+1],mask=sideMask,mat=sideMat,halfThick=.5*sideThick)
	for p in retParticles: p.shape.wire=True
	import itertools
	return retParticles,centrals+list(itertools.chain.from_iterable(nnn))

def prepareCylTriax(pre):
	import woo
	margin=.6
	rad,ht=pre.radHt[0],pre.radHt[1]
	bot,top=margin*ht,(1+margin)*ht
	xymin=Vector2(margin*rad,margin*rad)
	xymax=Vector2((margin+2)*rad,(margin+2)*rad)
	xydomain=Vector2((2*margin+2)*rad,(2*margin+2)*rad)
	xymid=.5*xydomain
	S=woo.core.Scene(fields=[DemField()])
	S.pre=pre.deepcopy()
	S.periodic=True
	S.cell.setBox(xydomain[0],xydomain[1],(2*margin+1)*ht)

	meshMask=0b0011
	spheMask=0b0001
	loneMask=0b0010
	S.dem.loneMask=loneMask

	# save materials for later manipulation
	S.lab.parMat=pre.parMat
	S.lab.memMat=(pre.memMat if pre.memMat else pre.parMat.deepcopy())
	S.lab.suppMat=pre.parMat.deepcopy()
	S.lab.suppMat.tanPhi=pre.suppTanPhi

	## generate particles inside cylinder
	# radius minus polygonal imprecision (circle segment), minus halfThickness of the membrane
	if pre.memThick<0: pre.memThick*=-pre.psd[-1][0]
	innerRad=rad-rad*(1.-math.cos(.5*2*math.pi/pre.cylDiv))-.5*pre.memThick

	if pre.packCacheDir:
		import hashlib,os
		compactMemoize=pre.packCacheDir+'/'+hashlib.sha1(pre.dumps(format='expr')+'ver2').hexdigest()+'.triax-compact'
		print 'Compaction memoize file is ',compactMemoize
	if pre.packCacheDir and os.path.exists(compactMemoize):
		print 'Using memoized compact state'
		sp=woo.pack.SpherePack()
		sp.load(compactMemoize)
		meshAabb=eval(sp.userData)
		S.lab.compactMemoize=None
		sp.toSimulation(S,mat=S.lab.parMat)
	else:
		S.dem.par.append(woo.pack.randomLoosePsd(predicate=woo.pack.inCylinder(centerBottom=(xymid[0],xymid[1],bot),centerTop=(xymid[0],xymid[1],top),radius=innerRad),psd=pre.psd,mat=S.lab.parMat))
		meshAabb=AlignedBox3((xymin[0],xymin[1],bot),(xymax[0],xymax[1],top))
		S.lab.compactMemoize=compactMemoize

	sumParMass=sum([p.mass for p in S.dem.par])

	# create mesh (supports+membrane)
	cylDivHt=int(round(pre.radHt[1]/(2*math.pi*pre.radHt[0]/pre.cylDiv))) # try to be as square as possible
	nodeMass=(pre.radHt[1]/cylDivHt)**2*pre.memThick*pre.memMat.density # approx mass of square slab of our size
	nodeInertia=((3/4.)*(nodeMass/math.pi))**(5/3.)*(6/15.)*math.pi # inertial of sphere with the same mass
	particles,nodes=mkFacetCyl(
		aabb=meshAabb,
		cylDiv=(pre.cylDiv,cylDivHt),
		suppMask=meshMask,sideMask=meshMask,
		sideBlock='xyzXYZ',suppBlock='xyzXYZ',
		mass=nodeMass,inertia=nodeInertia*Vector3(1,1,1),
		suppMat=S.lab.suppMat,sideMat=S.lab.memMat,
		sideThick=pre.memThick,
	)
	S.lab.cylNodes=nodes
	S.dem.par.append(particles)

	##
	# collect nodes from both facets and spheres
	S.dem.collectNodes() 

	S.dt=pre.pWaveSafety*woo.utils.pWaveDt(S)
	# setup engines
	S.engines=[
		InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],verletDist=-.05),
		ContactLoop(
			[Cg2_Sphere_Sphere_L6Geom(),Cg2_Facet_Sphere_L6Geom()],
			[Cp2_FrictMat_FrictPhys()],
			[Law2_L6Geom_FrictPhys_IdealElPl(noFrict=True,label='contactLaw')],
			applyForces=True,label='contactLoop'
		), 
		IntraForce([
				In2_Sphere_ElastMat(),
				In2_FlexFacet_ElastMat(bending=True)],
			label='intraForce',dead=True # ContactLoop applies forces during compaction
		), 
		WeirdTriaxControl(goal=(pre.sigIso,pre.sigIso,pre.sigIso),maxStrainRate=(pre.maxRates[0],pre.maxRates[0],pre.maxRates[0]),relVol=math.pi*innerRad**2*ht/S.cell.volume,stressMask=0b0111,maxUnbalanced=pre.maxUnbalanced,mass=pre.massFactor*sumParMass,doneHook='import woo.pre.cylTriax; woo.pre.cylTriax.compactionDone(S)',label='triax',absStressTol=1e4,relStressTol=1e-2),
		# run the same as addPlotData
		MeshVolume(mask=S.dem.loneMask,stepPeriod=20,label='meshVolume',dead=False),  
		woo.core.PyRunner(20,'import woo.pre.cylTriax; woo.pre.cylTriax.addPlotData(S)'),
		VtkExport(out=pre.vtkFmt,stepPeriod=pre.vtkStep,what=VtkExport.all,dead=True,label='vtk'),
		Leapfrog(damping=pre.damping,reset=True),
	]

	S.lab.stage='compact'

	## if spheres were loaded externally, compaction is done just now
	##
	if S.lab.compactMemoize==None: compactionDone(S)

	try:
		import woo.gl
		S.any=[woo.gl.Renderer(dispScale=(5,5,2),rotScale=0,cell=False),woo.gl.Gl1_DemField(),woo.gl.Gl1_CPhys(),woo.gl.Gl1_FlexFacet(phiSplit=False,phiWd=1,relPhi=0.,uScale=0.,slices=-1),woo.gl.Gl1_Facet(wd=2,slices=-1)]
	except ImportError: pass

	return S

def addPlotData(S):
	assert S.lab.stage in ('compact','stabilize','triax')
	import woo
	t=S.lab.triax
	sxx,syy,szz=t.stress.diagonal()
	p=t.stress.diagonal().sum()/3. # mean stress
	q=szz-.5*(sxx+syy)         # deviatoric stress?!
	qDivP=(q/p if p!=0 else float('nan'))
	vol=S.lab.meshVolume.netVol
	if S.lab.stage in ('compact','stabilize'):
		exx,eyy,ezz=t.strain
		err=.5*(exx+eyy)
		eVol=float('nan')
	else:
		if not hasattr(S.lab,'netVol0'): S.lab.netVol0=S.lab.meshVolume.netVol
		ezz=t.strain[2] # xy components irrelevant
		eVol=math.log(vol/S.lab.netVol0)
		err=.5*(eVol-ezz)
		exx=eyy=float('nan') # undefined
	eDev=ezz-(1/3.)*(2*err+ezz) # FIXME: is this correct?!

	S.plot.addData(unbalanced=woo.utils.unbalancedForce(),i=S.step,
		sxx=sxx,syy=syy,srr=.5*(sxx+syy),szz=szz,
		exx=exx,eyy=eyy,err=err,ezz=ezz,
		eDev=eDev,eVol=eVol,
		vol=vol,
		p=p,q=q,qDivP=qDivP,
		isTriax=(1 if S.lab.stage=='triax' else 0), # to be able to filter data
		grossVol=S.lab.meshVolume.vol,
		# save all available energy data
		#Etot=O.energy.total()#,**O.energy
	)
	if not S.plot.plots:
		S.plot.plots={
			'i':('unbalanced',),'i ':('sxx','syy','srr','szz'),' i':('exx','eyy','err','ezz','eVol'),'  i':('vol','grossVol')
			# energy plot
			#' i ':(O.energy.keys,None,'Etot'),
		}
		S.plot.xylabels={'i ':('step','Stress [Pa]',),' i':('step','Strains [-]','Strains [-]')}
		S.plot.labels={
			'sxx':r'$\sigma_{xx}$','syy':r'$\sigma_{yy}$','szz':r'$\sigma_{zz}$','srr':r'$\sigma_{rr}$',
			'exx':r'$\varepsilon_{xx}$','eyy':r'$\varepsilon_{yy}$','ezz':r'$\varepsilon_{zz}$','err':r'$\varepsilon_{rr}$','eVol':r'$\varepsilon_{v}$','vol':'net volume','grossVol':'midplane volume'
		}
	## already in compaction phase
	## adjust surface load to maintain the stress tensor
	if not S.lab.meshVolume.dead:
		srr=.5*(sxx+syy) # current radial stress
		surfLoad=2*S.pre.sigIso-srr # (sIso-(srr-sIso))
		# print 'Changing surface load to ',surfLoad,', srr is',srr

		for p in S.dem.par:
			if isinstance(p.shape,FlexFacet): p.shape.surfLoad=-surfLoad
		#for n in S.dem.nodes:
		#	n.dem.angVel*=.99
	if S.lab.meshVolume.netVol>0:
		S.lab.triax.relVol=S.lab.meshVolume.netVol/S.cell.volume

def velocityFieldPlots(S,nameBase):
	import woo
	from woo import post2d
	flattener=post2d.CylinderFlatten(useRef=False,axis=2,origin=(.5*S.cell.size[0],.5*S.cell.size[1],(.6/2.2*S.cell.size[2])))
	#maxVel=float('inf') #5e-2
	#exVel=lambda p: p.vel if p.vel.norm()<=maxVel else p.vel/(p.vel.norm()/maxVel)
	exVel=lambda p: p.vel
	exVelNorm=lambda p: exVel(p).norm()
	import pylab
	fVRaw=pylab.figure()
	post2d.plot(post2d.data(S,exVel,flattener),alpha=.3,minlength=.3,cmap='jet')
	fV2=pylab.figure()
	post2d.plot(post2d.data(S,exVel,flattener,stDev=.5*S.pre.psd[0][0],div=(80,80)),minlength=.6,cmap='jet')
	fV1=pylab.figure()
	post2d.plot(post2d.data(S,exVelNorm,flattener,stDev=.5*S.pre.psd[0][0],div=(80,80)),cmap='jet')
	outs=[]
	for name,fig in [('particle-velocity',fVRaw),('smooth-velocity',fV2),('smooth-velocity-norm',fV1)]:
		out=nameBase+'.%s.png'%name
		fig.savefig(out)
		outs.append(out)
	return outs

def membraneStabilized(S):
	print 'Membrane stabilized at step',S.step
	S.lab.triax.goal=(0,0,S.pre.stopStrain)
	S.lab.triax.maxUnbalanced=10 # don't care, just compress until done
	S.lab.triax.doneHook='import woo.pre.cylTriax; woo.pre.cylTriax.triaxDone(S)'
	del S.lab.stage # avoid warning 
	S.lab.stage='triax'


def compactionDone(S):
	if S.lab.compactMemoize: print 'Compaction done at step',S.step
	import woo
	t=S.lab.triax
	# set the current cell configuration to be the reference one
	S.cell.trsf=Matrix3.Identity
	S.cell.refHSize=S.cell.hSize
	# for a while, do nothing, just wait for the membrane to stabilize
	t.maxUnbalanced=.5*S.pre.maxUnbalanced
	t.goal=(0,0,0)
	t.doneHook='import woo.pre.cylTriax; woo.pre.cylTriax.membraneStabilized(S)'
	t.stressMask=0b0000 # z is strain-controlled, x,y stress-controlled
	# allow faster deformation along x,y to better maintain stresses
	t.maxStrainRate=(0,0,S.pre.maxRates[1])
	# next time, call triaxFinished instead of compactionFinished
	# do not wait for stabilization before calling triaxFinished
	##
	try:
		import woo.gl
		woo.gl.Gl1_DemField.updateRefPos=True
	except ImportError: pass

	# restore friction
	S.lab.contactLaw.noFrict=False
	# reset contacts so that they pick up new friction angle (FIXME: this is perhaps not necessary now?!)
	for c in S.dem.con: c.resetPhys()

	# make the membrane flexible: apply force on the membrane
	S.lab.contactLoop.applyForces=False
	S.lab.intraForce.dead=False
	S.lab.meshVolume.dead=False
	S.lab.vtk.dead=(S.pre.vtkStep>0 and S.pre.vtkFmt!='')
	# free the nodes
	top,bot=S.lab.cylNodes[:2]
	tol=1e-3*abs(top.pos[2]-bot.pos[2])
	for n in S.lab.cylNodes[2:]:
		# supports may move in-plane, and also may rotate
		if abs(n.pos[2]-top.pos[2])<tol or abs(n.pos[2]-bot.pos[2])<tol:
			n.dem.blocked='z' ## FIXME: 'zXYZ'?
		else: n.dem.blocked='' # don't rotate unless we have bending at some point
	# add surface load
	for p in S.dem.par:
		if isinstance(p.shape,FlexFacet): p.shape.surfLoad=-S.pre.sigIso
	# set velocity to 0 (so that when loading packing, the conditions are the same)
	for n in S.dem.nodes: n.dem.vel=n.dem.angVel=Vector3.Zero

	if S.lab.compactMemoize: # if None, don't save
		S.save('/tmp/compact.gz')
		aabb=AlignedBox3()
		for n in S.lab.cylNodes: aabb.extend(n.pos)
		sp=woo.pack.SpherePack()
		sp.fromSimulation(S)
		sp.userData=str(aabb)
		sp.save(S.lab.compactMemoize)
		print 'Saved compacted packing to',S.lab.compactMemoize

	del S.lab.stage # avoid warning 
	S.lab.stage='stabilize'

	

def plotBatchResults(db):
	'Hook called from woo.batch.writeResults'
	import pylab,re,math,woo.batch,os
	results=woo.batch.dbReadResults(db)
	out='%s.pdf'%re.sub('\.results$','',db)

	from matplotlib.ticker import FuncFormatter
	kiloPascal=FuncFormatter(lambda x,pos=0: '%g'%(1e-3*x))
	percent=FuncFormatter(lambda x,pos=0: '%g'%(1e2*x))

	fig=pylab.figure(figsize=(8,20))
	ed_qp=fig.add_subplot(311)
	ed_qp.set_xlabel(r'$\varepsilon_d$ [%]')
	ed_qp.set_ylabel(r'$q/p$')
	ed_qp.xaxis.set_major_formatter(percent)
	ed_qp.grid(True)

	ed_ev=fig.add_subplot(312)
	ed_ev.set_xlabel(r'$\varepsilon_d$ [%]')
	ed_ev.set_ylabel(r'$\varepsilon_v$ [%]')
	ed_ev.xaxis.set_major_formatter(percent)
	ed_ev.yaxis.set_major_formatter(percent)
	ed_ev.grid(True)

	p_q=fig.add_subplot(313)
	p_q.set_xlabel(r'$p$ [kPa]')
	p_q.set_ylabel(r'$q$ [kPa]')
	p_q.xaxis.set_major_formatter(kiloPascal)
	p_q.yaxis.set_major_formatter(kiloPascal)
	p_q.grid(True)

	for res in results:
		series,pre=res['series'],res['pre']
		title=res['title'] if res['title'] else res['sceneId']
		isTriax=series['isTriax']
		# skip the very first number, since that's the transitioning step and strains are still at their old value
		ed=series['eDev'][isTriax==1][1:]
		ev=series['eVol'][isTriax==1][1:]
		p=series['p'][isTriax==1][1:]
		q=series['q'][isTriax==1][1:]
		qDivP=series['qDivP'][isTriax==1][1:]
		ed_qp.plot(ed,qDivP,label=title,alpha=.6)
		ed_ev.plot(ed,ev,label=title,alpha=.6)
		p_q.plot(p,q,label=title,alpha=.6)
	ed_qp.invert_xaxis()
	ed_ev.invert_xaxis()
	ed_ev.invert_yaxis()
	p_q.invert_xaxis()
	p_q.invert_yaxis()
	for ax,loc in (ed_qp,'lower right'),(ed_ev,'lower right'),(p_q,'upper left'):
		l=ax.legend(loc=loc,labelspacing=.2,prop={'size':7})
		l.get_frame().set_alpha(.4)
	fig.savefig(out)
	print 'Batch figure saved to file://%s'%os.path.abspath(out)


def triaxDone(S):
	print 'Triaxial done at step',S.step
	S.stop()
	import woo.utils
	(repName,figs)=woo.utils.xhtmlReport(S,S.pre.reportFmt,'Cylindrical triaxial test',afterHead='',figures=[(None,f) for f in S.plot.plot(noShow=True,subPlots=False)],svgEmbed=True,show=True)
	woo.batch.writeResults(defaultDb='cylTriax.results',series=S.plot.data,postHooks=[plotBatchResults],simulationName='horse',report='file://'+repName)


