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

#include "include.h"
#include "GUIRSSControl.h"
#include "GUIWindowManager.h"
#include "GUISettings.h"
#include "utils/CriticalSection.h"
#include "utils/SingleLock.h"
#include "utils/RssReader.h"
#include "StringUtils.h"

using namespace std;

CGUIRSSControl::CGUIRSSControl(int parentID, int controlID, float posX, float posY, float width, float height, const CLabelInfo& labelInfo, const CGUIInfoColor &channelColor, const CGUIInfoColor &headlineColor, CStdString& strRSSTags, int scrollSpeed)
: CGUIControl(parentID, controlID, posX, posY, width, height),
  m_scrollInfo(0,0,scrollSpeed,"")
{
  m_label = labelInfo;
  m_headlineColor = headlineColor;
  m_channelColor = channelColor;

  m_strRSSTags = strRSSTags;

  m_pReader = NULL;
  ControlType = GUICONTROL_RSS;

  if (g_guiSettings.GetBool("lookandfeel.rssfeedsrtl"))
  {
    m_scrollInfo.SetSpeed(-60);
  }
}

CGUIRSSControl::CGUIRSSControl(const CGUIRSSControl &from)
: CGUIControl(from),m_scrollInfo(from.m_scrollInfo)
{
  m_label = from.m_label;
  m_headlineColor = from.m_headlineColor;
  m_channelColor = from.m_channelColor;
  m_strRSSTags = from.m_strRSSTags;
  m_pReader = NULL;
  ControlType = GUICONTROL_RSS;
}

CGUIRSSControl::~CGUIRSSControl(void)
{
  CSingleLock lock(m_criticalSection);
  if (m_pReader)
    m_pReader->SetObserver(NULL);
  m_pReader = NULL;
}

void CGUIRSSControl::SetUrls(const vector<string> &vecUrl)
{
  m_vecUrls = vecUrl;
}

void CGUIRSSControl::SetIntervals(const vector<int>& vecIntervals)
{
  m_vecIntervals = vecIntervals;
}

void CGUIRSSControl::UpdateColors()
{
  m_label.UpdateColors();
  m_headlineColor.Update();
  m_channelColor.Update();
  CGUIControl::UpdateColors();
}

void CGUIRSSControl::Render()
{
  // only render the control if they are enabled
  if (g_guiSettings.GetBool("lookandfeel.enablerssfeeds") && g_rssManager.IsActive())
  {
    CSingleLock lock(m_criticalSection);
    // Create RSS background/worker thread if needed
    if (m_pReader == NULL)
    {
      if (g_rssManager.GetReader(GetID(), GetParentID(), this, m_pReader))
        m_scrollInfo.characterPos = m_pReader->m_SavedScrollPos; 
      else 
      {
        if (m_strRSSTags != "")
        {
          CStdStringArray vecSplitTags;

          StringUtils::SplitString(m_strRSSTags, ",", vecSplitTags);

          for (unsigned int i = 0;i < vecSplitTags.size();i++)
            m_pReader->AddTag(vecSplitTags[i]);
        }
        // use half the width of the control as spacing between feeds, and double this between feed sets
        float spaceWidth = (m_label.font) ? m_label.font->GetCharWidth(L' ') : 15;
        m_pReader->Create(this, m_vecUrls, m_vecIntervals, (int)(0.5f*GetWidth() / spaceWidth) + 1);
      }
    }

    if (m_label.font)
    {
      vecColors colors;
      colors.push_back(m_label.textColor);
      colors.push_back(m_headlineColor);
      colors.push_back(m_channelColor);
      m_label.font->DrawScrollingText(m_posX, m_posY, colors, m_label.shadowColor, m_feed, 0, m_width, m_scrollInfo);
    }

    if (m_pReader)
    {
      m_pReader->CheckForUpdates();
      m_pReader->m_SavedScrollPos = m_scrollInfo.characterPos;
    }
  }
  CGUIControl::Render();
}

void CGUIRSSControl::OnFeedUpdate(const vecText &feed)
{
  CSingleLock lock(m_criticalSection);
  m_feed = feed;
}

void CGUIRSSControl::OnFeedRelease()
{
  m_pReader = NULL;
}


