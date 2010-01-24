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

#include "system.h"
#include "GUIWindowSystemInfo.h"
#include "utils/GUIInfoManager.h"
#include "GUIWindowManager.h"
#include "Key.h"
#include "LocalizeStrings.h"
#ifdef HAS_SYSINFO
#include "SystemInfo.h"
#endif
#include "MediaManager.h"

#define CONTROL_BT_STORAGE  94
#define CONTROL_BT_DEFAULT  95
#define CONTROL_BT_NETWORK  96
#define CONTROL_BT_VIDEO    97
#define CONTROL_BT_HARDWARE 98
#define CONTROL_BT_PVR      99

#define CONTROL_START       CONTROL_BT_STORAGE
#define CONTROL_END         CONTROL_BT_PVR

CGUIWindowSystemInfo::CGUIWindowSystemInfo(void)
:CGUIWindow(WINDOW_SYSTEM_INFORMATION, "SettingsSystemInfo.xml")
{
  m_section = CONTROL_BT_DEFAULT;
}
CGUIWindowSystemInfo::~CGUIWindowSystemInfo(void)
{
}
bool CGUIWindowSystemInfo::OnAction(const CAction &action)
{
  if (action.actionId == ACTION_PREVIOUS_MENU)
  {
    g_windowManager.PreviousWindow();
    return true;
  }
  return CGUIWindow::OnAction(action);
}
bool CGUIWindowSystemInfo::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_WINDOW_INIT:
    {
      CGUIWindow::OnMessage(message);
      ResetLabels();
      SET_CONTROL_LABEL(52, "XBMC " + g_infoManager.GetLabel(SYSTEM_BUILD_VERSION) +
                            " (Compiled : " + g_infoManager.GetLabel(SYSTEM_BUILD_DATE)+")");
      return true;
    }
    break;
  case GUI_MSG_WINDOW_DEINIT:
    {
      CGUIWindow::OnMessage(message);
      m_diskUsage.clear();
      return true;
    }
    break;
  case GUI_MSG_FOCUSED:
    {
      CGUIWindow::OnMessage(message);
      int focusedControl = GetFocusedControlID();
      if (focusedControl >= CONTROL_START && focusedControl <= CONTROL_END)
        m_section = focusedControl;
      return true;
    }
    break;
  }
  return CGUIWindow::OnMessage(message);
}

