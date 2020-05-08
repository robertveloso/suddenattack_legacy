#include "LTAFile.h"

LTAFile::LTAFile( bool detailBrushes, bool externalKeys )
{
	nodeAlloc = new CLTADefaultAlloc;

	this->externalKeys = externalKeys;
	numKeys = 0;

	this->detailBrushes = detailBrushes;

	root = new CLTANode;
	header = new CLTANode;
	polyhedronList = new CLTANode;
	nodeHierarchy = new CLTANode;
	globalPropList = new CLTANode;
	navigatorPosList = new CLTANode;
	nodeRoot = new CLTANode;
	polyRoot = new CLTANode;
	propRoot = new CLTANode;
	curPolyhedronRoot = NULL;

	keyRoot = NULL;
	keyList = NULL;

	// first container for brushes
	CLTANode* container = new CLTANode;

	// setup the node tree
	nodeList = new LTANodeList;
	nodeList->containerNode = nodeRoot;
	nodeList->next = NULL;
	nodeList->prev = NULL;

	// setup the property tree
	propList = NULL;

	// setup the polyhedron info
	curPolyhedron = 0;
	curNodeID = 3;		// first two are hardcoded below
	nextPropID = 1;		// only applies to brush export
	curPropID = 0;		// only applies to brush export
	curBrushID = 0;		// only applies to brush export

	// setup the base level of the tree
	A( root, N("world") );
	A( root, header );
	A( root, polyhedronList );
	A( root, nodeHierarchy );
	A( root, globalPropList );
	A( root, navigatorPosList );

	// setup the header tree
	A(
	A(
	header,
		N("header")
	),
		A(
		N(),
			A(
			N(),
				N("versioncode 1")
			)
		)
	);

	// setup the navigatorposlist tree
	A(
	A(
	navigatorPosList,
		N("navigatorposlist")
	),
		N()
	);

	// setup the polyhedronlist tree
	A(
	A(
	polyhedronList,
		N("polyhedronlist")
	),
		polyRoot	// new polyhedrons are appended to this node
	);

	// setup the nodehierarchy
	A(
	A(
	nodeHierarchy,
		N("nodehierarchy")
	),
		A(
		A(
		A(
		A(
		A(
		A(
		A(
		N(),
			N("worldnode")
		),
			A(
			N(),
				N("type null")
			)
		),
			A(
			N(),
				N("label \"Container\"")
			)
		),
			A(
			N(),
				N("nodeid 1")
			)
		),
			A(
			A(
			N(),
				N("flags")
			),
				A(
				N(),
					N("worldroot")
				)
			)
		),
			A(
			A(
			N(),
				N("properties")
			),
				A(
				N(),
					N("propid 0")
				)
			)
		),
			A(
			A(
			N(),
				N("childlist")
			),
				container
			)
		)
	);

	// setup the first container
	A(
	container,
		A(
		A(
		A(
		A(
		A(
		A(
		A(
		N(),
			N("worldnode")
		),
			A(
			N(),
				N("type null")
			)
		),
			A(
			N(),
				N("label \"Maya Brushes\"")
			)
		),
			A(
			N(),
				N("nodeid 2")
			)
		),
			A(
			A(
			N(),
				N("flags")
			),
				N()
			)
		),
			A(
			A(
			N(),
				N("properties")
			),
				A(
				N(),
					N("propid 0")
				)
			)
		),
			A(
			A(
			N(),
				N("childlist")
			),
				nodeRoot	// new nodes are appended to this
			)
		)
	);

	// setup the globalproplist
	A(
	A(
	globalPropList,
		N("globalproplist")
	),
		propRoot	// new properties are appended to this
	);

	// add the empty property used by containers
	A(
	propRoot,
		A(
		A(
		N(),
			N("proplist")
		),
			N()
		)
	);

	// setup external key file
	if( externalKeys )
	{
		keyRoot = new CLTANode;
		keyList = new CLTANode;

		A( keyRoot, N("keys") );
		A( keyRoot, keyList );
	}
}

