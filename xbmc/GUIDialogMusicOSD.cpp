#include "stdafx.h"
#include "GUIDialogMusicOSD.h"
#include "GUIWindowSettingsCategory.h"


#define CONTROL_VIS_BUTTON       500
#define CONTROL_LOCK_BUTTON      501
#define CONTROL_VIS_CHOOSER      503

CGUIDialogMusicOSD::CGUIDialogMusicOSD(void)
    : CGUIDialog(WINDOW_DIALOG_MUSIC_OSD, "MusicOSD.xml")
{
  m_pVisualisation = NULL;
  LoadOnDemand(false);    // we are loaded by the vis window.
}

CGUIDialogMusicOSD::~CGUIDialogMusicOSD(void)
{
}

bool CGUIDialogMusicOSD::OnMessage(CGUIMessage &message)
{
  switch (message.GetMessage())
  {
  case GUI_MSG_CLICKED:
    {
      unsigned int iControl = message.GetSenderId();
      if (iControl == CONTROL_VIS_CHOOSER)
      {
        CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), iControl);
        OnMessage(msg);
        CStdString strLabel = msg.GetLabel();
        if (msg.GetParam1() == 0)
          g_guiSettings.SetString("mymusic.visualisation", "None");
        else
          g_guiSettings.SetString("mymusic.visualisation", strLabel + ".vis");
        // hide the control and reset focus
        SET_CONTROL_HIDDEN(CONTROL_VIS_CHOOSER);
        SET_CONTROL_FOCUS(CONTROL_VIS_BUTTON, 0);
        return true;
      }
      else if (iControl == CONTROL_VIS_BUTTON)
      {
        SET_CONTROL_VISIBLE(CONTROL_VIS_CHOOSER);
        SET_CONTROL_FOCUS(CONTROL_VIS_CHOOSER, 0);
        // fire off an event that we've pressed this button...
        CAction action;
        action.wID = ACTION_SELECT_ITEM;
        OnAction(action);
      }
      else if (iControl == CONTROL_LOCK_BUTTON)
      {
        CGUIMessage msg(GUI_MSG_VISUALISATION_ACTION, 0, 0, ACTION_VIS_PRESET_LOCK);
        g_graphicsContext.SendMessage(msg);
      }
      return true;
    }
    break;
  case GUI_MSG_WINDOW_DEINIT:
  case GUI_MSG_VISUALISATION_UNLOADING:
    {
      m_pVisualisation = NULL;
    }
    break;
  case GUI_MSG_VISUALISATION_LOADED:
    {
      if (message.GetLPVOID())
        m_pVisualisation = (CVisualisation *)message.GetLPVOID();
    }
  }
  return CGUIDialog::OnMessage(message);
}

void CGUIDialogMusicOSD::Render()
{
  CGUIDialog::Render();
}

void CGUIDialogMusicOSD::OnInitWindow()
{
  CSetting *pSetting = g_guiSettings.GetSetting("mymusic.visualisation");
  CGUIWindowSettingsCategory::FillInVisualisations(pSetting, CONTROL_VIS_CHOOSER);

  ResetControlStates();
  CGUIDialog::OnInitWindow();
}