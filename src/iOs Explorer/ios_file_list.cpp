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

#include "StdAfx.h"
#include "ios_file_list.h"

#include "ole_drop_source.h"

#include "ios_explorer_dlg.h"
#include "ios_directory_tree.h"

IMPLEMENT_DYNAMIC(CIOSFileList,CListCtrl)
CIOSFileList::CIOSFileList(void): m_chocedColumn(-1), m_bSortAscending(true), m_indexList(NULL)
{
}

CIOSFileList::~CIOSFileList(void)
{
	FreeItemDatas();
}

void CIOSFileList::Init(CIOSDirectoryTree *iosDirectoryTree)
{
	SetBkColor(RGB(255,255,255));
	SetExtendedStyle(GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	CRect rect;
	GetClientRect(rect);
	int width = (int)(rect.Width() * 0.15);
	InsertColumn(0, _T("名称"), LVCFMT_LEFT, rect.Width() - width * 4);
	InsertColumn(1, _T("类型"), LVCFMT_LEFT, width);
	InsertColumn(2, _T("大小"), LVCFMT_RIGHT, width);
	InsertColumn(3, _T("块大小"), LVCFMT_RIGHT, width);
	InsertColumn(4, _T("链接数"), LVCFMT_LEFT, width);

	m_imageList.Create(16,16,ILC_COLOR8 | ILC_MASK,2,2);
	m_imageList.SetBkColor(RGB(255,255,255)); 

	CBitmap bitmap[2];

	bitmap[0].LoadBitmap(IDB_FOLDER);
	m_imageList.Add(&bitmap[0], RGB(255,255,255));

	bitmap[1].LoadBitmap(IDB_SYMBOL_LINK);
	m_imageList.Add(&bitmap[1], RGB(255,255,255));
	
	SetImageList(&m_imageList, LVSIL_SMALL);

	m_indexList = iosDirectoryTree;
}

std::wstring CIOSFileList::GetFileExt(LPCTSTR lpszItem)
{
	size_t pointPos = wcslen(lpszItem);
	LPCTSTR p = lpszItem + pointPos;
	
	while( p != lpszItem)
	{
		if(*p == _T('.'))
		{
			pointPos = p - lpszItem;
		}
		--p;
	}

	if(pointPos == wcslen(lpszItem))
	{
		return _T("*.exe");
	}

	return lpszItem + pointPos;
}

int CIOSFileList::InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
{
	int imageIndex = nImage;
	if(nImage == -1 || nImage == -2)
	{
		std::wstring ext;
		if (nImage == -1)
		{
			ext = GetFileExt(lpszItem);
		}
		else if (nImage == -2)
		{
			ext = _T("*.Lnk");
		}

		if(m_extToImageIndex.count(ext))
		{
			imageIndex = m_extToImageIndex[ext];
		}
		else
		{
			SHFILEINFO sfi;
			SHGetFileInfo(ext.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES|SHGFI_ICON);
			imageIndex = m_imageList.Add(sfi.hIcon);
			m_extToImageIndex[ext] = imageIndex;
		}
	}

	return CListCtrl::InsertItem(nItem, lpszItem, imageIndex);
}

void CIOSFileList::SetHTreeItem(HTREEITEM hTreeItem)
{
	m_hTreeItem = hTreeItem;
}

HTREEITEM CIOSFileList::GetHTreeItem()
{
	return m_hTreeItem;
}

void CIOSFileList::FreeItemDatas()
{
	for (size_t i = 0; i < m_itemDatas.size(); ++i)
	{
		if(m_itemDatas[i])
		{
			delete m_itemDatas[i];
		}
	}
	m_itemDatas.clear();
}

BOOL CIOSFileList::DeleteAllItems()
{
	FreeItemDatas();
	return CListCtrl::DeleteAllItems();
}

BOOL CIOSFileList::SetSItemData(int nItem, SItemData* itemData)
{
	m_itemDatas.push_back(itemData);
	return SetItemData(nItem, (DWORD_PTR)itemData);
}

int CALLBACK CIOSFileList::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if(!lParamSort)
	{
		return 0;
	}

	if(!lParam1)
	{
		return -1;
	}

	if(!lParam2)
	{
		return 1;
	}

	CIOSFileList* pListCtrl = reinterpret_cast<CIOSFileList*>( lParamSort );

	SItemData* pid1 = reinterpret_cast<SItemData*>( lParam1 );

	if(!pid1->itemData.empty())
	{
		if (!wcscmp(pid1->itemData[0]->strValue, _T("..")))
			return 1;
	}

	SItemData* pid2 = reinterpret_cast<SItemData*>( lParam2 );

	if( pListCtrl->m_chocedColumn < (int)(pid1->itemData.size()) && pListCtrl->m_chocedColumn < (int)(pid2->itemData.size()) )
	{
		SSingleItemData *data1 = pid1->itemData[pListCtrl->m_chocedColumn];
		SSingleItemData *data2 = pid2->itemData[pListCtrl->m_chocedColumn];
		if(data1->type == data2->type)
		{
			int iRtn = 0;
			switch(data1->type)
			{
			case 0:
				{
					iRtn = data1->intValue - data2->intValue;
				}
				break;
			case 1:
				{
					iRtn = wcscmp(data1->strValue, data2->strValue);
				}
				break;
			default:
				{
				}
				break;
			}
			if(pListCtrl->m_bSortAscending)
			{
				return iRtn;
			}
			else
			{
				return 0 - iRtn;
			}
		}
	}

	return 0;
}