LTAFile::~LTAFile()
{
	// recursively delete the tree
	nodeAlloc->FreeNode( root );

	if( externalKeys )
		nodeAlloc->FreeNode( keyRoot );

	// delete the property list
	PropertyList* curProp = propList;
	while( curProp )
	{
		PropertyList* delProp = curProp;
		curProp = delProp->next;
		delete delProp;
	}

	// delete the node list
	assert( nodeList->next == NULL );
	delete nodeList;

	// delete the memory allocator
	delete nodeAlloc;
}

// add a brush property to the tree if needed and set the curPropID
void LTAFile::AddProperty( const char* name )
{
	// search for an existing property with this name
	bool foundProp = false;
	PropertyList* curProp = propList;
	while( curProp )
	{
		if( curProp->isBrush && (curProp->name == MString(name)) )
		{
			assert( curProp->id > 0 );
			foundProp = true;
			break;
		}
		curProp = curProp->next;
	}

	// found an existing property with this name, set the id and return
	if( foundProp )
	{
		curPropID = curProp->id;
		return;
	}

	// add the property to the list
	PropertyList* newProp = new PropertyList;
	newProp->name = name;
	newProp->isBrush = true;
	newProp->id = nextPropID++;
	newProp->next = propList;
	propList = newProp;
	curPropID = newProp->id;

	// setup strings for the tree
	MString propName( "string \"Name\" () (data \"" );
	propName += name + MString( "\")" );

	MString sdetail( "bool \"Detail\" () (data " );
	sdetail += detailBrushes;
	sdetail += ")";

	// add the property to the property tree
	A(
	propRoot,
		A(
		A(
		N(),
			N("proplist")
		),
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			 A(
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			N(),
				A(
				N(),
					N(propName.asChar())
				)
			),
				A(
				N(),
					N("vector \"Pos\" ( distance ) (data (vector (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N("rotation \"Rotation\" () (data (eulerangles (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N("string \"Type\" (staticlist) (data \"Normal\")")
				)
			),
				A(
				N(),
					N("string \"Lighting\" (staticlist) (data \"Gouraud\")")
				)
			),
				A(
				N(),
					N("bool \"NoSnap\" () (data 1)")
				)
			),
				A(
				N(),
					N("bool \"NotAStep\" () (data 0)")
				)
			),
				A(
				N(),
					N(sdetail.asChar())
				)
			),
				A(
				N(),
					N("longint \"LightControl\" (groupowner group1) (data 0.000)")
				)
			),
				A(
				N(),
					N("string \"TextureEffect\" (textureeffect)")
				)
			),
				A(
				N(),
					N("color \"AmbientLight\" (group1) (data (vector (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N("longint \"LMGridSize\" (group1) (data 0.000)")
				)
			),
				A(
				N(),
					N("bool \"ClipLight\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("bool \"CastShadowMesh\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("bool \"ReceiveLight\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("bool \"ReceiveShadows\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("bool \"ReceiveSunlight\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("real \"LightPenScale\" (group1) (data 0.000)")
				)
			),
				A(
				N(),
					N("real \"CreaseAngle\" (group1) (data 45.000)")
				)
			)
		)
	);
}

