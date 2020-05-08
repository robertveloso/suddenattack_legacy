//#include <fstream.h>
#include "MaxWorldImport.h"
#include "dummy.h"
#include "stdmat.h"
//#include "mnmath.h"


// configuration dialog process
static BOOL CALLBACK ConfigDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	MAXWorldImport* import = (MAXWorldImport*)GetWindowLong( hWnd, GWL_USERDATA );
	if( !import && (msg != WM_INITDIALOG) )
		return FALSE;

	switch( msg )
	{
	case WM_INITDIALOG:
		import = (MAXWorldImport*)lParam;
		// setup dialog controls to match settings
		SetCheckBox( hWnd, IDC_IMPORT_UV, import->importTexInfo );
		SetCheckBox( hWnd, IDC_IMPORT_OBJECTS, import->importObjects );
		SetCheckBox( hWnd, IDC_TGA, import->importTexTga );
		SetCheckBox( hWnd, IDC_DTX, !import->importTexTga );
		SetWindowText( GetDlgItem( hWnd, IDC_TEXTURE_BASEPATH ), import->basePath );
		char tmpStr[256];
		sprintf( tmpStr, "%.3f", import->scale );
		SetWindowText( GetDlgItem( hWnd, IDC_SCALE ), tmpStr );
		// put a pointer to the importer class instance in the user data
		SetWindowLong( hWnd, GWL_USERDATA, lParam );
		CenterWindow( hWnd, GetParent( hWnd ) );
		break;

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_CANCEL:
			EndDialog( hWnd, 1 );
			break;

		case IDC_OK:
			import->DialogAccepted( hWnd );
			EndDialog( hWnd, 0 );
			break;
			
		case IDC_BROWSE_TEXTURE_BASEPATH:
		{
			TCHAR dir[_MAX_PATH];
			GetWindowText( GetDlgItem( hWnd, IDC_TEXTURE_BASEPATH ), dir, _MAX_PATH );
			import->ip->ChooseDirectory( hWnd, GetString(IDS_CHOOSE_DIRECTORY), dir );
			if( strlen( dir ) > 0 )
				SetWindowText( GetDlgItem( hWnd, IDC_TEXTURE_BASEPATH ), dir );
			break;
		}
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}


// fill in settings based on import dialog
void MAXWorldImport::DialogAccepted( HWND hWnd )
{
	char tmpStr[256];
	GetWindowText( GetDlgItem( hWnd, IDC_SCALE ), tmpStr, 255 );
	scale = (float)atof( tmpStr );

	importTexInfo = GetCheckBox( hWnd, IDC_IMPORT_UV ) > 0;
	importObjects = GetCheckBox( hWnd, IDC_IMPORT_OBJECTS ) > 0;
	importTexTga = GetCheckBox( hWnd, IDC_TGA ) > 0;
	GetWindowText( GetDlgItem( hWnd, IDC_TEXTURE_BASEPATH ), basePath, _MAX_PATH );
	basePathLen = strlen( basePath );

	// make sure path ends in a backslash
	if( basePathLen && basePath[basePathLen-1] != '\\' )
	{
		basePath[basePathLen] = '\\';
		basePath[basePathLen+1] = 0;
		basePathLen++;
	}
}


// import from a file
int MAXWorldImport::DoImport( const TCHAR* name, ImpInterface* ii, Interface* ip, BOOL suppressPrompts )
{
	this->ip = ip;
	this->ii = ii;

	// try to load the configuration info
	LoadImporterConfig();

	if( !suppressPrompts )
	{
		// show the configuration dialog
		int res = DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_IMPORT), ip->GetMAXHWnd(), ConfigDialogProc, (LPARAM)this );

		// they canceled
		if( res != 0 ) return 1;
	}

	// save the configuration info
	SaveImporterConfig();

    HCURSOR waitCursor = LoadCursor(NULL, IDC_WAIT);
    HCURSOR normalCursor = LoadCursor(NULL, IDC_ARROW);
    SetCursor( waitCursor );

	int retVal = 1;

	if( retVal && !world.LoadFile( name ) )
		retVal = 0;

	if( retVal && !LoadTextures() )
		retVal = 0;

	if( retVal && !BuildHierarchy() )
		retVal = 0;

	SetCursor( normalCursor );

	return retVal;
}


