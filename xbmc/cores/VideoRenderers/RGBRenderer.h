#ifndef RGB_RENDERER
#define RGB_RENDERER

#include "XBoxRenderer.h"

class CRGBRenderer : public CXBoxRenderer
{
public:
  CRGBRenderer(LPDIRECT3DDEVICE8 pDevice);
  //~CRGBRenderer();

  // Functions called from mplayer
  // virtual void     WaitForFlip();
  virtual unsigned int Configure(unsigned int width, unsigned int height, unsigned int d_width, unsigned int d_height, float fps);
  virtual unsigned int PreInit();
  virtual void UnInit();

protected:
  virtual void Render(DWORD flags);
  virtual void ManageTextures();
  bool Create444PTexture();
  void Delete444PTexture();
  void Clear444PTexture();
  bool CreateLookupTextures();
  void DeleteLookupTextures();

  void InterleaveYUVto444P(
      YUVPLANES pSources,
      LPDIRECT3DTEXTURE8 pTarget,
      unsigned width,     unsigned height,
      unsigned cshift_x,  unsigned cshift_y,
      float    coffset_x, float    coffset_y);

  // YUV interleaved texture
  LPDIRECT3DTEXTURE8 m_444PTexture[MAX_FIELDS];

  // textures for YUV->RGB lookup
  LPDIRECT3DTEXTURE8 m_UVLookup;
  LPDIRECT3DTEXTURE8 m_UVErrorLookup;

  // Pixel shaders
  DWORD m_hInterleavingShader;
  DWORD m_hYUVtoRGBLookup;


  // Vertex types
  static const DWORD FVF_YUVRGBVERTEX = D3DFVF_XYZRHW | D3DFVF_TEX4;
  static const DWORD FVF_RGBVERTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;
};

#endif