void LTAFile::AddObject( const char* name, bool asBrush )
{
	// location in tree we are adding the container to
	CLTANode* curRoot = nodeList->containerNode;

	// create the root below this one
	CLTANode* nextRoot = new CLTANode;

	// update the lta node list
	LTANodeList* prevNode = nodeList;
	nodeList = new LTANodeList;
	prevNode->next = nodeList;
	nodeList->prev = prevNode;
	nodeList->next = NULL;
	nodeList->containerNode = nextRoot;

	if( asBrush )
	{
		// adding a new brush node
		MString nodeid( "nodeid " );
		nodeid += curNodeID++;

		// check for an existing property with this name, add it if not found
		AddProperty( name );
		assert( curPropID );
		MString propid( "propid " );
		propid += curPropID;

		// brush index
		MString brushid( "brushindex " );
		brushid += curBrushID++;

		// add the brush node to the node tree
		A(
		curRoot,
			A(
			A(
			A(
			A(
			A(
			A(
			A(
			A(
			N(),
				N("worldnode")
			),
				A(
				N(),
					N("type brush")
				)
			),
				A(
				N(),
					N(brushid.asChar())
				)
			),
				A(
				N(),
					N("label \"Brush\"")
				)
			),
				A(
				N(),
					N(nodeid.asChar())
				)
			),
				A(
				A(
				N(),
					N("flags")
				),
					N()
				)
			),
				A(
				A(
				A(
				N(),
					N("properties")
				),
					A(
					N(),
						N("name \"Brush\"")
					)
				),
					A(
					N(),
						N(propid.asChar())
					)
				)
			),
				A(
				A(
				N(),
					N("childlist")
				),
					nextRoot
				)
			)
		);

		// create a new polyhedron and its poly list
		curPolyhedronRoot = new CLTANode;
		curEditPoly = new CLTANode;
		curPointList = new CLTANode;

		// add the new polyhedron to the tree
		A(
		polyRoot,
			A(
			A(
			N(),
				N("polyhedron")
			),
				curPolyhedronRoot
			)
		);

		// set the color of the polyhedron
		A(
		curPolyhedronRoot,
			A(
			N(),
				N("color 255 255 255")
			)
		);

		// add the pointlist to the polyhedron
		A(
		curPolyhedronRoot,
			curPointList
		);

		// add the polylist to the polyhedron
		A(
		curPolyhedronRoot,
			A(
			A(
			N(),
				N("polylist")
			),
				curEditPoly
			)
		);
	}
	else
	{
		// adding a new container node
		MString label( "label \"" );
		label += name + MString("\"");
		MString nodeid( "nodeid " );
		nodeid += curNodeID++;

		// add the container to the node tree
		A(
		curRoot,
			A(
			A(
			A(
			A(
			A(
			A(
			A(
			N(),
				N("worldnode")
			),
				A(
				N(),
					N("type null")
				)
			),
				A(
				N(),
					N(label.asChar())
				)
			),
				A(
				N(),
					N(nodeid.asChar())
				)
			),
				A(
				A(
				N(),
					N("flags")
				),
					N()
				)
			),
				A(
				A(
				N(),
					N("properties")
				),
					A(
					N(),
						N("propid 0")
					)
				)
			),
				A(
				A(
				N(),
					N("childlist")
				),
					nextRoot
				)
			)
		);
	}
}

void LTAFile::AscendTree( void )
{
	nodeList = nodeList->prev;
	delete nodeList->next;
	nodeList->next = NULL;
}

// add polygon info to the current polyhedron
void LTAFile::AddBrushPolygon( int numVerts, int* indices, const MVector& n, const float d, const MVector& o, const MVector& p, const MVector& q, const MString& textureName )
{
	assert( numVerts && indices );

	MString sbrushindex;
	MString snodeid;

	// setup the vert index string
	static const MString space( " " );
	MString sf( "f" );
	for( int i = 0; i < numVerts; i++ )
	{
		sf += space + indices[i];
	}

	// setup strings for the polygon
	MString sn = "n " + VectorString( n );
	MString sd = "dist ";
	sd += d;
	MString so = VectorString( o );
	MString sp = VectorString( p );
	MString sq = VectorString( q );
	MString stex( "name \"" );
	stex += textureName + "\"";

	// add the editpoly node
	A(
	curEditPoly,
		A(
		A(
		A(
		A(
		A(
		A(
		A(
		N(),
			N("editpoly")
		),
			A(
			N(),
				N(sf.asChar())
			)
		),
			A(
			N(),
				N(sn.asChar())
			)
		),
			A(
			N(),
				N(sd.asChar())
			)
		),
			A(
			A(
			A(
			A(
			A(
			A(
			N(),
				N("textureinfo")
			),
				A(
				N(),
					N(so.asChar())
				)
			),
				A(
				N(),
					N(sp.asChar())
				)
			),
				A(
				N(),
					N(sq.asChar())
				)
			),
				A(
				N(),
					N("sticktopoly 1")
				)
			),
				A(
				N(),
					N(stex.asChar())
				)
			)
		),
			A(
			N(),
				N("flags")
			)
		),
			A(
			N(),
				N("shade 0 0 0")
			)
		)
	);
}