void CGUIWindowSystemInfo::Render()
{
  ResetLabels();
  int i = 2;
  if (m_section == CONTROL_BT_DEFAULT)
  {
    SET_CONTROL_LABEL(40,g_localizeStrings.Get(20154));
    SetControlLabel(i++, "%s: %s", 158, SYSTEM_FREE_MEMORY);
    SetControlLabel(i++, "%s: %s", 150, NETWORK_IP_ADDRESS);
    SetControlLabel(i++, "%s %s", 13287, SYSTEM_SCREEN_RESOLUTION);
#ifdef HAS_SYSINFO
    SetControlLabel(i++, "%s %s", 13283, SYSTEM_KERNEL_VERSION);
#endif
    SetControlLabel(i++, "%s: %s", 12390, SYSTEM_UPTIME);
    SetControlLabel(i++, "%s: %s", 12394, SYSTEM_TOTALUPTIME);
  }
  else if (m_section == CONTROL_BT_STORAGE)
  {
    SET_CONTROL_LABEL(40,g_localizeStrings.Get(20155));
    if (m_diskUsage.size() == 0)
      m_diskUsage = g_mediaManager.GetDiskUsage();

    for (size_t d = 0; d < m_diskUsage.size(); d++)
    {
      SET_CONTROL_LABEL(i++, m_diskUsage[d]);
    }
  }
  else if (m_section == CONTROL_BT_NETWORK)
  {
    SET_CONTROL_LABEL(40,g_localizeStrings.Get(20158));
#ifdef HAS_SYSINFO
    SET_CONTROL_LABEL(i++, g_infoManager.GetLabel(NETWORK_LINK_STATE));
    SetControlLabel(i++, "%s: %s", 149, NETWORK_MAC_ADDRESS);
#endif
    SetControlLabel(i++, "%s: %s", 150, NETWORK_IP_ADDRESS);
    SetControlLabel(i++, "%s: %s", 13159, NETWORK_SUBNET_ADDRESS);
    SetControlLabel(i++, "%s: %s", 13160, NETWORK_GATEWAY_ADDRESS);
    SetControlLabel(i++, "%s: %s", 13161, NETWORK_DNS1_ADDRESS);
    SetControlLabel(i++, "%s: %s", 20307, NETWORK_DNS2_ADDRESS);
    SetControlLabel(i++, "%s %s", 13295, SYSTEM_INTERNET_STATE);
  }
  else if (m_section == CONTROL_BT_VIDEO)
  {
    SET_CONTROL_LABEL(40,g_localizeStrings.Get(20159));
#ifdef HAS_SYSINFO
    SET_CONTROL_LABEL(i++,g_infoManager.GetLabel(SYSTEM_VIDEO_ENCODER_INFO));
    SetControlLabel(i++, "%s %s", 13287, SYSTEM_SCREEN_RESOLUTION);
#endif
#ifndef HAS_DX
    SetControlLabel(i++, "%s %s", 22007, SYSTEM_RENDER_VENDOR);
    SetControlLabel(i++, "%s %s", 22009, SYSTEM_RENDER_VERSION);
#else
    SetControlLabel(i++, "%s %s", 22023, SYSTEM_RENDER_VENDOR);
    SetControlLabel(i++, "%s %s", 22024, SYSTEM_RENDER_VERSION);
#endif
    SetControlLabel(i++, "%s %s", 22010, SYSTEM_GPU_TEMPERATURE);
  }
  else if (m_section == CONTROL_BT_HARDWARE)
  {
    SET_CONTROL_LABEL(40,g_localizeStrings.Get(20160));
#ifdef HAS_SYSINFO
    SET_CONTROL_LABEL(i++, g_sysinfo.GetXBVerInfo());
    SetControlLabel(i++, "%s %s", 22011, SYSTEM_CPU_TEMPERATURE);
    SetControlLabel(i++, "%s %s", 13284, SYSTEM_CPUFREQUENCY);
#endif
    SetControlLabel(i++, "%s %s", 13271, SYSTEM_CPU_USAGE);
    i++; // empty line
    SetControlLabel(i++, "%s: %s", 22012, SYSTEM_TOTAL_MEMORY);
    SetControlLabel(i++, "%s: %s", 158, SYSTEM_FREE_MEMORY);
  }
  else if(m_section == CONTROL_BT_PVR)
  {
    SET_CONTROL_LABEL(40,g_localizeStrings.Get(18030));
    int i = 2;

    SetControlLabel(i++, "%s %s", 18208, PVR_BACKEND_NUMBER);
    i++; // empty line
    SetControlLabel(i++, "%s %s", 18200, PVR_BACKEND_NAME);
    SetControlLabel(i++, "%s %s", 18201, PVR_BACKEND_VERSION);
    SetControlLabel(i++, "%s %s", 18202, PVR_BACKEND_HOST);
    SetControlLabel(i++, "%s %s", 18203, PVR_BACKEND_DISKSPACE);
    SetControlLabel(i++, "%s %s", 18204, PVR_BACKEND_CHANNELS);
    SetControlLabel(i++, "%s %s", 18205, PVR_BACKEND_RECORDINGS);
    SetControlLabel(i++, "%s %s", 18206, PVR_BACKEND_TIMERS);
  }
  SET_CONTROL_LABEL(52, "XBMC "+g_infoManager.GetLabel(SYSTEM_BUILD_VERSION)+" (Compiled : "+g_infoManager.GetLabel(SYSTEM_BUILD_DATE)+")");

  CGUIWindow::Render();
}

void CGUIWindowSystemInfo::ResetLabels()
{
  for (int i = 2; i < 12; i++)
  {
    SET_CONTROL_LABEL(i,"");
  }
}

void CGUIWindowSystemInfo::SetControlLabel(int id, const char *format, int label, int info)
{
  CStdString tmpStr;
  tmpStr.Format(format, g_localizeStrings.Get(label).c_str(), g_infoManager.GetLabel(info).c_str());
  SET_CONTROL_LABEL(id, tmpStr);
}
