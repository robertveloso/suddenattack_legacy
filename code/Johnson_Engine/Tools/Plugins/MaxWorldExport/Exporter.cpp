//#include <fstream.h>
#include "MaxWorldExport.h"
#include "mnmath.h"
#include "decomp.h"
#include "notetrck.h"


// determine supported exporter options
BOOL MAXWorldExport::SupportsOptions( int ext, DWORD options )
{
	switch( ext )
	{
	case 0:		// .lta
	case 1:		// .ltc
		if( options == SCENE_EXPORT_SELECTED )
			return TRUE;
		else
			return FALSE;
		break;
	}

	return FALSE;
}

// given a dialog state, enable controls as appropriate
void UpdateEnabledControls( HWND hWnd )
{
	bool exportAnims = GetCheckBox( hWnd, IDC_EXPORT_ANIMATIONS ) > 0;
	bool forcedKeys = GetCheckBox( hWnd, IDC_FORCE_KEYS ) > 0;
	bool externalKeys = GetCheckBox( hWnd, IDC_EXTERNAL_KEY_FILE ) > 0;

	EnableWindow( GetDlgItem( hWnd, IDC_AS_PREFAB ), exportAnims );
	EnableWindow( GetDlgItem( hWnd, IDC_FORCE_KEYS ), exportAnims );
	EnableWindow( GetDlgItem( hWnd, IDC_FORCE_KEYS_FRAMES ), exportAnims );
	EnableWindow( GetDlgItem( hWnd, IDC_STATIC_FORCE_KEYS ), exportAnims );
	EnableWindow( GetDlgItem( hWnd, IDC_EXTERNAL_KEY_FILE ), exportAnims );

	EnableWindow( GetDlgItem( hWnd, IDC_INCLUDE_DEFINED_KEYS ), exportAnims && forcedKeys );

	EnableWindow( GetDlgItem( hWnd, IDC_EXTERNAL_KEY_FILE_PATH ), exportAnims && externalKeys );
	EnableWindow( GetDlgItem( hWnd, IDC_STATIC_EXTERNAL_KEY_FILE_PATH ), exportAnims && externalKeys );
}