bool MAXWorldImport::LoadTextures( void )
{
	int numTextures = world.NumTextures();
	matList.reserve( numTextures );
	texDims.reserve( numTextures );

	// create the multi-material if we might need it
	if( numTextures > 1 )
	{
		multiMat = NewDefaultMultiMtl();
		multiMat->SetNumSubMtls( numTextures );
		multiMat->SetName( "AllTextures" );
	}

	for( int i = 0; i < numTextures; i++ )
	{
		BitmapTex* tex = NewDefaultBitmapTex();
		StdMat* mat = NewDefaultStdMat();
		mat->SetSubTexmap( ID_DI, tex );
		ip->ActivateTexture( tex, mat );

		if( multiMat )
		{
			multiMat->SetSubMtl( i, mat );
		}

		// attempt to load the texture
		floatPair curTexDims;
		LoadTexture( world.TextureName( i ), mat, tex, curTexDims );

		texDims.push_back( curTexDims );
		matList.push_back( mat );
	}

	return true;
}


// attempt to load a texture.
// always sets the name of the material and texture, and creates default dimensions
// if user wants full texture info, it will attempt to load an actual texture and use its dims
void MAXWorldImport::LoadTexture( const TSTR name, StdMat* mat, BitmapTex* tex, floatPair& dims )
{
	TSTR fullPath = name;
	TSTR simpleName;

	// default for the dimensions
	dims = floatPair( 256.0f, 256.0f );

	// build the full path and simple material name given the relative path
	GenerateTextureName( fullPath, simpleName );

	// set the name of the material and texture
	mat->SetName( simpleName );
	tex->SetName( simpleName );

	// we need to attempt to load texture information
	if( importTexInfo )
	{
		FILE* fileExists = fopen( fullPath, "rb" );
		if( !fileExists )
		{
			numFailedTextureLoads++;
		}
		else
		{
			fclose( fileExists );

			// the texture exists, ask the bitmap texture to load it
			tex->SetMapName( fullPath );

			// get the dimensions of the loaded texture
			Bitmap* bitmap = tex->GetBitmap( 0 );

			if( !bitmap )
			{
				numFailedTextureLoads++;
			}
			else
			{
				dims = floatPair( bitmap->Width(), bitmap->Height() );
			}
		}
	}
}


void MAXWorldImport::GenerateTextureName( TSTR& name, TSTR& simpleName )
{
	// strip off the starting backslash if any
	if( name.first( '\\' ) == 0 )
		name = name.Substr( 1, name.Length() - 1 );

	// add on the base texture path
	name = TSTR(basePath) + name;

	// replace the extension
	int extPos = name.last( '.' );
	if( extPos > 0 )
		name = name.Substr( 0, extPos );
	if( importTexTga )
		name += ".tga";
	else
		name += ".dtx";

	// get the simple name
	int slashPos = name.last( '\\' );
	if( extPos > 0 )
		simpleName = name.Substr( 0, extPos );
	simpleName = simpleName.Substr( slashPos + 1, simpleName.Length() - slashPos - 1 );
}


bool MAXWorldImport::BuildHierarchy( void )
{
	WINode* root = world.GetWorldRoot();
	if( !root )
		return false;

	// build up the scene from children of the root
	int numChildren = root->NumChildren();

	for( int i = 0; i < numChildren; i++ )
	{
		WINode* curChild = root->GetChild( i );
		if( IsValidChild( curChild ) && !AddNode( curChild ) )
		{
			assert( 0 );	// this should never happen, and will result in a partially imported scene
			return false;
		}
	}

	return true;
}


