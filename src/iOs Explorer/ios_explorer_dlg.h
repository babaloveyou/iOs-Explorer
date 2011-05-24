/*
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# The Original Code is Copyright (C) 2009 by exherb (htpp://4leaf.me).
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): none yet.
#
*/

#ifndef IOS_EXPLORER_DLG_H_
#define IOS_EXPLORER_DLG_H_

#include <afxwin.h>
#include <afxcmn.h>

#include "ie_wrapper.h"

#include "ios_directory_tree.h"
#include "ios_file_list.h"
#include "splitter_window.h"

class CIOSExplorerDlg : public CDialog
{
public:
	CIOSExplorerDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_IOS_EXPLORER_DIALOG };
	void DropFiles( HDROP hDropInfo );

private:
	HICON													m_hIcon;

	CySplitterWnd											m_ysplitter;
	CIOSDirectoryTree										m_indexList;
	CIOSFileList												m_fileList;
	CIEWrapper												m_preview;

	CEdit													m_status;
	CEdit													m_path;
	CButton													m_favBtn;
	CMenu													m_menu;
	CButton													m_backWardBtn;
	CButton													m_farWardBtn;
	CProgressCtrl											m_progress;
	std::map<int, std::pair<std::wstring, std::wstring> >	m_favList;
	unsigned int											m_favCount;
	std::wstring											m_favConfFile;
	boost::thread*											m_statusThread;

	void AddFavMenu();
	void SavFavMenu();
	void LoadFavFromFile(const std::wstring &favConfile);

	BOOL PreTranslateMessage(MSG* pMsg);

	void ShowStatus();

	void DropFilesToIOSDevice( HDROP hDrop );

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnFavMenuClicled(UINT nID);
	afx_msg void OnDelFavMenu(UINT nID);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOpen();
	afx_msg void OnOpenWith();
	afx_msg void OnCopyTo();
	afx_msg void OnDel();
	afx_msg void OnRename();
	afx_msg void OnPaste();
	afx_msg void OnCopyAllTo();
	afx_msg void OnDelAll();
	afx_msg void OnNew();
	afx_msg void OnCopypath();
	afx_msg void OnBnClickedFav();

	afx_msg void GotoRingtones();
	afx_msg void GotoThridRingtones();
	afx_msg void GotoUISounds();
	afx_msg void GotoFonts();
	afx_msg void GotoWallpaper();
	afx_msg void GotoSpringBoard();
	afx_msg void GotoThemes();
	afx_msg void GotoSummerboard();

	afx_msg void Gotozh_CN();
	afx_msg void GotoPXL();
	afx_msg void GotoCrashReporter();
	afx_msg void GotoAddressBook();
	afx_msg void GotoSMS();
	afx_msg void GotoRun();

	afx_msg void GotoMusic();
	afx_msg void GotoPodcasts();
	afx_msg void GotoPhotos();
	afx_msg void GotoDCIM();
	afx_msg void GotoPDF();
	afx_msg void GotoRecordings();

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedGo();

	afx_msg void OnBnClickedBackward();
	afx_msg void OnBnClickedForward();
	afx_msg void OnNMClickFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	
};

#endif // IOS_EXPLORER_DLG_H_