// configuration dialog process
static BOOL CALLBACK ConfigDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	MAXWorldExport* export = (MAXWorldExport*)GetWindowLong( hWnd, GWL_USERDATA );
	if( !export && (msg != WM_INITDIALOG) )
		return FALSE;

	switch( msg )
	{
	case WM_INITDIALOG:
		{
		export = (MAXWorldExport*)lParam;
		// setup dialog controls to match settings
		SetCheckBox( hWnd, IDC_OBJECTSPACE, export->objectSpace );
		SetCheckBox( hWnd, IDC_TRIANGULATE_BRUSHES, export->triangulateBrushes );
		SetCheckBox( hWnd, IDC_EXPORT_ANIMATIONS, export->exportAnimations );
		SetCheckBox( hWnd, IDC_AS_PREFAB, export->asPrefab );
		SetCheckBox( hWnd, IDC_FORCE_KEYS, export->forceKeys );
		char buf[256];
		sprintf( buf, "%d", export->forceKeysFrames );
		SetWindowText( GetDlgItem( hWnd, IDC_FORCE_KEYS_FRAMES ), buf );
		SetCheckBox( hWnd, IDC_INCLUDE_DEFINED_KEYS, export->includeDefinedKeys );
		SetCheckBox( hWnd, IDC_EXTERNAL_KEY_FILE, export->externalKeyFile );
		SetWindowText( GetDlgItem( hWnd, IDC_EXTERNAL_KEY_FILE_PATH), export->keyFilePath );
		SetCheckBox( hWnd, IDC_EXPORT_UV, export->exportUVs );
		SetWindowText( GetDlgItem( hWnd, IDC_PROJECT_PATH ), export->projectPath );
		// disable controls as needed
		UpdateEnabledControls( hWnd );
		// put a pointer to the exporter class instance in the user data
		SetWindowLong( hWnd, GWL_USERDATA, lParam );
		CenterWindow( hWnd, GetParent( hWnd ) );
		break;
		}

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_CANCEL:
			EndDialog( hWnd, 1 );
			break;

		case IDC_OK:
			export->DialogAccepted( hWnd );
			EndDialog( hWnd, 0 );
			break;

		case IDC_EXPORT_ANIMATIONS:
			UpdateEnabledControls( hWnd );
			break;

		case IDC_FORCE_KEYS:
			UpdateEnabledControls( hWnd );
			break;

		case IDC_EXTERNAL_KEY_FILE:
			UpdateEnabledControls( hWnd );
			break;
			
		case IDC_BROWSE_PROJECT_PATH:
			{
			TCHAR dir[_MAX_PATH];
			GetWindowText( GetDlgItem( hWnd, IDC_PROJECT_PATH ), dir, _MAX_PATH );
			export->ip->ChooseDirectory( hWnd, GetString(IDS_CHOOSE_DIRECTORY), dir );
			if( strlen( dir ) > 0 )
				SetWindowText( GetDlgItem( hWnd, IDC_PROJECT_PATH ), dir );
			break;
			}
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

// progress bar dummy function
DWORD WINAPI ProgressBarUpdate( LPVOID arg )
{
	return 0;
}

// fill in settings based on export dialog
void MAXWorldExport::DialogAccepted( HWND hWnd )
{
	char keyFrames[256];

	objectSpace = GetCheckBox( hWnd, IDC_OBJECTSPACE ) > 0;
	triangulateBrushes = GetCheckBox( hWnd, IDC_TRIANGULATE_BRUSHES ) > 0;
	exportAnimations = GetCheckBox( hWnd, IDC_EXPORT_ANIMATIONS ) > 0;
	asPrefab = GetCheckBox( hWnd, IDC_AS_PREFAB ) > 0;
	forceKeys = GetCheckBox( hWnd, IDC_FORCE_KEYS ) > 0;
	GetWindowText( GetDlgItem( hWnd, IDC_FORCE_KEYS_FRAMES ), keyFrames, 255 );
	forceKeysFrames = atoi( keyFrames );
	if( forceKeysFrames <= 0 )
	{
		MaxMsgBox( ip->GetMAXHWnd(), GetString(IDS_MESSAGE_FORCEKEYSRANGE), GetString(IDS_MESSAGE_TITLE,true), MB_OK | MB_ICONWARNING );
		forceKeysFrames = 1;
	}
	includeDefinedKeys = GetCheckBox( hWnd, IDC_INCLUDE_DEFINED_KEYS ) > 0;
	externalKeyFile = GetCheckBox( hWnd, IDC_EXTERNAL_KEY_FILE ) > 0;
	GetWindowText( GetDlgItem( hWnd, IDC_EXTERNAL_KEY_FILE_PATH ), keyFilePath, _MAX_PATH );
	keyFilePathLen = strlen( keyFilePath );
	exportUVs = GetCheckBox( hWnd, IDC_EXPORT_UV ) > 0;
	GetWindowText( GetDlgItem( hWnd, IDC_PROJECT_PATH ), projectPath, _MAX_PATH );
	projectPathLen = strlen( projectPath );

	// make sure path ends in a backslash
	if( projectPathLen && projectPath[projectPathLen-1] != '\\' )
	{
		projectPath[projectPathLen] = '\\';
		projectPath[projectPathLen+1] = 0;
		projectPathLen++;
	}

	// make sure key file path ends in a backslash
	if( keyFilePathLen && keyFilePath[keyFilePathLen-1] != '\\' )
	{
		keyFilePath[keyFilePathLen] = '\\';
		keyFilePath[keyFilePathLen+1] = 0;
		keyFilePathLen++;
	}

	// make path lowercase
	for( int i = 0; i < projectPathLen; i++ )
	{
		if( isupper( projectPath[i] ) )
			projectPath[i] = tolower( projectPath[i] );
	}
}

// export to a file
int MAXWorldExport::DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options )
{
	this->ip = ip;

	// try to load the configuration info
	LoadExporterConfig();

	// make sure the extension is all lowercase in order to import into DEdit 2.2
	TSTR newName = LowercaseFileExtension( name );

	if( !suppressPrompts )
	{
		// show the configuration dialog
		int res = DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_EXPORT), ip->GetMAXHWnd(), ConfigDialogProc, (LPARAM)this );

		// they canceled
		if( res != 0 ) return 1;
	}

	// only exporting selected nodes?
	exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	// get the animation range
	animRange = ip->GetAnimRange();

	// start the progress bar
	ip->ProgressStart( GetString(IDS_PROGRESSBAR), TRUE, ProgressBarUpdate, NULL );

	// get the number of nodes in the scene to process
	numNodes = 0;
	PreProcess( ip->GetRootNode(), numNodes, NULL );

	// create the lta node tree
	LTAFile ltafile( externalKeyFile );

	// export the scene
	curNode = 0;
	int numChildren = ip->GetRootNode()->NumberOfChildren();
	for( int i = 0; i < numChildren; i++ )
	{
		if( !NodeEnum( ip->GetRootNode()->GetChildNode( i ), NULL, ltafile ) )
		{
			canceled = true;
			break;
		}
	}

	// get the external key file paths (project relative and full)
	TSTR newKeyRelative, newKeyFull, newKeyFullDir;
	ExternalKeyFileName( newName, newKeyRelative, newKeyFull, newKeyFullDir );

	// make sure the directory for the external key file exists (create it if not)
	if( !canceled && externalKeyFile )
	{
		// attempt to create the directory
		if( !CreateDirectory( newKeyFullDir, NULL ) )
		{
			if( GetLastError() != ERROR_ALREADY_EXISTS )
			{
				MaxMsgBox( ip->GetMAXHWnd(), GetString(IDS_MESSAGE_KEY_FILE_CREATEDIRECTORY_FAILED), GetString(IDS_MESSAGE_TITLE,true), MB_OK | MB_ICONERROR );
				canceled = true;
			}
		}
	}

	// export animated brushes
	if( !canceled && !ExportAnimNodes( ltafile, newKeyRelative ) )
		canceled = true;

	int retVal = 1;

	//save out the processed node tree
	if( !canceled )
	{
		if( !ltafile.Output( newName, newKeyFull ) )
			retVal = 0;
	}

	// close the progress bar
	ip->ProgressEnd();

	// save the config info
	SaveExporterConfig();

	if( numImproperFaces )
	{
		char buf[256];
		sprintf( buf, GetString(IDS_ENDREPORT_BADMATS), numImproperFaces );
		MessageBox( ip->GetMAXHWnd(), buf, GetString(IDS_ENDREPORT_BADMATS_TITLE), MB_OK|MB_ICONWARNING );
	}

	return retVal;
}

// make a first pass through the scene to count nodes, find animations, etc.
void MAXWorldExport::PreProcess( INode* node, int& nodeCount, AnimNode* parentAnimNode )
{
	AnimNode* curAnimNode = NULL;

	// if we are exporting animations, find the animated nodes
	if( exportAnimations )
	{
		curAnimNode = BuildAnimNode( node, parentAnimNode );

		if( curAnimNode )
			animNodes.push_back( curAnimNode );
	}

	nodeCount++;

	int numChildren = node->NumberOfChildren();

	// recurse through children
	for( int i = 0; i < numChildren; i++ )
	{
		PreProcess( node->GetChildNode( i ), nodeCount, curAnimNode );
	}
}

// export a specific node in the scene.  returns false if the user has canceled the export
bool MAXWorldExport::NodeEnum( INode* node, Box3* bounds, LTAFile& file, bool checkForAnim )
{
	assert( node );

	bool nodeAdded = false;

	// check if the user has pressed cancel
	if( ip->GetCancel() )
		return false;

	// don't add the node (or its children) here if it is animated
	if( checkForAnim && FindAnimNode( node ) )
		return true;

	// add the node (assuming it is selected if requested)
	if( !exportSelected || node->Selected() )
	{
		nodeAdded = ExportNode( node, bounds, file );
	}

	// update the progress bar
	curNode++;
	float complete = (float)curNode / (float)numNodes;
	ip->ProgressUpdate( int(complete * 100.0f) );

	// recurse through children
	for( int i = 0; i < node->NumberOfChildren(); i++ )
	{
		if( !NodeEnum( node->GetChildNode( i ), bounds, file ) )
			return false;
	}

	// back up to the correct level in the node tree
	if( nodeAdded )
		file.AscendTree();

	return true;
}