INode* MAXWorldImport::AddNode( WINode* node )
{
	assert( node );

	INode* thisNode = NULL;
	bool isContainer = false;
	int i;

	switch( node->Type() )
	{
	case ePrefabRef:
	case eContainer:
		// don't create a node for containers
		isContainer = true;
		break;
	case eBrush:
		thisNode = AddBrush( node );
		break;
	case eObject:
		if( importObjects )
			thisNode = AddObject( node );
		else
			isContainer = true;
		break;
	default:
		assert( 0 );
		return NULL;
	}

	// count up the number of valid children
	int numChildren = node->NumChildren();
	int numValidChildren = numChildren;

	for( i = numChildren - 1; i >= 0; i-- )
	{
		WINode* curChild = node->GetChild( i );
		if( !IsValidChild( curChild ) )
			numValidChildren--;
	}

	if( !numValidChildren )
		return thisNode;

	// set up the array of child nodes
	INodeTab children;
	children.SetCount( numValidChildren );

	// recurse into children
	int curChildNum = 0;
	for( i = 0; i < numChildren; i++ )
	{
		WINode* curChild = node->GetChild( i );

		if( !IsValidChild( curChild ) )
			continue;

		INode* addedNode = AddNode( curChild );

		if( !addedNode )
		{
			assert( 0 );
			return NULL;
		}

		children[curChildNum++] = addedNode;

		// add the new child to this node
		if( !isContainer )
			thisNode->AttachChild( addedNode );
	}

	// create the container
	if( isContainer )
	{
		CStr containerName( node->Name() );
		thisNode = ip->GroupNodes( &children, &containerName, FALSE );
		assert( thisNode );
	}

	return thisNode;
}


// returns true if the child should be created given the current import options
bool MAXWorldImport::IsValidChild( WINode* node )
{
	// brushes always import
	if( node->Type() == eBrush )
		return true;

	// we are importing objects and the node is an object
	if( importObjects && (node->Type() == eObject) )
		return true;

	// otherwise the node must have valid children in order to be valid itself
	int numChildren = node->NumChildren();

	for( int i = numChildren - 1; i >= 0; i-- )
	{
		WINode* curChild = node->GetChild( i );
		if( !IsValidChild( curChild ) )
			numChildren--;
	}

	return numChildren > 0;
}


INode* MAXWorldImport::AddObject( WINode* node )
{
	DummyObject* obj = (DummyObject*)ip->CreateInstance( HELPER_CLASS_ID, Class_ID(DUMMY_CLASS_ID,0) );
	assert( obj );

	Point3 objSize( IMPORTED_OBJECT_SIZE, IMPORTED_OBJECT_SIZE, IMPORTED_OBJECT_SIZE );
	objSize *= scale;
	obj->SetBox( Box3(-objSize, objSize) );

	INode* objNode = ip->CreateObjectNode( obj );	
	if( !objNode )
	{
		assert( 0 );
		return NULL;
	}

	objNode->SetName( (char*)node->Name() );

	// grab the position from the proeprty
	WIProp* property = world.GetProperty( node->PropIndex() );
	LTVector3f ltPos = property->Pos();
	LTVector3f ltRot = property->Rot();
	ltPos *= scale;
	Point3 pos( ltPos.x, ltPos.z, ltPos.y );

	// position the node based on the center of the world space position of the brush verts
	Matrix3 nodeTM( TRUE );
	nodeTM.SetTrans( pos );
	objNode->SetNodeTM( 0, nodeTM );

	return objNode;
}