// add vertex info to the current polyhedron
void LTAFile::AddBrushVertices( int numVerts, const MPoint* verts )
{
	// add the pointlist node
	A(
	curPointList,
		N("pointlist")
	);
	
	// add the verts to the pointlist node
	for( int i = 0; i < numVerts; i++ )
	{
		MString vertOut = VectorString( verts[i] );
		
		A(
		curPointList,
			A(
			N(),
				N(vertOut.asChar())
			)
		);
	}
}

// adds a new worldmodel as a child, and sets the root to that object
void LTAFile::AddWorldModel( const char* name, const MVector& pos )
{
	// location in the tree we are adding the worldmodel to
	CLTANode* curRoot = nodeList->containerNode;

	// create the root below this one
	CLTANode* nextRoot = new CLTANode;

	// update the lta node list
	LTANodeList* prevNode = nodeList;
	nodeList = new LTANodeList;
	prevNode->next = nodeList;
	nodeList->prev = prevNode;
	nodeList->next = NULL;
	nodeList->containerNode = nextRoot;

	// add the property to the list
	PropertyList* newProp = new PropertyList;
	newProp->name = name;
	newProp->isBrush = false;
	newProp->id = nextPropID++;
	newProp->next = propList;
	propList = newProp;
	curPropID = newProp->id;

	// setup strings for the tree
	MString propName = MString("string \"Name\" () (data \"") + name + MString("\")");
	MString propPos = MString("vector \"Pos\" ( distance ) (data (vector (") + VectorString(pos) + MString(")))");

	// add the property to the property tree
	A(
	propRoot,
		A(
		A(
		N(),
			N("proplist")
		),
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			 A(
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			 A(
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			 A(
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			 A(
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			N(),
				A(
				N(),
					N(propName.asChar())
				)
			),
				A(
				N(),
					N(propPos.asChar())
				)
			),
				A(
				N(),
					N("rotation \"Rotation\" () (data (eulerangles (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N("bool \"Template\" () (data 0)")
				)
			),
				A(
				N(),
					N("longint \"DamageProperties\" (groupowner group1) (data 0.000)")
				)
			),
				A(
				N(),
					N("longint \"ExplosionProperties\" (groupowner group2) (data 0.000)")
				)
			),
				A(
				N(),
					N("string \"DebrisType\" (staticlist) (data \"Glass large\")")
				)
			),
				A(
				N(),
					N("string \"SurfaceOverride\" (staticlist) (data \"Unknown\")")
				)
			),
				A(
				N(),
					N("bool \"Visible\" () (data 1)")
				)
			),
				A(
				N(),
					N("bool \"Solid\" () (data 1)")
				)
			),
				A(
				N(),
					N("bool \"Rayhit\" () (data 1)")
				)
			),
				A(
				N(),
					N("bool \"Gravity\" () (data 0)")
				)
			),
				A(
				N(),
					N("string \"BlendMode\" (staticlist) (data \"None\")")
				)
			),
				A(
				N(),
					N("bool \"BlockLight\" () (data 1)")
				)
			),
				A(
				N(),
					N("bool \"BoxPhysics\" () (data 1)")
				)
			),
				A(
				N(),
					N("bool \"FogDisable\" () (data 0)")
				)
			),
				A(
				N(),
					N("bool \"IsKeyframed\" () (data 1)")
				)
			),
				A(
				N(),
					N("real \"Alpha\" () (data 1.000)")
				)
			),
				A(
				N(),
					N("string \"Attachments\" ()")
				)
			),
				A(
				N(),
					N("bool \"RemoveAttachments\" () (data 1)")
				)
			),
				A(
				N(),
					N("vector \"AttachDir\" () (data (vector (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N("longint \"Disturbances\" (groupowner group6) (data 0.000)")
				)
			),
				A(
				N(),
					N("real \"HitPoints\" (group1) (data 100.000)")
				)
			),
				A(
				N(),
					N("real \"MaxHitPoints\" (group1) (data 100.000)")
				)
			),
				A(
				N(),
					N("real \"Energy\" (group1) (data 100.000)")
				)
			),
				A(
				N(),
					N("real \"MaxEnergy\" (group1) (data 100.000)")
				)
			),
				A(
				N(),
					N("real \"Armor\" (group1) (data 100.000)")
				)
			),
				A(
				N(),
					N("real \"MaxArmor\" (group1) (data 100.000)")
				)
			),
				A(
				N(),
					N("longint \"DamageTriggerCounter\" (group1) (data 0.000)")
				)
			),
				A(
				N(),
					N("string \"DamageCommand\" (group1 notifychange)")
				)
			),
				A(
				N(),
					N("longint \"DamageTriggerNumSends\" (group1) (data 1.000)")
				)
			),
				A(
				N(),
					N("string \"DamagerMessage\" (group1)")
				)
			),
				A(
				N(),
					N("string \"DeathCommand\" (group1 notifychange)")
				)
			),
				A(
				N(),
					N("string \"PlayerDeathCommand\" (group1 notifychange)")
				)
			),
				A(
				N(),
					N("string \"KillerMessage\" (group1)")
				)
			),
				A(
				N(),
					N("bool \"CanHeal\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("bool \"CanRepair\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("bool \"CanDamage\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("real \"Mass\" (group1) (data 2000.000)")
				)
			),
				A(
				N(),
					N("bool \"CreateExplosion\" (group2) (data 0)")
				)
			),
				A(
				N(),
					N("longint \"WeaponId\" (group2) (data 0.000)")
				)
			),
				A(
				N(),
					N("bool \"FireAlongForward\" (group2) (data 0)")
				)
			),
				A(
				N(),
					N("real \"DamageFactor\" (group2) (data 1.000)")
				)
			),
				A(
				N(),
					N("string \"Spawn\" (group2)")
				)
			),
				A(
				N(),
					N("bool \"NeverDestroy\" (group1) (data 1)")
				)
			),
				A(
				N(),
					N("longint \"DestroyAlarmLevel\" (group6) (data 0.000)")
				)
			),
				A(
				N(),
					N("real \"StimulusRadius\" (group6) (data 0.000)")
				)
			)
		)
	);

	// adding a new worldmodel node
	MString nodeid( "nodeid " );
	nodeid += curNodeID++;
	MString propid( "propid " );
	propid += curPropID;

	// add the worldmodel node to the node tree
	A(
	curRoot,
		A(
		A(
		A(
		A(
		A(
		A(
		N(),
			N("worldNode")
		),
			A(
			N(),
				N("type object")
			)
		),
			A(
			N(),
				N(nodeid.asChar())
			)
		),
			A(
			N(),
				N("flags ()")
			)
		),
			A(
			A(
			A(
			N(),
				N("properties")
			),
				A(
				N(),
					N("name \"WorldModel\"")
				)
			),
				A(
				N(),
					N(propid.asChar())
				)
			)
		),
			A(
			A(
			N(),
				N("childlist")
			),
				nextRoot
			)
		)
	);
}

