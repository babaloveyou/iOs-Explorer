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

#ifndef IOS_DIRECTORY_TREE_H_
#define IOS_DIRECTORY_TREE_H_

#include <map>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include <afxcmn.h>

#include "ios_file_list.h"

#include "ios_io.h"

using namespace iOsIO;
class CIOSDirectoryTree : public CTreeCtrl
{
public:
	CIOSDirectoryTree(void);
	~CIOSDirectoryTree(void);

	void SetAppPath(const wchar_t* appPath);

	bool Init(CIOSFileList *list, CEdit *path, CProgressCtrl *progress);

	void ExpandRoot();

	void Goto(const wchar_t* path, bool addToVisited = true);

	void OpenSelectedFileItem(const bool openWith = false);
	void OpenFileItem(int iItem, const bool openWith = false);

	std::wstring GetSelectedFileItemPath();

	bool RemoveSelectedFile();
	bool RenameSelectedFile(const std::wstring &toName);

	bool IsLnkToDirectory(const wchar_t *path);

	bool CreateAtSelectedFolder();
	bool PasteToSelectedFolder(const std::wstring &src);

	bool CopyTo(const std::wstring &target, const std::wstring &src, const bool transLater = true, const bool toTxt = false);
	bool CopyTo(const std::wstring &target, const std::wstring &src, const FileInfo inf, const bool transLater = true, const bool toTxt = false);
	bool CopyDicTo(const std::wstring &target, const std::wstring &src);
	bool CopyItemTo(int iItem, const std::wstring &target, const bool isFolder = false, const bool toTxt = false);
	bool CopySelectedItemTo(const std::wstring &target, const bool isFolder = false);
	bool CopyItemToCFile(int iItem, CFile *pFile);
	bool CopyNormalFileToCFile(const std::wstring &src, CFile *pFile);
	std::wstring CopyItemToTemp(int iItem, const bool isFolder = false);
	int IsFolderItem(const std::wstring &name, const bool isFullPath = false);
	bool IsBinaryPlist(const std::wstring &name, const bool isFullPath = false);

	std::wstring GetLastStatus();
	std::wstring GetPathByHTreeItem(HTREEITEM hTreeItem);

	void BackWard();
	void ForWard();

	static bool					m_askIsTransPng;
	static bool					m_askIsTransPlist;

private:
	CiOsIO*						m_iOsDevice;
	bool						m_isWarding;

	CImageList					m_imageList;
	HTREEITEM					m_hTreeItemRoot;
	std::map<HTREEITEM,bool>	m_cachedItem;
	CIOSFileList*					m_fileList;
	CEdit*						m_path;
	CProgressCtrl*				m_progress;
	std::wstring				m_status;
	std::wstring				m_appFolder;
	CString						m_visitedPaths[7];
	unsigned short				m_visitedPos;

	boost::thread				*m_expandThread;

	void AddToVisitedPath(const wchar_t* path);
	void EnumHTreeItem(HTREEITEM hItem);
	bool CacheFolder(const std::wstring &folderPath, HTREEITEM folderHTreeItem, std::wstring &error);

	void InsertFileItem(const std::wstring &subPath, const std::wstring &name, const size_t index);
	bool PasteFile(boost::filesystem::wpath &inPath, const std::wstring &outPath, bool needInsert = true);
	bool PasteDirectory(boost::filesystem::wpath &inPath, const std::wstring &outPath, HTREEITEM lastItem);
	bool RemoveItem(const std::wstring &src);
	HTREEITEM GetHTreeItemByName(LPCTSTR name, HTREEITEM parentHTreeItem);
	int GetItemByName(LPCTSTR name, int flag = LVNI_SELECTED);
	static std::string UTF16ToGB(const std::wstring &utf16Str);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
};

#endif // IOS_DIRECTORY_TREE_H_

