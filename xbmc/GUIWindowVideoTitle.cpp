// Todo:
//  - directory history
//  - if movie does not exists when play movie is called then show dialog asking to insert the correct CD
//  - show if movie has subs

#include "stdafx.h"
#include "GUIWindowVideoTitle.h"
#include "Util.h"
#include "Utils/imdb.h"
#include "GUIWindowVideoInfo.h"
#include "NFOFile.h"
#include "PlayListPlayer.h"
#include "GUIThumbnailPanel.h"
#include "GUIPassword.h"

#define CONTROL_PLAY_DVD          6
#define CONTROL_STACK             7
#define CONTROL_IMDB              9
#define CONTROL_BTNSHOWMODE       10
#define LABEL_TITLE              100

/*  REMOVED: There was a method here for sort by DVD label, but as this
    doesn't seem to be used anywhere (at least it is not given a value
    anywhere) I've removed it. (JM, 3 Dec 2005) */

//****************************************************************************************************************************
CGUIWindowVideoTitle::CGUIWindowVideoTitle()
: CGUIWindowVideoBase(WINDOW_VIDEO_TITLE, "MyVideoTitle.xml")
{
  m_vecItems.m_strPath = "";
}

//****************************************************************************************************************************
CGUIWindowVideoTitle::~CGUIWindowVideoTitle()
{
}

//****************************************************************************************************************************
bool CGUIWindowVideoTitle::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_WINDOW_INIT:
    {
    }
    break;
  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      if (iControl == CONTROL_BTNSHOWMODE)
	    {
        g_stSettings.m_iMyVideoWatchMode++;
		    if (g_stSettings.m_iMyVideoWatchMode > VIDEO_SHOW_WATCHED)
          g_stSettings.m_iMyVideoWatchMode = VIDEO_SHOW_ALL;
        g_settings.Save();
		    Update(m_vecItems.m_strPath);
        return true;
      }
      else
        return CGUIWindowVideoBase::OnMessage(message);
    }
  }
  return CGUIWindowVideoBase::OnMessage(message);
}

bool CGUIWindowVideoTitle::GetDirectory(const CStdString &strDirectory, CFileItemList &items)
{
  items.m_strPath = strDirectory;
  VECMOVIES movies;
  m_database.GetMovies(movies);
  // Display an error message if the database doesn't contain any movies
  DisplayEmptyDatabaseMessage(movies.empty());
  SetDatabaseDirectory(movies, items);
  return true;
}

void CGUIWindowVideoTitle::UpdateButtons()
{
  CGUIWindowVideoBase::UpdateButtons();
  SET_CONTROL_LABEL(LABEL_TITLE, m_vecItems.m_strPath);
}

void CGUIWindowVideoTitle::OnDeleteItem(int iItem)
{
  if (iItem < 0 || iItem >= (int)m_vecItems.Size()) return;

  CFileItem* pItem = m_vecItems[iItem];
  if (pItem->m_bIsFolder) return;

  CGUIDialogYesNo* pDialog = (CGUIDialogYesNo*)m_gWindowManager.GetWindow(WINDOW_DIALOG_YES_NO);
  if (!pDialog) return;
  pDialog->SetHeading(432);
  pDialog->SetLine(0, 433);
  pDialog->SetLine(1, 434);
  pDialog->SetLine(2, "");;
  pDialog->DoModal();
  if (!pDialog->IsConfirmed()) return;

  CStdString path;
  //m_database.GetFilePath(atol(pItem->m_strPath), path);
  m_database.GetFilePath(atol(pItem->m_musicInfoTag.GetURL()), path);
  if (path.IsEmpty()) return;
  m_database.DeleteMovie(path);

  // delete the cached thumb for this item (it will regenerate if it is a user thumb)
  CStdString thumb(pItem->GetCachedVideoThumb());
  CFile::Delete(thumb);

  Update( m_vecItems.m_strPath );
  m_viewControl.SetSelectedItem(iItem);
  return;
}

void CGUIWindowVideoTitle::OnQueueItem(int iItem)
{
  CGUIWindowVideoBase::OnQueueItem(iItem);
}