// adds a new keyframer as a child, and sets the root to that object
void LTAFile::AddKeyFramer( const char* name, const char* worldModelName, const char* keyBaseName, const char* keyFile, const MVector& pos )
{
	// location in tree we are adding the container to
	CLTANode* curRoot = nodeList->containerNode;

	// create the root below this one
	CLTANode* nextRoot = new CLTANode;

	// update the lta node list
	LTANodeList* prevNode = nodeList;
	nodeList = new LTANodeList;
	prevNode->next = nodeList;
	nodeList->prev = prevNode;
	nodeList->next = NULL;
	nodeList->containerNode = nextRoot;

	// add the property to the list
	PropertyList* newProp = new PropertyList;
	newProp->name = name;
	newProp->isBrush = false;
	newProp->id = nextPropID++;
	newProp->next = propList;
	propList = newProp;
	curPropID = newProp->id;

	// setup strings for the tree
	MString propName = MString("string \"Name\" () (data \"") + name + MString("\")");
	MString propPos = MString("vector \"Pos\" ( distance ) (data (vector (") + VectorString(pos) + MString(")))");
	MString propModelName = MString("string \"ObjectName\" () (data \"") + worldModelName + MString("\")");
	MString propKeyName = MString("string \"BaseKeyName\" () (data \"") + keyBaseName + MString("\")");

	MString propKeyFile;
	if( externalKeys )
		propKeyFile = MString("string \"ExternalKeyFile\" () (data \"") + keyFile + MString("\")");
	else
		propKeyFile = MString("string \"ExternalKeyFile\" ()");

	// add the property to the property tree
	A(
	propRoot,
		A(
		A(
		N(),
			N("proplist")
		),
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			 A(
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			 A(
			A(
			N(),
				A(
				N(),
					N(propName.asChar())
				)
			),
				A(
				N(),
					N(propPos.asChar())
				)
			),
				A(
				N(),
					N("rotation \"Rotation\" () (data (eulerangles (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N(propModelName.asChar())
				)
			),
				A(
				N(),
					N(propKeyName.asChar())
				)
			),
				A(
				N(),
					N(propKeyFile.asChar())
				)
			),
				A(
				N(),
					N("real \"KeyDataIndex\" (hidden) (data -1.0)")
				)
			),
				A(
				N(),
					N("bool \"PushObjects\" () (data 0)")
				)
			),
				A(
				N(),
					N("bool \"StartActive\" () (data 0)")
				)
			),
				A(
				N(),
					N("bool \"StartPaused\" () (data 0)")
				)
			),
				A(
				N(),
					N("bool \"Looping\" () (data 0)")
				)
			),
				A(
				N(),
					N("bool \"AlignToPath\" () (data 0)")
				)
			),
				A(
				N(),
					N("bool \"IgnoreOffsets\" () (data 1)")
				)
			),
				A(
				N(),
					N("string \"TargetName\" ()")
				)
			),
				A(
				N(),
					N("vector \"TargetOffset\" () (data (vector (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N("string \"ActiveSound\" ()")
				)
			),
				A(
				N(),
					N("real \"SoundRadius\" () (data 1000.000)")
				)
			),
				A(
				N(),
					N("string \"RotationWave\" () (data \"LINEAR\")")
				)
			),
				A(
				N(),
					N("longint \"Waveform\" (groupowner group1) (data 0.000)")
				)
			),
				A(
				N(),
					N("real \"TotalPathTime\" (group1) (data 0.000)")
				)
			),
				A(
				N(),
					N("string \"Wavetype\" (group1 staticlist) (data \"Linear\")")
				)
			)
		)
	);

	// adding a new keyframer node
	MString nodeid( "nodeid " );
	nodeid += curNodeID++;
	MString propid( "propid " );
	propid += curPropID;

	// add the keyframer node to the tree
	A(
	curRoot,
		A(
		A(
		A(
		A(
		A(
		A(
		N(),
			N("worldnode")
		),
			A(
			N(),
				N("type object")
			)
		),
			A(
			N(),
				N(nodeid.asChar())
			)
		),
			A(
			N(),
				N("flags ( path )")
			)
		),
			A(
			A(
			A(
			N(),
				N("properties")
			),
				A(
				N(),
					N("name \"KeyFramer\"")
				)
			),
				A(
				N(),
					N(propid.asChar())
				)
			)
		),
			A(
			A(
			N(),
				N("childlist")
			),
				nextRoot
			)
		)
	);
}