INode* MAXWorldImport::AddBrush( WINode* node )
{
	int i;

	TriObject* obj = CreateNewTriObject();
	INode* objNode = ip->CreateObjectNode( obj );
	
	if( !objNode )
	{
		assert( 0 );
		return NULL;
	}

	objNode->SetName( (char*)node->Name() );

	Mesh* mesh = &(obj->mesh);

	// grab the brush
	WIBrush* brush = world.GetBrush( node->BrushIndex() );

	int numVerts = brush->numVerts;
	int numPolies = brush->numPolies;

	// count the number of tris
	int numTris = 0;
	for( i = 0; i < numPolies; i++ )
	{
		int numIndices = brush->polies[i].numIndices;
		if( numIndices >= 3 )
		{
			numTris += (numIndices - 2);
		}
	}

	if( !numTris || !numVerts )
		return objNode;

	// setup the arrays of actual geometry information
	mesh->setNumTVerts( numTris * 3 );
	mesh->setNumVerts( numVerts );
	mesh->setNumFaces( numTris );
	mesh->setNumTVFaces( numTris );

	Point3 center( brush->center.x, brush->center.z, brush->center.y );
	center *= scale;

	// create a verts array (used later for calculating UVs from OPQs)
	Point3* verts = new Point3[numVerts];
	assert( verts );

	// add the brush verts to the array and the mesh
	for( i = 0; i < numVerts; i++ )
	{
		LTVector3f vec = brush->verts[i];
		verts[i] = Point3( vec.x, vec.z, vec.y );
		verts[i] *= scale;
		verts[i] -= center;
		mesh->verts[i] = verts[i];
	}

	// add the faces to the mesh
	int curTri = 0;					// current triangle index being used
	int curTexIndex;				// the current texture index (first initialized in first pass through loop)
	bool multipleTexUsed = false;	// when true, apply the multi-map

	for( i = 0; i < numPolies; i++ )
	{
		WIPoly& curPoly = brush->polies[i];

		// check to see if more than one map has been used on this object
		if( i == 0 )
			curTexIndex = curPoly.textureIndex;
		if( curTexIndex != curPoly.textureIndex )
			multipleTexUsed = true;
		curTexIndex = curPoly.textureIndex;

		// texture coordinate information (OPQ->UV conversion)
		float texWidth = texDims[curTexIndex].first;
		float texHeight = texDims[curTexIndex].second;
		Point3 o( curPoly.O.x, curPoly.O.z, curPoly.O.y );
		Point3 p( curPoly.P.x, curPoly.P.z, curPoly.P.y );
		Point3 q( curPoly.Q.x, curPoly.Q.z, curPoly.Q.y );
		o *= scale;
		o -= center;
		p /= scale;
		q /= scale;

		for( int j = 0; j < (curPoly.numIndices - 2); j++ )
		{
			Face& tri = mesh->faces[curTri];
			TVFace& uvTri = mesh->tvFace[curTri];

			// all faces are flat shaded
			tri.setSmGroup( 0 );
			// add the vert indices for the current tri of the poly
			tri.setVerts( curPoly.vertIndices[0], curPoly.vertIndices[j+2], curPoly.vertIndices[j+1] );
			// set edge visibility to true for polygon edges
			tri.setEdgeVis( 0, (j==curPoly.numIndices-3) ? EDGE_VIS : EDGE_INVIS );
			tri.setEdgeVis( 1, EDGE_VIS );
			tri.setEdgeVis( 2, (j==0) ? EDGE_VIS : EDGE_INVIS );
			// set the material index
			tri.setMatID( curTexIndex );

			// build up the list of vert positions for this triangle
			Point3 curVerts[3];
			curVerts[0] = verts[ curPoly.vertIndices[0] ];
			curVerts[1] = verts[ curPoly.vertIndices[j+2] ];
			curVerts[2] = verts[ curPoly.vertIndices[j+1] ];

			// create the uvs for this tri
			for( int k = 0; k < 3; k++ )
			{
				Point3 curVert = o - curVerts[k];

				float u = DotProd( curVert, p );
				float v = DotProd( curVert, q );

				u /= texWidth;
				v /= texHeight;

				mesh->tVerts[curTri*3 + k] = Point3( -u, v, 0.0f );
			}

			// map the tri verts to the uvs
			uvTri.setTVerts( curTri*3, curTri*3 + 1, curTri*3 + 2 );

			curTri++;
		}
	}

	// set the material for the node
	if( multipleTexUsed )
		objNode->SetMtl( multiMat );
	else
		objNode->SetMtl( matList[curTexIndex] );


	// position the node based on the center of the world space position of the brush verts
	Matrix3 nodeTM( TRUE );
	nodeTM.SetTrans( center );
	objNode->SetNodeTM( 0, nodeTM );

	delete [] verts;

	return objNode;
}
