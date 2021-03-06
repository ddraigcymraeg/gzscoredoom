
#ifndef __GL_TEXTURE_H
#define __GL_TEXTURE_H

#include "gl/gltexture.h"
#include "r_data.h"
#include "i_system.h"

EXTERN_CVAR(Bool, gl_precache)
EXTERN_CVAR(Bool, gl_brightmap_shader)

struct GL_RECT;

void ModifyPalette(PalEntry * pout, PalEntry * pin, int cm, int count);
void CopyColorsRGBA(unsigned char * pout, const unsigned char * pin, int cm, int count, int step);

// Two intermediate classes which wrap the low level textures.
// These ones are returned by the Bind* functions to ensure
// that the coordinate functions aren't used without the texture
// being initialized.
// Unfortunately it is necessary to maintain 2 of these.
// On older graphics cards which don't support non-power of 2 textures
// these are not interchangable so if a texture happens to be used
// both as sprite and texture there need to be different versions.

class WorldTextureInfo
{
protected:
	GLTexture * gltexture;
	float scalex;
	float scaley;

	void Clean(bool all)
	{
		if (gltexture) 
		{
			if (!all) gltexture->Clean(false);
			else
			{
				delete gltexture;
				gltexture=NULL;
			}
		}
	}

public:

	float GetU(float upix) const { return gltexture->GetU(upix*scalex); }
	float GetV(float vpix) const { return gltexture->GetV(vpix*scaley); }
		  
	float FloatToTexU(float v) const { return gltexture->FloatToTexU(v*scalex); }
	float FixToTexU(int v) const { return gltexture->FixToTexU(v)*scalex; }
	float FixToTexV(int v) const { return gltexture->FixToTexV(v)*scaley; }
};

class PatchTextureInfo
{
protected:
	GLTexture * glpatch;

	void Clean(bool all)
	{
		if (glpatch) 
		{
			if (!all) glpatch->Clean(false);
			else
			{
				delete glpatch;
				glpatch=NULL;
			}
		}
	}

public:
	float GetUR() const { return glpatch->GetUR(); }
	float GetVB() const { return glpatch->GetVB(); }
	float GetU(float upix) const { return glpatch->GetU(upix); }
	float GetV(float vpix) const { return glpatch->GetV(vpix); }
};


//===========================================================================
// 
// this is the texture maintenance class for OpenGL. 
//
//===========================================================================
class GLShader;

class FGLTexture : protected WorldTextureInfo, protected PatchTextureInfo
{
	friend void Debug_Patch();	// debug code should have full access

	static TArray<FGLTexture *> * gltextures;
public:
	FTexture * tex;
	FTexture * hirestexture;
	FTexture * brightmap;
	bool bSkybox;
	char bIsBrightmap;
	bool bBrightmapDisablesFullbright;
	char bIsTransparent;
	bool createWarped;
	int HiresLump;

private:
	int index;

	signed char areacount;
	bool bHasColorkey;		// only for hires
	GL_RECT * areas;
	GLShader * Shader;

	short LeftOffset;
	short TopOffset;
	short Width;
	short Height;
	short RenderWidth;
	short RenderHeight;
	float AlphaThreshold;

	bool FindHoles(const unsigned char * buffer, int w, int h);
	bool ProcessData(unsigned char * buffer, int w, int h, int cm, bool ispatch);
	void CheckTrans(unsigned char * buffer, int size, int trans);
	static bool SmoothEdges(unsigned char * buffer,int w, int h, bool clampsides);
	int CheckDDPK3();
	int CheckExternalFile(bool & hascolorkey);
	unsigned char * LoadHiresTexture(int *width, int *height,intptr_t cm);


	void SetSize(int w, int h)
	{
		Width=w;
		Height=h;
		scalex=(float)Width/RenderWidth;
		scaley=(float)Height/RenderHeight;
	}

	void CheckForAlpha(const unsigned char * buffer);

	const WorldTextureInfo * Bind(int texunit, int cm, int clamp, int translation);
	const PatchTextureInfo * BindPatch(int texunit, int cm, int translation, const BYTE * translationtable);

public:
	FGLTexture(FTexture * tx);
	~FGLTexture();

	unsigned char * CreateTexBuffer(int cm, int translation, const BYTE * translationtable, int & w, int & h, bool allowhires=true);
	const WorldTextureInfo * Bind(int cm, int clamp=0, int translation = 0);
	const PatchTextureInfo * BindPatch(int cm, int translation=0, const BYTE * translationtable=NULL);

	const WorldTextureInfo * GetWorldTextureInfo();
	const PatchTextureInfo * GetPatchTextureInfo();

	void Clean(bool all);

	static void FlushAll();
	static void DestroyAll();
	static FGLTexture * ValidateTexture(FTexture * tex);
	static FGLTexture * ValidateTexture(int no, bool translate=true);

	// Patch drawing utilities

	void GetRect(GL_RECT * r) const;

	int TextureHeight() const { return RenderHeight; }
	int TextureWidth() const { return RenderWidth; }

	int GetAreaCount() const { return areacount; }
	GL_RECT * GetAreas() const { return areas; }

	fixed_t RowOffset(fixed_t rowoffset) const
	{
		if (scaley==1.f || tex->bWorldPanning) return rowoffset;
		else return quickertoint(rowoffset/scaley);
	}

	fixed_t TextureOffset(fixed_t textureoffset) const
	{
		if (scalex==1.f || tex->bWorldPanning) return textureoffset;
		else return quickertoint(textureoffset/scalex);
	}

	// Returns the size for which texture offset coordinates are used.
	fixed_t TextureAdjustWidth() const
	{
		if (tex->bWorldPanning) return RenderWidth;
		else return Width;
	}

	int GetWidth() const
	{
		return Width;
	}

	int GetHeight() const
	{
		return Height;
	}

	int GetLeftOffset() const
	{
		return LeftOffset;
	}

	int GetTopOffset() const
	{
		return TopOffset;
	}

	int GetScaledLeftOffset() const
	{
		return DivScale16(LeftOffset, tex->xScale);
	}

	int GetScaledTopOffset() const
	{
		return DivScale16(TopOffset, tex->yScale);
	}

	int GetIndex() const
	{
		return index;
	}

	bool GetTransparent()
	{
		if (bIsTransparent == -1) tex->PrecacheGL();
		return !!bIsTransparent;
	}

};


class FBrightmapTexture : public FTexture
{
	friend class FGLTexture;

public:
	FBrightmapTexture (FTexture *source);
	~FBrightmapTexture ();

	const BYTE *GetColumn (unsigned int column, const Span **spans_out);
	const BYTE *GetPixels ();
	void Unload ();

	int CopyTrueColorPixels(BYTE * buffer, int buf_width, int buf_height, int x, int y, intptr_t cm, int translation);
	bool UseBasePalette() { return false; }

protected:
	FTexture *SourcePic;
	//BYTE *Pixels;
	//Span **Spans;
};

void gl_EnableTexture(bool on);
void gl_GenerateGlobalBrightmapFromColormap();

#endif