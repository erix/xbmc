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

#include "PictureThumbLoader.h"
#include "Picture.h"
#include "URL.h"
#include "FileSystem/File.h"
#include "FileItem.h"
#include "VideoInfoTag.h"
#include "TextureManager.h"
#include "TextureCache.h"

using namespace XFILE;

CPictureThumbLoader::CPictureThumbLoader()
{
  m_regenerateThumbs = false;
}

CPictureThumbLoader::~CPictureThumbLoader()
{
  StopThread();
}

bool CPictureThumbLoader::LoadItem(CFileItem* pItem)
{
  pItem->SetCachedPictureThumb();
  if (pItem->m_bIsShareOrDrive) return true;

  if (pItem->HasThumbnail())
  {
    CStdString thumb(pItem->GetThumbnailImage());

    // look for remote thumbs
    if (!g_TextureManager.CanLoad(thumb))
    {
      thumb = CTextureCache::Get().CheckAndCacheImage(thumb);
      pItem->SetThumbnailImage(thumb);
      pItem->FillInDefaultIcon();
      return !thumb.IsEmpty();
    }
    else if (m_regenerateThumbs)
    {
      CTextureCache::Get().ClearCachedImage(thumb);
      pItem->SetThumbnailImage("");
    }
  }

  if ((pItem->IsPicture() && !pItem->IsZIP() && !pItem->IsRAR() && !pItem->IsCBZ() && !pItem->IsCBR() && !pItem->IsPlayList()) && !pItem->HasThumbnail())
  { // load the thumb from the image file
    CPicture::CreateThumbnail(pItem->m_strPath, pItem->GetCachedPictureThumb());
  }
  // refill in the thumb to get it to update
  pItem->SetCachedPictureThumb();
  pItem->FillInDefaultIcon();
  return true;
}

void CPictureThumbLoader::OnLoaderFinish()
{
  m_regenerateThumbs = false;
}