// add a new group object as a child, and sets the root to that object
void LTAFile::AddKeyFramerGroup( const char* name, std::vector<MString>& keyFramerNames )
{
	// location in tree we are adding the container to
	CLTANode* curRoot = nodeList->containerNode;

	// create the root below this one
	CLTANode* nextRoot = new CLTANode;

	// create the node that properties are added to
	CLTANode* propNode = new CLTANode;

	// update the lta node list
	LTANodeList* prevNode = nodeList;
	nodeList = new LTANodeList;
	prevNode->next = nodeList;
	nodeList->prev = prevNode;
	nodeList->next = NULL;
	nodeList->containerNode = nextRoot;

	// add the property to the list
	PropertyList* newProp = new PropertyList;
	newProp->name = name;
	newProp->isBrush = false;
	newProp->id = nextPropID++;
	newProp->next = propList;
	propList = newProp;
	curPropID = newProp->id;

	// setup strings for the tree
	MString propName = MString("string \"Name\" () (data \"") + name + MString("\")");

	// add the property to the property tree
	A(
	propRoot,
		A(
		A(
		N(),
			N("proplist")
		),
			A(
			A(
			A(
			propNode,
				A(
				N(),
					N(propName.asChar())
				)
			),
				A(
				N(),
					N("vector \"Pos\" ( distance ) (data (vector (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N("rotation \"Rotation\" () (data (eulerangles (0.000 0.000 0.000)))")
				)
			)
		)
	);

	MString curProp;
	int numKeyFramers = keyFramerNames.size();

	// add the keyframer references
	for( int i = 0; i < 50; i++ )
	{
		if( i < numKeyFramers )
			curProp = MString("string \"Object") + (i+1) + MString("\" (objectlink) (data \"") + keyFramerNames[i] + "\")";
		else
			curProp = MString("string \"Object") + (i+1) + MString("\" (objectlink)");
	
		// add the property
		A( propNode, A(N(), N(curProp.asChar())) );
	}

	// adding a new key node
	MString nodeid( "nodeid " );
	nodeid += curNodeID++;
	MString propid( "propid " );
	propid += curPropID;

	// add the group node to the tree
	A(
	curRoot,
		A(
		A(
		A(
		A(
		A(
		A(
		A(
		N(),
			N("worldnode")
		),
			A(
			N(),
				N("type object")
			)
		),
			A(
			N(),
				N("label \"Group\"")
			)
		),
			A(
			N(),
				N(nodeid.asChar())
			)
		),
			A(
			N(),
				N("flags ()")
			)
		),
			A(
			A(
			A(
			N(),
				N("properties")
			),
				A(
				N(),
					N("name \"Group\"")
				)
			),
				A(
				N(),
					N(propid.asChar())
				)
			)
		),
			A(
			A(
			N(),
				N("childlist")
			),
				nextRoot
			)
		)
	);
}

// add a new key as a child, and sets the root to that object
void LTAFile::AddKey( const char* name, float time, const MVector& kfPos, const MVector& pos, const MVector& eulerRotation, const char* sound, float soundRadius, const char* command )
{
	if( externalKeys )
		AddKeyExternalFile( name, time, pos - kfPos, eulerRotation, sound, soundRadius, command );
	else
		AddKeyWorldFile( name, time, pos, eulerRotation, sound, soundRadius, command );
}

// add a new key as a child, and sets the root to that object
void LTAFile::AddKeyWorldFile( const char* name, float time, const MVector& pos, const MVector& eulerRotation, const char* sound, float soundRadius, const char* command )
{
	// location in the tree we are adding the container to
	CLTANode* curRoot = nodeList->containerNode;

	// create the root below this one
	CLTANode* nextRoot = new CLTANode;

	// update the lta node list
	LTANodeList* prevNode = nodeList;
	nodeList = new LTANodeList;
	prevNode->next = nodeList;
	nodeList->prev = prevNode;
	nodeList->next = NULL;
	nodeList->containerNode = nextRoot;

	// add the property to the list
	PropertyList* newProp = new PropertyList;
	newProp->name = name;
	newProp->isBrush = false;
	newProp->id = nextPropID++;
	newProp->next = propList;
	propList = newProp;
	curPropID = newProp->id;

	// setup strings for the tree
	MString propName = MString("string \"Name\" () (data \"") + name + MString("\")");
	MString propPos = MString("vector \"Pos\" ( distance ) (data (vector (") + VectorString(pos) + MString(")))");
	MString propRot = MString("rotation \"Rotation\" () (data (eulerangles (") + VectorString(eulerRotation) + MString(")))");
	MString propTime = MString("real \"TimeStamp\" () (data ") + time + MString(")");
	MString propSoundRadius = MString("real \"SoundRadius\" (radius) (data ") + soundRadius + MString(")");

	MString propSound = MString("string \"SoundName\" ()");
	if( strlen( sound ) )
		propSound += MString(" (data \"") + sound + MString("\")");

	MString propCommand = MString("string \"Command\" (notifychange)");
	if( strlen( command ) )
		propCommand += MString(" (data \"") + command + MString("\")");

	// add the property to the property tree
	A(
	propRoot,
		A(
		A(
		N(),
			N("proplist")
		),
			A(
			A(
			A(
			A(
			A(
			 A(
			 A(
			 A(
			 A(
			N(),
				A(
				N(),
					N(propName.asChar())
				)
			),
				A(
				N(),
					N(propPos.asChar())
				)
			),
				A(
				N(),
					N(propRot.asChar())
				)
			),
				A(
				N(),
					N(propTime.asChar())
				)
			),
				A(
				N(),
					N(propSound.asChar())
				)
			),
				A(
				N(),
					N(propSoundRadius.asChar())
				)
			),
				A(
				N(),
					N(propCommand.asChar())
				)
			),
				A(
				N(),
					N("vector \"BezierPrev\" (bezierprevtangent) (data (vector (0.000 0.000 0.000)))")
				)
			),
				A(
				N(),
					N("vector \"BezierNext\" (beziernexttangent) (data (vector (0.000 0.000 0.000)))")
				)
			)
		)
	);

	// adding a new key node
	MString nodeid( "nodeid " );
	nodeid += curNodeID++;
	MString propid( "propid " );
	propid += curPropID;

	// add the key node to the tree
	A(
	curRoot,
		A(
		A(
		A(
		A(
		A(
		A(
		N(),
			N("worldnode")
		),
			A(
			N(),
				N("type object")
			)
		),
			A(
			N(),
				N(nodeid.asChar())
			)
		),
			A(
			N(),
				N("flags ()")
			)
		),
			A(
			A(
			A(
			N(),
				N("properties")
			),
				A(
				N(),
					N("name \"Key\"")
				)
			),
				A(
				N(),
					N(propid.asChar())
				)
			)
		),
			A(
			A(
			N(),
				N("childlist")
			),
				nextRoot
			)
		)
	);
}

// add a key to the external key file
void LTAFile::AddKeyExternalFile( const char* name, float time, const MVector& pos, const MVector& eulerRotation, const char* sound, float soundRadius, const char* command )
{
	MString keyName = MString("\"") + name + MString("\"");
	MString keyPos = MString("pos ") + VectorString( pos );
	MString keyRot = MString("rot ") + VectorString( eulerRotation );
	MString keyTime = MString("time ") + time;
	MString keySound = MString("sound \"") + sound + MString("\"");
	MString keySoundRadius = MString("soundradius ") + soundRadius;
	MString keyCommand = MString("command \"") + command + MString("\"");

	A(
	keyList,
		A(
		A(
		A(
		A(
		A(
		 A(
		 A(
		 A(
		 A(
		N(),
			N(keyName.asChar())
		),
			A(
			N(),
				N(keyPos.asChar())
			)
		),
			A(
			N(),
				N(keyRot.asChar())
			)
		),
			A(
			N(),
				N(keyTime.asChar())
			)
		),
			A(
			N(),
				N(keySound.asChar())
			)
		),
			A(
			N(),
				N(keySoundRadius.asChar())
			)
		),
			A(
			N(),
				N(keyCommand.asChar())
			)
		),
			A(
			N(),
				N("bezierprev 0.0 0.0 0.0")
			)
		),
			A(
			N(),
				N("beziernext 0.0 0.0 0.0")
			)
		)
	);
}

// save out the lta file.  returns 0 on error
bool LTAFile::Output( const MString& fileName, const MString& keyFileName )
{
	bool success = false;

	success = CLTANodeWriter::SaveNode( root, fileName.asChar(), CLTAUtil::IsFileCompressed( fileName.asChar() ) );

	if( success && externalKeys )
		success = CLTANodeWriter::SaveNode( keyRoot, keyFileName.asChar(), CLTAUtil::IsFileCompressed( keyFileName.asChar() ) );

	return success;
}

// create a maya string containing a formatted vector
MString LTAFile::VectorString( const MVector& vec ) const
{
	char str[256];
	sprintf( str, "%f %f %f", vec.x, vec.y, vec.z );

	return MString( str );
}
