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

#include "GUIDialog.h"
#include "SmartPlaylist.h"

class CGUIDialogSmartPlaylistRule :
      public CGUIDialog
{
public:
  CGUIDialogSmartPlaylistRule(void);
  virtual ~CGUIDialogSmartPlaylistRule(void);
  virtual bool OnMessage(CGUIMessage& message);
  virtual bool OnAction(const CAction &action);
  virtual void OnInitWindow();

  static bool EditRule(CSmartPlaylistRule &rule, const CStdString& type="music");

protected:
  void OnValue();
  void OnField();
  void OnOperator();
  void OnOK();
  void OnCancel();
  void UpdateButtons();
  void AddOperatorLabel(CSmartPlaylistRule::SEARCH_OPERATOR op);
  void OnBrowse();

  enum FIELD { TEXT_FIELD, NUMERIC_FIELD, DATE_FIELD, PLAYLIST_FIELD, SECONDS_FIELD };
  FIELD GetFieldType(CSmartPlaylistRule::DATABASE_FIELD field);

  CSmartPlaylistRule m_rule;
  bool m_cancelled;
  CStdString m_type;
};