BEGIN_MESSAGE_MAP(CIOSFileList, CListCtrl)
ON_NOTIFY(HDN_ITEMCLICKA, 0, &CIOSFileList::OnHdnItemclick)
ON_NOTIFY(HDN_ITEMCLICKW, 0, &CIOSFileList::OnHdnItemclick)
ON_WM_DROPFILES()
ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CIOSFileList::OnLvnBegindrag)
END_MESSAGE_MAP()

void CIOSFileList::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	m_chocedColumn = phdr->iItem;
	SortItems(CompareFunc, (DWORD_PTR)this);
	m_bSortAscending = !m_bSortAscending;
	*pResult = 0;
}

void CIOSFileList::OnDropFiles(HDROP hDropInfo)
{
	CWnd *pParent = this->GetParent();
	CIOSExplorerDlg *pParentDlg = (CIOSExplorerDlg *)pParent;
	pParentDlg->DropFiles(hDropInfo);
	CListCtrl::OnDropFiles(hDropInfo);
}

void CIOSFileList::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	CIOSDirectoryTree::m_askIsTransPng = true;
	CIOSDirectoryTree::m_askIsTransPlist = true;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	UINT uFileCount = 0;
	int iItem = GetNextItem(-1,LVNI_SELECTED);
	std::vector<std::wstring> names;
	std::vector<int> items;
	while (iItem != -1)
	{
		std::wstring name = this->GetItemText(iItem, 0).GetString();
		std::wstring path = m_indexList->GetPathByHTreeItem(m_hTreeItem) + name;
		if (m_indexList->IsFolderItem(path, true))
		{
			iItem = GetNextItem(iItem,LVNI_SELECTED);
			continue;
		}
		++uFileCount;
		names.push_back(name);
		items.push_back(iItem);
		iItem = GetNextItem(iItem,LVNI_SELECTED);
	}

	UINT uBuffSize = sizeof(FILEGROUPDESCRIPTOR) + (uFileCount) * sizeof(FILEDESCRIPTOR);
	HGLOBAL hFileDescriptor = GlobalAlloc (GHND | GMEM_SHARE, uBuffSize);        
	if(hFileDescriptor)
	{
		FILEGROUPDESCRIPTOR* pGroupDescriptor = (FILEGROUPDESCRIPTOR*) GlobalLock ( hFileDescriptor );
		if(pGroupDescriptor)
		{
			FILEDESCRIPTOR* pFileDescriptorArray = (FILEDESCRIPTOR*)((LPBYTE)pGroupDescriptor + sizeof(UINT));
			pGroupDescriptor->cItems = uFileCount;
			for (size_t index = 0; index < names.size(); ++index)
			{          
				ZeroMemory(&pFileDescriptorArray[index], sizeof(FILEDESCRIPTOR));
				pFileDescriptorArray[index].dwFlags = FD_FILESIZE|FD_ATTRIBUTES;

				int size = 0;
				for (size_t i = 0; i < m_itemDatas.size(); ++i)
				{
					if (m_itemDatas[i])
					{
						if (!wcscmp(m_itemDatas[i]->itemData[0]->strValue, names[index].c_str()))
						{
							size = m_itemDatas[i]->itemData[2]->intValue;
							break;
						}
					}
				}
				pFileDescriptorArray[index].nFileSizeLow = size;
				pFileDescriptorArray[index].nFileSizeHigh = 0;
				pFileDescriptorArray[index].dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
				lstrcpy ( pFileDescriptorArray[index].cFileName, names[index].c_str());	
			}
			GlobalUnlock ( hFileDescriptor ); 
		}
	}
	else
	{
		GlobalFree ( hFileDescriptor );
	}


	CHOleDataSource dragDataSource;
	dragDataSource.SetAssociat(items, m_indexList);
	FORMATETC etcDescriptor = { RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR), NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	dragDataSource.CacheGlobalData( RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR), hFileDescriptor, &etcDescriptor );

	FORMATETC etcContents = {RegisterClipboardFormat(CFSTR_FILECONTENTS), NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	dragDataSource.DelayRenderFileData(RegisterClipboardFormat(CFSTR_FILECONTENTS), &etcContents);

	DROPEFFECT dwEffect = dragDataSource.DoDragDrop( DROPEFFECT_COPY | DROPEFFECT_MOVE );
	if(dwEffect == DROPEFFECT_NONE )
	{
		GlobalFree( hFileDescriptor );
	} 

	CIOSDirectoryTree::m_askIsTransPng = true;
	CIOSDirectoryTree::m_askIsTransPlist = true;
	*pResult = 0;
}
