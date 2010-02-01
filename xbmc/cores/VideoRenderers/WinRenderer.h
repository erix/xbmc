#pragma once

/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#if !defined(_LINUX) && !defined(HAS_GL)




#include "WinBaseRenderer.h"
//#define MP_DIRECTRENDERING

class CBaseTexture;


class CWinRenderer : public CWinBaseRenderer
{
public:
  CWinRenderer();
  ~CWinRenderer();

  virtual void Update(bool bPauseDrawing);
  virtual void SetupScreenshot() {};
  void CreateThumbnail(CBaseTexture *texture, unsigned int width, unsigned int height);

  // Player functions
  virtual bool Configure(unsigned int width, unsigned int height, unsigned int d_width, unsigned int d_height, float fps, unsigned flags);
  virtual int          GetImage(YV12Image *image, int source = AUTOSOURCE, bool readonly = false);
  virtual void         ReleaseImage(int source, bool preserve = false);
  virtual unsigned int DrawSlice(unsigned char *src[], int stride[], int w, int h, int x, int y);
  virtual void         FlipPage(int source);
  virtual unsigned int PreInit();
  virtual void         UnInit();
  virtual void         Reset(); /* resets renderer after seek for example */
  virtual bool         IsConfigured() { return m_bConfigured; }

  // TODO:DIRECTX - implement these
  virtual bool         SupportsBrightness() { return true; }
  virtual bool         SupportsContrast() { return true; }
  virtual bool         SupportsGamma() { return false; }
  virtual bool         Supports(EINTERLACEMETHOD method);
  virtual bool         Supports(ESCALINGMETHOD method);

  void RenderUpdate(bool clear, DWORD flags = 0, DWORD alpha = 255);

protected:
  virtual void Render(DWORD flags);
  void CopyAlpha(int w, int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dst, unsigned char* dsta, int dststride);
  virtual void ManageTextures();
  void DeleteYV12Texture(int index);
  void ClearYV12Texture(int index);
  bool CreateYV12Texture(int index);
  void CopyYV12Texture(int dest);
  int  NextYV12Texture();

  void UpdateVideoFilter();

  bool LoadEffect(CD3DEffect &effect, CStdString filename);

  // low memory renderer (default PixelShaderRenderer)
  void RenderLowMem(CD3DEffect &effect, DWORD flags);
  int m_iYV12RenderBuffer;
  int m_NumYV12Buffers;

  bool m_bConfigured;

  typedef CD3DTexture             YUVVIDEOPLANES[MAX_PLANES];
  typedef BYTE*                   YUVMEMORYPLANES[MAX_PLANES];
  typedef YUVVIDEOPLANES          YUVVIDEOBUFFERS[NUM_BUFFERS];
  typedef YUVMEMORYPLANES         YUVMEMORYBUFFERS[NUM_BUFFERS];

  #define PLANE_Y 0
  #define PLANE_U 1
  #define PLANE_V 2

  #define FIELD_FULL 0
  #define FIELD_ODD 1
  #define FIELD_EVEN 2

  // YV12 decoder textures
  // field index 0 is full image, 1 is odd scanlines, 2 is even scanlines
  // Since DX is single threaded, we will render all video into system memory
  // We will them copy in into the device when rendering from main thread
  YUVVIDEOBUFFERS m_YUVVideoTexture;
  YUVMEMORYBUFFERS m_YUVMemoryTexture;

  CD3DTexture m_HQKernelTexture;
  CD3DEffect  m_YUV2RGBEffect;
  CD3DEffect  m_YUV2RGBHQScalerEffect;

  ESCALINGMETHOD m_scalingMethod;
  ESCALINGMETHOD m_scalingMethodGui;

  D3DCAPS9 m_deviceCaps;

  bool m_bUseHQScaler;
  bool m_bFilterInitialized;

  // clear colour for "black" bars
  DWORD m_clearColour;
  unsigned int m_flags;
};


class CPixelShaderRenderer : public CWinRenderer
{
public:
  CPixelShaderRenderer();
  virtual bool Configure(unsigned int width, unsigned int height, unsigned int d_width, unsigned int d_height, float fps, unsigned flags);

protected:
  virtual void Render(DWORD flags);
};

#else
#include "LinuxRenderer.h"
#endif