bool MAXWorldExport::ExportNode( INode* node, Box3* bounds, LTAFile& file )
{
	// group parent nodes are a special case so hierarchy is exported correctly
	if( node->IsGroupHead() )
	{
		file.AddObject( node->GetName(), false );
		return true;
	}

	// make sure it is an object we actually care about
	ObjectState objectState = node->EvalWorldState( 0 );
	if( !objectState.obj || (objectState.obj->SuperClassID() != GEOMOBJECT_CLASS_ID) )
		return false;
	if( objectState.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0) )
		return false;

	bool deleteObject = false;
	TriObject* tri = GetTriObjectFromNode( node, deleteObject );
	if( !tri )
		return false;

	Mtl* material = node->GetMtl();
	Matrix3 xform = node->GetObjTMAfterWSM( animRange.Start() );
	Mesh* mesh = &(tri->GetMesh());

	int numVerts = mesh->getNumVerts();
	int numFaces = mesh->getNumFaces();

	bool retVal = true;

	// don't add objects that have no verts or faces
	if( numVerts && numFaces )
	{
		// update the bounding box to include this nodes geometry
		if( bounds )
		{
			for( int i = 0; i < numVerts; i++ )
			{
				Point3 curVert = mesh->getVert( i );
				if( !objectSpace )
					curVert = curVert * xform;

				*bounds += curVert;
			}
		}

		// add the object to the node hierarchy
		file.AddObject( node->GetName(), true );

		if( triangulateBrushes )
			ExportNodeAsTriBrush( mesh, material, xform, file );
		else
			ExportNodeAsBrush( mesh, material, xform, file );
	}
	else
		retVal = false;

	if( deleteObject )
		delete tri;

	return retVal;
}


bool MAXWorldExport::GetTextureInfo( BitmapTex* tex, int texNum, MultiTextureInfo& texInfo )
{
	ASSERT( tex && texNum < 2 && texNum >= 0 );

	Bitmap* bitmap = tex->GetBitmap( 0 );
	if( !bitmap )
		return false;

	// get the name of the texture file
	TSTR texName = tex->GetMapName();
	if( !GenerateTextureName( texName ) )
		return false;
	texInfo.texName[texNum] = texName;

	// get the texture dimensions
	texInfo.mapWidth[texNum] = bitmap->Width();
	texInfo.mapHeight[texNum] = bitmap->Height();

	if( !(texInfo.mapWidth[texNum] > 0 && texInfo.mapHeight[texNum] > 0) )
		return false;

	// get the texture transform
	StdUVGen* uvGen = tex->GetUVGen();
	uvGen->GetUVTransform( texInfo.uvXForm[texNum] );

	// get the mapping coordinate channel
	switch( tex->GetUVWSource() )
	{
	case UVWSRC_EXPLICIT:
		texInfo.mapChannel[texNum] = tex->GetMapChannel();
		break;
	case UVWSRC_EXPLICIT2:
		texInfo.mapChannel[texNum] = 0;		// vertex color special case
		break;
	default:
		return false;
	}

	return true;
}


