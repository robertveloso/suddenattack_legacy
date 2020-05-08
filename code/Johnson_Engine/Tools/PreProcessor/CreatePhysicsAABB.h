#ifndef CREATEPHYSICSAABB_H
#define CREATEPHYSICSAABB_H

#include "collision_data.h"
#include "findworldmodel.h"


// default physics material info
const int32 PHYS_DEFAULT_TYPE	= 0;		// default physics material id
const float PHYS_DEFAULT_UK		= 1.0f;		// default coefficient of kinetic friction
const float PHYS_DEFAULT_US		= 1.0f;		// default coefficient of static friction
const float PHYS_DEFAULT_COR	= 1.0f;		// default coefficient of restitution


struct PrePhysicsMaterial;
struct PrePhysicsTri;
class  CPreMainWorld;
class  CEditRegion;


// the physics data for one world model (or terrain/brushes combined into one)
class CPhysicsWorldModel
{
public:
	// returns the name of the world model associated with this, or nullstring for terrain/world
	const char* GetWorldName() const		{ return m_WorldName; }

	//sets the name of the world that is associated with it
	void SetWorldName( const char* pszName ) 	{ strcpy( m_WorldName, pszName ); }

	// returns the number of different collision data elements there are
	uint32 GetNumSections() const { return m_nNumSections; }

	// returns a specific collision data element
	LTCollisionData* GetSection( uint32 nSectionNum );

	// returns the size of a specific collision data element
	uint32 GetSectionSize( uint32 nSectionNum ) const;

	// initialize the model with a triangle soup
	bool Initialize( const char* pName, uint32 nNumMats, PrePhysicsMaterial* pMats,
					 uint32 nNumTris, PrePhysicsTri* pTris, 
					 uint32 nNumVerts, LTVector3f* pVerts );

	//sets the number of sections that are attached to this physics info structure.
	//this will clear out all existing sections
	bool SetNumSections(uint32 nNumSections);

	//sets the data attached to the specified section.
	bool SetSectionData(uint32 nSection, LTCollisionData* pData, uint32 nDataSize);

	// constructor and destructor
	CPhysicsWorldModel( const char* pszName, LTCollisionData* pData, uint32 nDataSize );
	CPhysicsWorldModel();
	~CPhysicsWorldModel();

private:

	//deallocates all memory associated with the sections
	void	FreeSections();

	//deallocates all memory associated with the object
	void	Free();

	char m_WorldName[MAX_WORLDNAME_LEN];
	uint32 m_nNumSections;
	LTCollisionData** m_ppSection;
	uint32* m_pDataSize;
};


// a triangle with physics information
struct PrePhysicsTri
{
	uint32 material;		// material index
	uint32 a, b, c;			// vertex indices
//	LTVector3f max, min;	// bounding box
};


// physics information for the triangle soup
struct PrePhysicsMaterial
{
	int32 m_Sort;	// used for sorting tris by material
	int32 m_Type;	// type of this collision
	float m_Us;		// static friction
	float m_Uk;		// kinetic friction
	float m_COR;	// restitution
};


bool CreateAABBPhysicsData( CEditRegion* pRegion, CPreMainWorld* pMainWorld );
bool SaveAABBPhysicsData( CAbstractIO& outFile, CMoArray<CPhysicsWorldModel*>& physicsModels );
bool LoadAABBPhysicsData( CAbstractIO& inFile, CMoArray<CPhysicsWorldModel*>& physicsModels );


#endif // CREATEPHYSICSAABB_H
