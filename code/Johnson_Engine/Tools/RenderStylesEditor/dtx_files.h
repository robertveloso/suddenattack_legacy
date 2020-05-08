// This file contains all the dtx file loading code...

#ifndef __DTX_FILES_H__
#define __DTX_FILES_H__

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file
#endif

#include <d3d8.h>
#include <d3dx8.h>

// DEFINES...
#define LT_RESTYPE_DTX				0
#define CURRENT_DTX_VERSION			-5						// m_Version in the DTX header.
#define DTX_COMMANDSTRING_LEN		128

// Bits-per-pixel identifiers.
enum BPPIdent {
	BPP_8P=0,												// 8 bit palettized
	BPP_8,													// 8 bit RGB
	BPP_16,
	BPP_32,
	BPP_S3TC_DXT1,
	BPP_S3TC_DXT3,
	BPP_S3TC_DXT5,
	BPP_32P,												//! this was added for true color pallete support
	BPP_24,
	NUM_BIT_TYPES };

class DtxHeader
{

public:														// Accessors.
	inline uint32	GetTextureGroup()						{ return m_Extra[0]; }
	inline uint32	GetNumMipmaps()							{ return m_Extra[1]; }

	inline BPPIdent	GetBPPIdent()							{ return m_Extra[2] == 0 ? BPP_32 : (BPPIdent)m_Extra[2]; }
	inline void		SetBPPIdent(BPPIdent id)				{ m_Extra[2] = (uint8)id; }

	inline uint32	GetNonS3TCMipmapOffset()				{ return (uint32)m_Extra[3]; }
	inline void		SetNonS3TCMipmapOffset(uint32 offset)	{ m_Extra[3] = (uint8)offset; }

	inline uint32	GetUIMipmapOffset()						{ return m_Extra[4]; }
	inline float	GetUIMipmapScale()						{ return (float)(1 << GetUIMipmapOffset()); }
	inline void		SetUIMipmapOffset(uint32 val)			{ m_Extra[4] = (uint8)val; }

	inline uint32	GetTexturePriority()					{ return m_Extra[5]; }
	inline void		SetTexturePriority(uint32 val)			{ m_Extra[5] = (uint8)val; }

	// NOTE: it adds 1.0f so all the old values of 0.0f return 1.0f.
	// (the add of 1.0f should be totally transparent to everything though!)
	inline float	GetDetailTextureScale()					{ return *((float*)&m_Extra[6]) + 1.0f; }
	inline void		SetDetailTextureScale(float val)		{ *((float*)&m_Extra[6]) = (val - 1.0f); }

	inline int16	GetDetailTextureAngle()					{ return (int16)((m_Extra[10]) + (m_Extra[11] << 8)); }
	inline void		SetDetailTextureAngle(int16 angle)		{ m_Extra[10] = (uint8)(angle & 0xFF); m_Extra[11] = (uint8)(angle >> 8); }

public:
	uint32	m_ResType;
	int32	m_Version;		// CURRENT_DTX_VERSION
	uint16	m_BaseWidth, m_BaseHeight;
	uint16	m_nMipmaps;
	uint16	m_nSections;

	int32	m_IFlags;		// Combination of DTX_ flags.
	int32	m_UserFlags;	// Flags that go on surfaces.

	// Extra data.  Here's how it's layed out:
	// m_Extra[0] = Texture group.
	// m_Extra[1] = Number of mipmaps to use (there are always 4 in the file,
	//              but this says how many to use at runtime).
	// m_Extra[2] = BPPIdent telling what format the texture is in.
	// m_Extra[3] = Mipmap offset if the card doesn't support S3TC compression.
	// m_Extra[4] = Mipmap offset applied to texture coords (so a 512 could be
	//				treated like a 256 or 128 texture in the editor).
	// m_Extra[5] = Texture priority (default 0).
	// m_Extra[6-9] = Detail texture scale (float value).
	// m_Extra[10-11] = Detail texture angle (integer degrees)
	union {
		uint8	m_Extra[12];
		uint32	m_ExtraLong[3]; };

	char	m_CommandString[DTX_COMMANDSTRING_LEN];
};

// PROTOTYPES...
LPDIRECT3DTEXTURE8 LoadDTXFile(const char* szFilename);

#endif


