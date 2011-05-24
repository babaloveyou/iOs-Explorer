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

#pragma once
#include "afxcmn.h"
#include "resource.h"

#include <string>
#include <vector>
#include <map>

struct SSingleItemData
{
	int type;
	union
	{
		int intValue;
		wchar_t *strValue;
	};

	~SSingleItemData()
	{
		if(type == 1)
		{
			delete [] strValue;
		}
	};
};

struct SItemData
{
	std::vector<SSingleItemData *> itemData;

	void Insert(const int value)
	{
		SSingleItemData *pData = new SSingleItemData;
		pData->type = 0;
		pData->intValue = value;
		itemData.push_back(pData);
	};

	void Insert(const wchar_t *value)
	{
		SSingleItemData *pData = new SSingleItemData;
		pData->type = 1;
		int len = (int)wcslen(value);
		pData->strValue = new wchar_t[len + 1];
		pData->strValue[len] = _T('\0');
		memcpy(pData->strValue, value, (wcslen(value) + 1) * 2);
		itemData.push_back(pData);
	};

	~SItemData()
	{
		for (size_t i = 0; i < itemData.size(); ++i)
		{
			if(itemData[i])
			{
				delete itemData[i];
			}
		}
	};
};

class CIOSDirectoryTree;
class CIOSFileList :
	public CListCtrl
{
public:
	CIOSFileList(void);
	~CIOSFileList(void);

	DECLARE_DYNAMIC(CIOSFileList)

	void Init(CIOSDirectoryTree *iOsDirectoryTree);
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	BOOL DeleteAllItems();
	void SetHTreeItem(HTREEITEM hTreeItem);
	HTREEITEM GetHTreeItem();
	BOOL SetSItemData(int nItem, SItemData* itemData);

protected:
	int							m_chocedColumn;
	bool						m_bSortAscending;
	CImageList					m_imageList;
	HTREEITEM					m_hTreeItem;
	std::vector<SItemData *>	m_itemDatas;
	std::map<std::wstring, int>	m_extToImageIndex;
	CIOSDirectoryTree*			m_indexList;

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static std::wstring GetFileExt(LPCTSTR lpszItem);
	void FreeItemDatas();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnHdnItemdblclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);

};