void MAXWorldExport::GetMultiTextureInfo( Mtl* material, int matID, MultiTextureInfo& texInfo )
{
	// start off with a sane default
	texInfo.numTextures = 0;

	// if this is a multi/sub-object material, grab the appropriate one based on the material ID
	if( material && (material->NumSubMtls() != 0) && (material->ClassID() == Class_ID(MULTI_CLASS_ID,0)) )
		material = material->GetSubMtl( matID % material->NumSubMtls() );

	// no valid standard material found
	if( !material || (material->ClassID() != Class_ID(DMTL_CLASS_ID,0)) )
		return;

	// grab the diffuse texture
	Texmap* tex = material->GetSubTexmap( ID_DI );

	if( !tex )
		return;

	if( tex->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
	{
		// grab the info for this bitmap texture
		if( GetTextureInfo( (BitmapTex*)tex, 0, texInfo ) )
			texInfo.numTextures = 1;
	}
	else if( tex->NumSubTexmaps() >= 2 )
	{
		// attempt to fill in info for the first subtexture
		Texmap* subTex = tex->GetSubTexmap( 0 );
		if( !subTex || (subTex->ClassID() != Class_ID(BMTEX_CLASS_ID,0)) || !GetTextureInfo( (BitmapTex*)subTex, 0, texInfo ) )
			return;

		// attempt to fill in info for the second subtexture
		subTex = tex->GetSubTexmap( 1 );
		if( !subTex || (subTex->ClassID() != Class_ID(BMTEX_CLASS_ID,0)) || !GetTextureInfo( (BitmapTex*)subTex, 1, texInfo ) )
			return;

		texInfo.numTextures = 2;
	}
}


void MAXWorldExport::ExportNodeAsTriBrush( Mesh* mesh, Mtl* material, Matrix3& xform, LTAFile& file )
{
	int i;

	// variables used for exporting a face
	Point3 v0, v1, v2;
	Point3 tv0, tv1, tv2;
	Point3 pos;
	int indexList[3];

	int numVerts = mesh->getNumVerts();
	int numFaces = mesh->getNumFaces();
	bool negParity = xform.Parity() > 0;

	if( numFaces < 1 )
		return;

	// change vertex ordering based on transform parity
	int vx0, vx1, vx2;	// face->vertex indices
	if( negParity )
	{
		vx0 = 0;
		vx1 = 1;
		vx2 = 2;
	}
	else
	{
		vx0 = 2;
		vx1 = 1;
		vx2 = 0;
	}

	LTAVert* vertList = new LTAVert[numVerts];

	// fill in the vertlist from the mesh
	for( i = 0; i < numVerts; i++ )
	{
		pos = mesh->getVert( i );

		if( !objectSpace )
			pos = pos * xform;

		vertList[i].pos = ConvertPoint( pos );
		vertList[i].r = vertList[i].g = vertList[i].b = vertList[i].a = 255;
	}

	// fill in the vertex color info if it exists
	if( mesh->mapSupport( 0 ) )
	{
		TVFace* colorFaces = mesh->mapFaces( 0 );
		UVVert* colorVerts = mesh->mapVerts( 0 );

		for( i = 0; i < numFaces; i++ )
		{
			tv0 = colorVerts[colorFaces[i].t[vx0]];
			tv1 = colorVerts[colorFaces[i].t[vx1]];
			tv2 = colorVerts[colorFaces[i].t[vx2]];
			
			Face& curFace = mesh->faces[i];
			vertList[curFace.v[vx0]].r = (uint8)(255.0f * tv0.x);
			vertList[curFace.v[vx0]].g = (uint8)(255.0f * tv0.y);
			vertList[curFace.v[vx0]].b = (uint8)(255.0f * tv0.z);
			vertList[curFace.v[vx0]].a = (uint8)(255.0f * ((tv0.x + tv0.y + tv0.z) / 3.0f));
			vertList[curFace.v[vx1]].r = (uint8)(255.0f * tv1.x);
			vertList[curFace.v[vx1]].g = (uint8)(255.0f * tv1.y);
			vertList[curFace.v[vx1]].b = (uint8)(255.0f * tv1.z);
			vertList[curFace.v[vx1]].a = (uint8)(255.0f * ((tv1.x + tv1.y + tv1.z) / 3.0f));
			vertList[curFace.v[vx2]].r = (uint8)(255.0f * tv2.x);
			vertList[curFace.v[vx2]].g = (uint8)(255.0f * tv2.y);
			vertList[curFace.v[vx2]].b = (uint8)(255.0f * tv2.z);
			vertList[curFace.v[vx2]].a = (uint8)(255.0f * ((tv2.x + tv2.y + tv2.z) / 3.0f));
		}
	}

	// add the vertex list to the lta file
	file.AddBrushVertices( numVerts, vertList );

	// add the faces to the mesh
	for( i = 0; i < numFaces; i++ )
	{
		LTAPoly curPoly;

		Face& curFace = mesh->faces[i];

		// generate plane equation info
		v0 = vertList[curFace.v[vx0]].pos;
		v1 = vertList[curFace.v[vx1]].pos;
		v2 = vertList[curFace.v[vx2]].pos;
		if( negParity )
			curPoly.n = CrossProd( (v1-v0), (v2-v0) );
		else
			curPoly.n = CrossProd( (v2-v0), (v1-v0) );
		curPoly.n = Normalize( curPoly.n );
		curPoly.d = DotProd( curPoly.n, v0 );

		indexList[0] = curFace.v[vx0];
		indexList[1] = curFace.v[vx1];
		indexList[2] = curFace.v[vx2];

		curPoly.indices = indexList;
		curPoly.numVerts = 3;

		// add the texture info
		if( exportUVs )
		{
			MultiTextureInfo texInfo;
			GetMultiTextureInfo( material, curFace.getMatID(), texInfo );

			bool goodTexInfo = (texInfo.numTextures != 0);

			TVFace* mapFaces[2];
			UVVert* mapVerts[2];
			mapFaces[0] = mapFaces[1] = NULL;
			mapVerts[0] = mapVerts[1] = NULL;

			if( texInfo.numTextures >= 1 )
			{
				if( mesh->mapSupport( texInfo.mapChannel[0] ) )
				{
					mapFaces[0] = mesh->mapFaces( texInfo.mapChannel[0] );
					mapVerts[0] = mesh->mapVerts( texInfo.mapChannel[0] );
				}

				if( !(mapFaces[0] && mapVerts[0]) )
					goodTexInfo = false;
			}
			if( texInfo.numTextures >= 2 )
			{
				if( mesh->mapSupport( texInfo.mapChannel[1] ) )
				{
					mapFaces[1] = mesh->mapFaces( texInfo.mapChannel[1] );
					mapVerts[1] = mesh->mapVerts( texInfo.mapChannel[1] );
				}

				if( !(mapFaces[1] && mapVerts[1]) )
					goodTexInfo = false;
			}

			for( int j = 0; goodTexInfo && (j < texInfo.numTextures); j++ )
			{
				tv0 = mapVerts[j][mapFaces[j][i].t[vx0]] * texInfo.uvXForm[j];
				tv1 = mapVerts[j][mapFaces[j][i].t[vx1]] * texInfo.uvXForm[j];
				tv2 = mapVerts[j][mapFaces[j][i].t[vx2]] * texInfo.uvXForm[j];

				GenerateOPQ( v0, v1, v2, tv0, tv1, tv2, texInfo.mapWidth[j], texInfo.mapHeight[j], curPoly.o[j], curPoly.p[j], curPoly.q[j] );

				curPoly.textureName[j] = texInfo.texName[j];
			}
		}

		file.AddBrushPolygon( curPoly );
	}

	delete [] vertList;
}

void MAXWorldExport::ExportNodeAsBrush( Mesh* mesh, Mtl* material, Matrix3& xform, LTAFile& file )
{
	// variables used for exporting a face
	Point3 v0, v1, v2;
	Point3 tv0, tv1, tv2;
	Point3 cv;
	Point3 pos;
	int i;

	// create the connectivity mesh
	MNMesh topoMesh( *mesh );
	topoMesh.MakeConvexPolyMesh();
	topoMesh.MakeConvex();
	topoMesh.CollapseDeadStructs();

	if( topoMesh.FNum() < 1 )
		return;

	bool negParity = xform.Parity() > 0;

	// list of vertex positions to add to the lta file
	LTAVert* vertList = new LTAVert[topoMesh.VNum()];

	// fill in the vertlist from the mesh
	for( i = 0; i < topoMesh.VNum(); i++ )
	{
		pos = topoMesh.P( i );

		if( !objectSpace )
			pos = pos * xform;

		vertList[i].pos = ConvertPoint( pos );
		vertList[i].r = vertList[i].g = vertList[i].b = vertList[i].a = 255;
	}

	// fill in vertex color info if it exists
	for( i = 0; i < topoMesh.FNum(); i++ )
	{
		if( MNMapFace* colorFace = topoMesh.MF( 0, i ) )
		{
			MNFace* curFace = topoMesh.F( i );
			
			for( int j = 0; j < curFace->deg; j++ )
			{
				cv = topoMesh.MV( 0, colorFace->tv[j] );
				vertList[curFace->vtx[j]].r = (uint8)(255.0f * cv.x);
				vertList[curFace->vtx[j]].g = (uint8)(255.0f * cv.y);
				vertList[curFace->vtx[j]].b = (uint8)(255.0f * cv.z);
				vertList[curFace->vtx[j]].a = (uint8)(255.0f * ((cv.x + cv.y + cv.z) / 3.0f));
			}
		}
	}

	// add the vertex list to the lta file
	file.AddBrushVertices( topoMesh.VNum(), vertList );

	// the static vertex index list
	int fixedIndexList[STATIC_VERT_INDEX_SIZE];

	// add the faces in the mesh
	for( i = 0; i < topoMesh.FNum(); i++ )
	{
		LTAPoly curPoly;

		MNFace* curFace = topoMesh.F( i );

		// generate plane equation info
		v0 = vertList[curFace->vtx[0]].pos;
		v1 = vertList[curFace->vtx[1]].pos;
		v2 = vertList[curFace->vtx[2]].pos;
		if( negParity )
			curPoly.n = CrossProd( (v1-v0), (v2-v0) );
		else
			curPoly.n = CrossProd( (v2-v0), (v1-v0) );
		curPoly.n = Normalize( curPoly.n );
		curPoly.d = DotProd( curPoly.n, v0 );

		// add the texture info
		if( exportUVs )
		{
			MultiTextureInfo texInfo;
			GetMultiTextureInfo( material, curFace->material, texInfo );

			bool goodTexInfo = (texInfo.numTextures != 0);

			MNMapFace* mapFace[2];
			mapFace[0] = mapFace[1] = NULL;

			if( texInfo.numTextures >= 1 )
			{
				mapFace[0] = topoMesh.MF( texInfo.mapChannel[0], i );
				if( !mapFace[0] )
					goodTexInfo = false;
			}
			if( texInfo.numTextures >= 2 )
			{
				mapFace[1] = topoMesh.MF( texInfo.mapChannel[1], i );
				if( !mapFace[1] )
					goodTexInfo = false;
			}

			for( int j = 0; goodTexInfo && (j < texInfo.numTextures); j++ )
			{
				tv0 = topoMesh.MV( texInfo.mapChannel[j], mapFace[j]->tv[0] ) * texInfo.uvXForm[j];
				tv1 = topoMesh.MV( texInfo.mapChannel[j], mapFace[j]->tv[1] ) * texInfo.uvXForm[j];
				tv2 = topoMesh.MV( texInfo.mapChannel[j], mapFace[j]->tv[2] ) * texInfo.uvXForm[j];

				GenerateOPQ( v0, v1, v2, tv0, tv1, tv2, texInfo.mapWidth[j], texInfo.mapHeight[j], curPoly.o[j], curPoly.p[j], curPoly.q[j] );

				curPoly.textureName[j] = texInfo.texName[j];
			}
		}

		// dynamically create the index list if there are too many verts
		if( curFace->deg > STATIC_VERT_INDEX_SIZE )
			curPoly.indices = new int[curFace->deg];
		else
			curPoly.indices = fixedIndexList;

		// setup the order we index the verts in based on transform parity
		int curIndex, indexDelta;
		if( !negParity )
		{
			curIndex = curFace->deg - 1;
			indexDelta = -1;
		}
		else
		{
			curIndex = 0;
			indexDelta = 1;
		}

		// add indices to the index list
		for( int j = 0; j < curFace->deg; j++ )
		{
			curPoly.indices[curIndex] = curFace->vtx[j];
			curIndex += indexDelta;
		}

		curPoly.numVerts = curFace->deg;

		// add the polygon to the lta file
		file.AddBrushPolygon( curPoly );

		// delete dynamically allocated index list
		if( curFace->deg > STATIC_VERT_INDEX_SIZE )
			delete [] curPoly.indices;
	}

	delete [] vertList;
}

TriObject* MAXWorldExport::GetTriObjectFromNode( INode* node, bool& deleteIt )
{
	deleteIt = false;
	Object* obj = node->EvalWorldState( 0 ).obj;
	if( obj->CanConvertToType( Class_ID(TRIOBJ_CLASS_ID, 0) ) )
	{
		TriObject* tri = (TriObject*)obj->ConvertToType( 0, Class_ID(TRIOBJ_CLASS_ID, 0) );
		// only delete the triobject if it isn't the same as the object
		if( obj != tri ) deleteIt = true;
		return tri;
	}
	else
	{
		return NULL;
	}
}

double OPQArea( const DPoint3& p0, const DPoint3& p1, const DPoint3& p2 )
{
	DPoint3 e0 = p1 - p0;
	DPoint3 e1 = p2 - p0;

	return (e0.x * e1.y - e1.x * e0.y);
}

DPoint3 OPQBaryCoords( const DPoint3& p0, const DPoint3& p1, const DPoint3& p2, const DPoint3& p )
{
	double n = OPQArea( p0, p1, p2 );

	if( fabs(n) < 0.00000001 )
		return DPoint3( 1.0, 0.0, 0.0 );

	double u = OPQArea( p1, p2, p ) / n;
	double v = OPQArea( p2, p0, p ) / n;
	double w = 1.0 - u - v;

	return DPoint3( u, v, w );
}

void MAXWorldExport::GenerateOPQ( const Point3& v0, const Point3& v1, const Point3& v2, const Point3& tv0, const Point3& tv1, const Point3& tv2, int width, int height, Point3& o, Point3& p, Point3& q )
{
	assert( width > 0 && height > 0 );

	// vertex positions in texture space
	DPoint3 uv0( tv0.x, -tv0.y, 0.0 );
	DPoint3 uv1( tv1.x, -tv1.y, 0.0 );
	DPoint3 uv2( tv2.x, -tv2.y, 0.0 );

	// determine barycentric coordinates of OPQ in texture space
	DPoint3 bcO = OPQBaryCoords( uv0, uv1, uv2, DPoint3( 0.0, 0.0, 0.0 ) );
	DPoint3 bcP = OPQBaryCoords( uv0, uv1, uv2, DPoint3( 1.0, 0.0, 0.0 ) );
	DPoint3 bcQ = OPQBaryCoords( uv0, uv1, uv2, DPoint3( 0.0, 1.0, 0.0 ) );

	// double versions of world space verts
	DPoint3 dv0 = v0;
	DPoint3 dv1 = v1;
	DPoint3 dv2 = v2;

	// calculate OPQ in world space
	DPoint3 O = bcO.x*dv0 + bcO.y*dv1 + bcO.z*dv2;
	DPoint3 P = bcP.x*dv0 + bcP.y*dv1 + bcP.z*dv2;
	DPoint3 Q = bcQ.x*dv0 + bcQ.y*dv1 + bcQ.z*dv2;
	P = P - O;
	Q = Q - O;

	double tp = Length( P );
	tp *= 1.0 / (double)width;
	tp = 1.0 / tp;
	double tq = Length( Q );
	tq *= 1.0 / (double)height;
	tq = 1.0 / tq;

	P = Normalize( P );
	Q = Normalize( Q );

	//fix up p and q to be what DEdit really wants
	DPoint3 R = CrossProd( Q, P );
	DPoint3 PNew = CrossProd( R, Q );
	DPoint3 QNew = CrossProd( P, R );

	// fix up scale factors for new p and q
	PNew = Normalize( PNew );
	QNew = Normalize( QNew );
	double pscale = 1.0 / DotProd( P, PNew );
	double qscale = 1.0 / DotProd( Q, QNew );

	R = CrossProd( QNew, PNew );

	PNew *= tp * pscale;
	QNew *= tq * qscale;

	// orthogonalize P and Q
	R = Normalize( R );
	P = PNew + R;
	Q = QNew - (DotProd( PNew, QNew ) * R);

	o = Point3( O.x, O.y, O.z );
	p = Point3( P.x, P.y, P.z );
	q = Point3( Q.x, Q.y, Q.z );
}

// strip the base path off of the texture name, if a bogus name results, return false
bool MAXWorldExport::GenerateTextureName( TSTR& name )
{
	char newdir[_MAX_PATH];
	char fulldir[_MAX_PATH];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char path[_MAX_PATH];

	strcpy( path, name );
	int pathLen = strlen( path );
	for( int i = 0; i < pathLen; i++ )
	{
		if( isupper( path[i] ) )
			path[i] = tolower( path[i] );
	}

	// fulldir starts out being just the drive
	_splitpath( path, fulldir, dir, fname, ext );
	strcat( fulldir, dir );

	int fulldirLen = strlen( fulldir );

	// oops, the base path is longer than the texture directory path
	if( fulldirLen < projectPathLen )
		return false;

	// copy off the last part of the directory
	strcpy( newdir, fulldir + projectPathLen );

	// test to make sure the roots of each of the paths are identical
	fulldir[projectPathLen] = 0;
	if( strcmp( fulldir, projectPath ) )
		return false;

	// create the new texture name
	sprintf( path, "%s%s.dtx", newdir, fname );
	name = path;

	return true;
}


// make sure the extension is all lowercase in order to import into DEdit 2.2
TSTR MAXWorldExport::LowercaseFileExtension( const TCHAR* name )
{
	char newdir[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( name, drive, dir, fname, ext );

	// convert the extension to lowercase
	int extLen = strlen( ext );
	for( int i = 0; i < extLen; i++ )
	{
		if( isupper( ext[i] ) )
			ext[i] = tolower( ext[i] );
	}

	sprintf( newdir, "%s%s%s%s", drive, dir, fname, ext );

	return newdir;
}


// create the external key file name based on the filename and key path offset
void MAXWorldExport::ExternalKeyFileName( const TCHAR* name, TSTR& projectRelative, TSTR& fullPath, TSTR& fullPathDir )
{
	char projRel[_MAX_PATH];
	char full[_MAX_PATH];
	char fullDir[_MAX_PATH];
	char fname[_MAX_FNAME];

	_splitpath( name, NULL, NULL, fname, NULL );

	sprintf( full, "%s%s%sKeys.ltc", projectPath, keyFilePath, fname );
	sprintf( projRel, "%s%sKeys.ltc", keyFilePath, fname );
	sprintf( fullDir, "%s%s", projectPath, keyFilePath );

	projectRelative = projRel;
	fullPath = full;
	fullPathDir = fullDir;
}


// convert a max matrix to a position and euler rotation in lithtech space
void MAXWorldExport::ConvertMatrix( const Matrix3& source, Point3& position, Point3& eulerRotation )
{
	position = ConvertPoint( source.GetTrans() );

	float euler[3];
	MatrixToEuler( source, euler, EULERTYPE_ZXY | EULERTYPE_RF );

	eulerRotation.y = -euler[0];
	eulerRotation.x = -euler[1];
	eulerRotation.z = -euler[2];
}


// export the animated brushes
bool MAXWorldExport::ExportAnimNodes( LTAFile& file, TSTR& keyFilePath )
{
	for( std::list<AnimNode*>::iterator it = animNodes.begin(); it != animNodes.end(); it++ )
	{
		if( !ExportAnimNode( *it, file, keyFilePath ) )
			return false;
	}

	if( !ExportKeyFramerGroups( file ) )
		return false;

	return true;
}


// export a specific animnode
bool MAXWorldExport::ExportAnimNode( AnimNode* animNode, LTAFile& file, TSTR& keyFilePath )
{
	assert( animNode && animNode->objNode );

	// get the root position of the object
	Matrix3 nodeMat = animNode->objNode->GetObjTMAfterWSM( animRange.Start() );
	Point3 nodePos = ConvertPoint( nodeMat.GetTrans() );

	// add the worldmodel object
	TSTR baseName( animNode->objNode->GetName() );
	TSTR worldModelName = baseName + "WM";
	file.AddWorldModel( worldModelName, nodePos );

	// bounding box for the animnode geometry
	Box3 bounds;

	// add the node and its children
	if( !NodeEnum( animNode->objNode, &bounds, file, false ) )
		return false;

	// build the offset transform for the keys (worldnodes rotate about their frame 0 AABB center)
	Matrix3 offset( TRUE );
	offset.SetTrans( bounds.Center() );
	offset = offset * Inverse( nodeMat );

	// pop the worldmodel object
	file.AscendTree();

	// add the keyframer object
	TSTR keyFramerName = baseName + "KF";
	TSTR keyBaseName;
	
	keyBaseName = baseName + "Key";

	TSTR keyFramerKeyBaseName;
	TSTR keyFramerWorldModelName;

	if( asPrefab )
	{
		keyFramerWorldModelName = TSTR("#.") + worldModelName;
		if( !externalKeyFile )
			keyFramerKeyBaseName = TSTR("#.") + keyBaseName;
		else
			keyFramerKeyBaseName = keyBaseName;
	}
	else
	{
		keyFramerKeyBaseName = keyBaseName;
		keyFramerWorldModelName = worldModelName;
	}

	file.AddKeyFramer( keyFramerName, keyFramerWorldModelName, keyFramerKeyBaseName, keyFilePath, nodePos );

	int curKeyNum = 0;
	TSTR curKeyName;
	TimeValue prevKeyTime, curTimeInterval;

	// loop through the key times and add key nodes for them
	for( std::list<TimeValue>::iterator it = animNode->keys.begin(); it != animNode->keys.end(); it++ )
	{
		// first key time is the difference between two keys (last and first)
		if( !curKeyNum )
		{
			curTimeInterval = GetTicksPerFrame();
		}
		else
		{
			curTimeInterval = *it - prevKeyTime;
		}

		prevKeyTime = *it;

		// get the name for this key
		if( curKeyNum == 0 )
			curKeyName.printf( "%s0", keyBaseName, curKeyNum );
		else
			curKeyName.printf( "%s%02d", keyBaseName, curKeyNum );
		curKeyNum++;

		// attempt to find note track info for this key
		const char* keyNote = NULL;
		std::map<TimeValue,TSTR>::iterator noteIt = animNode->noteKeys.find( *it );
		if( noteIt != animNode->noteKeys.end() )
			keyNote = noteIt->second;

		ExportNodeKey( animNode->objNode, *it, curTimeInterval, keyNote, offset, nodePos, curKeyName, file );
	}

	// pop the keyframer object
	file.AscendTree();

	return true;
}


// export a key for this node
bool MAXWorldExport::ExportNodeKey( INode* node, TimeValue keyTime, TimeValue timeInterval, const char* keyNote, const Matrix3& offset, const Point3& kfPos, const char* keyName, LTAFile& file )
{
	assert( node && keyName && keyTime >= animRange.Start() && keyTime <= animRange.End() );

	// recover lithtech space position and euler rotations
	Point3 pos, eulerRotation;

	Matrix3 nodeMat = offset * node->GetObjTMAfterWSM( keyTime );
	ConvertMatrix( nodeMat, pos, eulerRotation );

	// convert ticks to seconds (4800 ticks per second)
	float timeInSeconds = TicksToSec( timeInterval );

	// parse the key note into command and sound info
	TSTR command;
	TSTR sound;
	float soundRadius = 0.0f;

	ParseKeyNote( keyNote, command, sound, soundRadius );

	// add the key object
	file.AddKey( keyName, timeInSeconds, kfPos, pos, eulerRotation, sound, soundRadius, command );

	// pop the key object if we added it to the main world file
	if( !externalKeyFile )
		file.AscendTree();

	return true;
}


bool MAXWorldExport::ExportKeyFramerGroups( LTAFile& file )
{
	int curGroup = 0;

	std::list<AnimNode*>::iterator it = animNodes.begin();

	// loop through all animnodes adding them to a group
	while( it != animNodes.end() )
	{
		// make space for the keyframer names
		std::vector<TSTR> keyFramerNames;
		keyFramerNames.reserve( 50 );

		// only 50 nodes to a group, get their names
		for( int numNodesInGroup = 0; (it != animNodes.end()) && (numNodesInGroup < 50); numNodesInGroup++, it++ )
		{
			AnimNode* curAnimNode = *it;
			TSTR baseName( curAnimNode->objNode->GetName() );
			TSTR keyFramerName = baseName + "KF";

			if( asPrefab )
				keyFramerName = TSTR("#.") + keyFramerName;

			keyFramerNames.push_back( keyFramerName );
		}

		// name the group
		TSTR groupName( "KeyFramers" );
		if( curGroup || it != animNodes.end() )
		{
			TSTR groupNumStr;
			groupNumStr.printf( "%d", curGroup );
			groupName += groupNumStr;
		}

		// add the group
		file.AddKeyFramerGroup( groupName, keyFramerNames );
		file.AscendTree();

		// move on to the next group
		curGroup++;
	}

	return true;
}


// given a max node, find the matching animnode if one exists, otherwise NULL
AnimNode* MAXWorldExport::FindAnimNode( const INode* node )
{
	for( std::list<AnimNode*>::iterator it = animNodes.begin(); it != animNodes.end(); it++ )
	{
		AnimNode* curAnimNode = *it;
		assert( curAnimNode );

		if( curAnimNode->objNode == node )
			return curAnimNode;
	}

	return NULL;
}


// take a max node and fill in the keyframes that it uses, returns NULL if the node is not animated
AnimNode* MAXWorldExport::BuildAnimNode( INode* node, const AnimNode* parentAnimNode )
{
	AnimNode* curAnimNode = new AnimNode;
	curAnimNode->objNode = node;

	// if exporting selected and node isn't selected, don't check for animations
	if( exportSelected && !(node->Selected()) )
	{
		delete curAnimNode;
		return NULL;
	}

	if( !BuildNodeKeyTimes( curAnimNode ) )
	{
		// the node isn't animated
		delete curAnimNode;
		return NULL;
	}

	// copy the parents keys if we want them
	if( parentAnimNode && (includeDefinedKeys || !forceKeys) )
	{
		for( std::list<TimeValue>::const_iterator it = parentAnimNode->keys.begin(); it != parentAnimNode->keys.end(); it++ )
		{
			curAnimNode->keys.push_back( *it );
		}
	}

	// clear out keyframed keys, we're only exporting forced interval keys
	if( forceKeys && !includeDefinedKeys )
		curAnimNode->keys.clear();

	TimeValue beginTime = animRange.Start();
	TimeValue endTime = animRange.End();

	// set forced keys
	if( forceKeys )
	{
		// number of ticks in each interval we are forcing a frame
		int ticksPerInterval = forceKeysFrames * GetTicksPerFrame();

		for( TimeValue curTime = beginTime; curTime < endTime; curTime += ticksPerInterval )
		{
			curAnimNode->keys.push_back( curTime );
		}
	}

	// set begin and end keys
	curAnimNode->keys.push_front( beginTime );
	curAnimNode->keys.push_back( endTime );

	// make keys unique and sorted
	curAnimNode->keys.sort();
	curAnimNode->keys.unique();

	return curAnimNode;
}


// find the keys set for this node, returns false if no keys were found for the node (non-recursive check)
bool MAXWorldExport::BuildNodeKeyTimes( AnimNode* animNode )
{
	int i;
	int keysAdded = 0;

	assert( animNode && animNode->objNode );
	animNode->keys.clear();

	INode* node = animNode->objNode;
	Control* tmController = node->GetTMController();

	if( !tmController )
		return false;

	Control* controllers[3];
	controllers[0] = tmController->GetPositionController();
	controllers[1] = tmController->GetRotationController();
	controllers[2] = tmController->GetScaleController();

	for( i = 0; i < 3; i++ )
	{
		if( !controllers[i] )
			continue;

		int numKeys = controllers[i]->NumKeys();

		for( int curKey = 0; curKey < numKeys; curKey++ )
		{
			TimeValue curTime = controllers[i]->GetKeyTime( curKey );
			if( curTime >= animRange.Start() && curTime <= animRange.End() )
			{
				animNode->keys.push_back( curTime );
				keysAdded++;
			}
		}
	}

	// don't check for note tracks if the object isn't otherwise animated
	if( !keysAdded )
		return 0;

	// check for note tracks
	int numNoteTracks = 0;
	if( node->HasNoteTracks() )
		numNoteTracks = node->NumNoteTracks();

	for( i = 0; i < numNoteTracks; i++ )
	{
		DefNoteTrack* curNoteTrack = (DefNoteTrack*)node->GetNoteTrack( i );

		if( !curNoteTrack )
			continue;

		int numNoteKeys = curNoteTrack->keys.Count();

		for( int curNoteKey = 0; curNoteKey < numNoteKeys; curNoteKey++ )
		{
			TimeValue curTime = curNoteTrack->keys[curNoteKey]->time;
			if( curTime >= animRange.Start() && curTime <= animRange.End() )
			{
				// add the note info to the note map
				animNode->noteKeys.insert( std::make_pair( curTime, curNoteTrack->keys[curNoteKey]->note ) );

				// add the time to the key list
				animNode->keys.push_back( curTime );
				keysAdded++;
			}
		}
	}

	return keysAdded > 0;
}


// given a key note string, break it into a command, sound, and sound radius if any exist
bool MAXWorldExport::ParseKeyNote( const char* keyNote, TSTR& command, TSTR& sound, float& soundRadius )
{
	if( !keyNote )
		return false;

	int len = strlen( keyNote );

	// find command
	const char* curPos = keyNote;
	while( (curPos - keyNote) <= (len - 3) )
	{
		const char* cmdEnd = NULL;
		// found the command start, now find the end
		if( (curPos[0] == 'c') && (curPos[1] == '\"') )
		{
			cmdEnd = curPos + 2;
			while( (*cmdEnd != '\"') && (cmdEnd - keyNote < len) )
				cmdEnd++;
		}

		if( cmdEnd )
		{
			// copy the command string
			curPos = curPos + 2;
			char* tmpCommand = new char[cmdEnd-curPos+1];
			char* tmpCommandPos = tmpCommand;
			tmpCommand[cmdEnd-curPos] = '\0';
			while( curPos < cmdEnd )
				*(tmpCommandPos++) = *(curPos++);

			command = tmpCommand;
			delete [] tmpCommand;
			break;
		}

		curPos++;
	}

	// find sound
	curPos = keyNote;
	while( (curPos - keyNote) <= (len - 3) )
	{
		const char* soundEnd = NULL;
		// found the sound start, now find the end
		if( (curPos[0] == 's') && (curPos[1] == '\"') )
		{
			soundEnd = curPos + 2;
			while( (*soundEnd != '\"') && (soundEnd - keyNote < len) )
				soundEnd++;
		}

		if( soundEnd )
		{
			// copy the sound string
			curPos = curPos + 2;
			char* tmpSound = new char[soundEnd-curPos+1];
			char* tmpSoundPos = tmpSound;
			tmpSound[soundEnd-curPos] = '\0';
			while( curPos < soundEnd )
				*(tmpSoundPos++) = *(curPos++);

			sound = tmpSound;
			delete [] tmpSound;
			break;
		}

		curPos++;
	}

	// find sound radius
	curPos = keyNote;
	while( (curPos - keyNote) <= (len - 4) )
	{
		const char* radiusEnd = NULL;
		// found the radius start, now find the end
		if( (curPos[0] == 's') && (curPos[1] == 'r') && (curPos[2] == '\"') )
		{
			radiusEnd = curPos + 3;
			while( (*radiusEnd != '\"') && (radiusEnd - keyNote < len) )
				radiusEnd++;
		}

		if( radiusEnd )
		{
			// copy the radius string
			curPos = curPos + 3;
			char* tmpRadius = new char[radiusEnd-curPos+1];
			char* tmpRadiusPos = tmpRadius;
			tmpRadius[radiusEnd-curPos] = '\0';
			while( curPos < radiusEnd )
				*(tmpRadiusPos++) = *(curPos++);

			soundRadius = (float)atof( tmpRadius );
			delete [] tmpRadius;
			break;
		}

		curPos++;
	}

	return true;
}
