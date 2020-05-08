#ifndef __PROCESSING_H__
#define __PROCESSING_H__


	class CPreWorld;
	class CEditBrush;
	class CBspHullMaker;
	class CWorldNode;


	// ------------------------------------------------------ //
	// Here's all the functions and variables the 
	// system-dependent stuff gets to handle.
	// ------------------------------------------------------ //

	#define MAX_THREADS					40
	#define MAX_PLATFORM_NAME_LEN		256

	class CProcessorGlobs
	{
	public:

		// When creating the WorldTree, these are the sizes it uses to determine whether
		// or not to continue subdividing
		PReal	m_WorldBoxSize;


		// Maximum lightmap size.  Comes from the world info string or DEFAULT_LIGHTMAP_PIXELS.
		PReal	m_MaxLMSize; 

		// Set to TRUE if it's making a world model (so the debug output can
		// be toned down a little).
		bool	m_bMakingWorldModel;

		// Maximum size of the world.  Objects outside this bound are removed
		// and brushes outside it are considered problem brushes.
		PReal	m_MaxBounds;
		
		// These should be set by the caller.
		char	**m_Argv;
		uint32	m_Argc;
		char	m_InputFile[MAX_PATH]; // Input filename (like worlds\test1.lta).
		char	m_OutputFile[MAX_PATH]; // Full output filename (like worlds\test1.dat).

		char	m_ProjectDir[MAX_PATH];

		//this is the platform we will be compiling for
		char	m_Platform[MAX_PLATFORM_NAME_LEN];

		// Selects brushes with polies that caused problems in hull generation.
		bool	m_bSelectHullProblemBrushes;

		// Removes problem objects and saves the ED file.
		bool	m_bRemoveProblemObjects;

		// Leak filename (0 length if none).
		char	m_LeakFilename[MAX_PATH];

		// Portal filename (0 length if none).
		char	m_PortalFilename[MAX_PATH];

		//for visibility calculations. Determines if portals should be culled based
		//upon distance
		bool	m_bDistanceCullVisList;

		//if m_bDistanceCullVisList is true, this value holds the distance that the
		//portals should be culled at. If it is false, the data is undefined
		PReal	m_CullDistanceVisList;

		//determines if the supersampling algorithm should be used
		bool	m_bLMSuperSample;

		//determine if the lightmaps should be expanded by a pixel on the right and 
		//bottom sides in order to have a little bit higher resolution (this
		//tends to fix continuity issues across corners)
		bool	m_bLMOverSample;

		//if the super sampling algorithm is used, this specifies the number
		//of samples to be taken along a side
		uint32	m_nLMNumSamplesOnSide;

		//center world around origin?
		bool	m_bCenterWorldAroundOrigin;

		// Use portals?
		bool	m_bPortals; 
		bool	m_bObjectsOnly;
		
		bool	m_bIgnoreHidden;
		bool	m_bIgnoreFrozen;

		PReal	m_SplitWeight, m_BalanceWeight;
		
		bool	m_bLight, m_bShadows, m_bLightAnimations;

		// use Lambert Lighting Model?
		bool	m_bLambertLight;

		//only light vertices (no lightmaps)
		bool	m_bVerticesOnly;

		//apply lambertian lighting model to lightmap samples?
		bool	m_bLambertianLightmaps;

		// Enable shadow meshing
		bool	m_bShadowMesh;

		// Volumetric ambient grid calculation
		bool	m_bVolumetricAmbient;

		//determine the maximum distance from where the light hits a blocking surface
		//from where the surface to the light hits a blocking surface that can be
		//still passed through
		PReal	m_fLMMaxPenThickness;

		//when taking lighting samples, this determines how much to extrude the points
		//along the polygon normal to prevent self intersection with its base polygon
		PReal	m_fLMSampleExtrude;

		//the global scale that all maximum light penetration values are scaled by
		PReal	m_fLMGlobalPenScale;

		//determine if we want polies split for the vis BSP. Much less overdraw,
		//but increases number of polies greatly
		bool	m_bSplitPolyForVisBSP;	

		//max detail level that can be used
		uint32	m_MaxDetail;

		//see if we are logging to a file
		bool	m_bLogFile;

		//see if we are logging to an error file
		bool	m_bErrorLog;

		//number of threads to use
		uint32	m_nThreads;

		//see if we want to skip the starting dialog
		bool	m_bSkipDialog;

		//see if we want to fix T junctions
		bool	m_bFixTJunc;
		bool	m_bRemoveUnseen;
		bool	m_bBalanceBSPs; // Balance the BSP when processing

		// Validate the lightmap data after processing
		bool	m_bValidateLightmaps;


		// The processor will fill these in.
		uint32	m_nInputPolies, m_nInputVertices, m_nInputObjects, m_nUnseenPolies;
		uint32	m_nOutputPolies, m_nOutputVertices, m_TreeDepth, m_VisListMemUse, m_nLeaves;
		uint32	m_nVisibleLeaves, m_LMDataSize;
	};

	extern CProcessorGlobs *g_pGlobs;


	// ------------------------------------------------------ //
	// Functions the Processing module implements.
	// ------------------------------------------------------ //

	//different types of messages that can be displayed
	enum EStatusTextType
	{
		eST_Error,		//used to signal errors that would indicate bugs in the level
		eST_Warning,	//used to indicate items that should be brought to the users attention but does not invalidate level
		eST_Normal		//used for displaying what the processor is doing
	};

	// Process argc and argv and setup the options.
	void ProcessOptions(CProcessorGlobs *pGlobs);
	
	// Do all the processing.
	void DoProcessing(CProcessorGlobs *pGlobs);

	void DrawStatusText(EStatusTextType eType, const char *pMsg, ...);
	// Draws text if not working on a world model.
	void DrawStatusTextIfMainWorld(EStatusTextType eType, const char *pMsg, ...); 

	// Makes a surface in pWorld for each brush poly.
	void GenerateSurfaces(GenList<CEditBrush*> &brushes, CPreWorld *pWorld);

	//given a node, it will look at the processor settings and determine if it should be ignored
	bool ShouldIgnoreNode(const CWorldNode* pNode);


	// ------------------------------------------------------ //
	// Functions the system-dependent stuff implements.
	// ------------------------------------------------------ //

	// Progress bar IDs.
	#define PROGRESS_BSP	0
	#define PROGRESS_LIGHT	1
	#define PROGRESS_HULL	2
	#define PROGRESS_VIS	3
	#define PROGRESS_SETUP	4

	// Question types and return values.
	#define QUES_YES		1
	#define QUES_NO			2
	#define QUES_CANCEL		4

	// Type is a combination of the above flags.
	// Returns one of the above flags.
	uint32 AskQuestion(const char *pQuestion, uint32 type);

	uint32 GetMSTime();

	void StartProgressBar(uint32 progressBar);
	void EndProgressBar(uint32 progressBar, bool bHide);
	void SetCurrentProgressBar(float percent);

	void SysDrawStatusText(const char *pMsg);
	void SetInFullVis();

	// Does a union on all brush geometry and converts to world geometry.
	bool MergeBrushes(
		GenList<CEditBrush*> &brushes,
		CPreWorld *pWorld);

	// Make a BSP.
	bool MakeBsp(CPreWorld *pWorld, 
		bool bMinimizePolySize, bool bAddHullMakers, bool bUseOriginals);

	// Make hulls in a BspHullMaker.
	bool MakeHulls(CPreWorld *pMakeOn, CBspHullMaker *pMaker);

	// Save the LTA file back out.
	bool SaveInputRegion();

	// Make hulls and remove parts of the world that are outside.
	bool RemoveUnseenPortions(CPreWorld *pWorld);

#endif  // __PROCESSING_H__
