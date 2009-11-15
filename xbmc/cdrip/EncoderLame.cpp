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

#include "EncoderLame.h"
#include "Id3Tag.h"
#include "GUISettings.h"
#include "utils/log.h"

#ifdef _WIN32
extern "C" FILE *fopen_utf8(const char *_Filename, const char *_Mode);
#else
#define fopen_utf8 fopen
#endif

using namespace MUSIC_INFO;

CEncoderLame::CEncoderLame()
{
  memset(m_inPath, 0, 1024 + 1);
  memset(m_outPath, 0, 1024 + 1);
}

bool CEncoderLame::Init(const char* strFile, int iInChannels, int iInRate, int iInBits)
{
  // we only accept 2 / 44100 / 16 atm
  if (iInChannels != 2 || iInRate != 44100 || iInBits != 16) return false;

  // set input stream information and open the file
  if (!CEncoder::Init(strFile, iInChannels, iInRate, iInBits)) return false;

  // load the lame dll
  if (!m_dll.Load())
    return false;

  m_pGlobalFlags = m_dll.lame_init();
  if (!m_pGlobalFlags)
  {
    CLog::Log(LOGERROR, "Error: lame_init() failed");
    return false;
  }

  // setup parmaters, see lame.h for possibilities
  if (g_guiSettings.GetInt("audiocds.quality") == CDDARIP_QUALITY_CBR)
  {
    int bitrate = g_guiSettings.GetInt("audiocds.bitrate");
    // use cbr and specified bitrate from settings
    CLog::Log(LOGDEBUG, "Lame setting CBR bitrate %d", bitrate);
    m_dll.lame_set_brate(m_pGlobalFlags, bitrate);
  }
  else
  {
    // use presets (VBR)
    CStdString strSettings;
    int preset;
    switch (g_guiSettings.GetInt("audiocds.quality"))
    {
    case CDDARIP_QUALITY_MEDIUM:
      preset = MEDIUM;
      break;
    case CDDARIP_QUALITY_STANDARD:
      preset = STANDARD;
      break;
    case CDDARIP_QUALITY_EXTREME:
      preset = EXTREME;
      break;
    default:
      preset = STANDARD;
    }
    CLog::Log(LOGDEBUG, "Lame setting preset %d", preset);
    m_dll.lame_set_preset(m_pGlobalFlags, preset);
  }

  m_dll.lame_set_asm_optimizations(m_pGlobalFlags, MMX, 1);
  m_dll.lame_set_asm_optimizations(m_pGlobalFlags, SSE, 1);
  m_dll.lame_set_in_samplerate(m_pGlobalFlags, 44100);

  // Now that all the options are set, lame needs to analyze them and
  // set some more internal options and check for problems
  if (m_dll.lame_init_params(m_pGlobalFlags) < 0)
  {
    CLog::Log(LOGERROR, "Error: Cannot init Lame params");
    return false;
  }

  return true;
}

int CEncoderLame::Encode(int nNumBytesRead, BYTE* pbtStream)
{
  int iBytes = m_dll.lame_encode_buffer_interleaved(m_pGlobalFlags, (short*)pbtStream, nNumBytesRead / 4, m_buffer, sizeof(m_buffer));

  if (iBytes < 0)
  {
    CLog::Log(LOGERROR, "Internal Lame error: %i", iBytes);
    return 0;
  }

  if (WriteStream(m_buffer, iBytes) != iBytes)
  {
    CLog::Log(LOGERROR, "Error writing Lame buffer to file");
    return 0;
  }

  return 1;
}

bool CEncoderLame::Close()
{
  // may return one more mp3 frames
  int iBytes = m_dll.lame_encode_flush(m_pGlobalFlags, m_buffer, sizeof(m_buffer));

  if (iBytes < 0)
  {
    CLog::Log(LOGERROR, "Internal Lame error: %i", iBytes);
    return false;
  }

  WriteStream(m_buffer, iBytes);
  FlushStream();
  FileClose();

  // open again, but now the old way, lame only accepts FILE pointers
  FILE* file = fopen_utf8(m_strFile.c_str(), "rb+");
  if (!file)
  {
    CLog::Log(LOGERROR, "Error: Cannot open file for writing tags: %s", m_strFile.c_str());
    return false;
  }

  m_dll.lame_mp3_tags_fid(m_pGlobalFlags, file); /* add VBR tags to mp3 file */
  fclose(file);

  m_dll.lame_close(m_pGlobalFlags);

  // unload the lame dll
  m_dll.Unload();

  // Store a id3 tag in the ripped file
  CID3Tag id3tag;
  CMusicInfoTag tag;
  tag.SetAlbum(m_strAlbum);
  tag.SetArtist(m_strArtist);
  tag.SetGenre(m_strGenre);
  tag.SetTitle(m_strTitle);
  tag.SetTrackNumber(atoi(m_strTrack.c_str()));
  SYSTEMTIME time;
  time.wYear=atoi(m_strYear.c_str());
  tag.SetReleaseDate(time);
  id3tag.SetMusicInfoTag(tag);
  id3tag.Write(m_strFile);

  return true;
}
