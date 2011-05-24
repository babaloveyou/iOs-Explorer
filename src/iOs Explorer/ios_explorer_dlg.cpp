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
#include "stdafx.h"
#include "ios_explorer_dlg.h"

#include "ios_explorer.h"

#include "rename_dialog.h"
#include "add_fav_dialog.h"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CIOSExplorerDlg::CIOSExplorerDlg(CWnd* pParent /*=NULL*/)
: CDialog(CIOSExplorerDlg::IDD, pParent), m_statusThread(NULL), m_favCount(0)
{
	OleInitialize(NULL);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIOSExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_indexList);
	DDX_Control(pDX, IDC_LIST1, m_fileList);
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_FAV, m_favBtn);
	DDX_Control(pDX, IDC_PATH, m_path);
	DDX_Control(pDX, IDC_BACKWARD, m_backWardBtn);
	DDX_Control(pDX, IDC_FORWARD, m_farWardBtn);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_EXPLORER1, m_preview);
}

//ID_RINGTONE: /Library/Ringtones
//ID_THRIDRINGTONE: /private/var/root/Library/Ringtones
//ID_UISound: /System/Library/Audio/UISounds
//ID_FONT: /System/Library/FOnts
//ID_WALLPAPER: /Library/Wallpaper
//ID_SPRING: /System/Library/CoreServices/SpringBoard.app
//ID_WINTER: /Library/Themes
//ID_SUMMER: /private/var/mobile/Library/Summerboard
//
//
//ID_HANZIFY: /Applications/Preferences.app/zh_CN.lproj
//ID_PXL: /private/var/root/Media/PXL
//ID_CRASHREPORT: /private/var/logs/CrashReporter
//ID_ADDBOOK: /private/var/root/Library/AddressBook
//ID_SMS: /private/var/root/Library/SMS
//ID_PROCESS: /private/var/run
//
//ID_MUSIC: /private/var/mobile/Media/iTunes_Contrl/Music
//ID_PODCAST: /private/var/mobile/Media/Podcasts
//ID_PHOTO: /private/var/mobile/Media/Photos
//ID_CAM: /private/var/mobile/Media/DCIM
//ID_PDF: /var/root/Media/PDF
//ID_RECORD: /var/root/Library/Recordings

BEGIN_MESSAGE_MAP(CIOSExplorerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, &CIOSExplorerDlg::OnNMDblclkFilelist)
	ON_NOTIFY(NM_RCLICK, IDC_FILELIST, &CIOSExplorerDlg::OnNMRclickFilelist)
	ON_COMMAND(ID_OPEN, &CIOSExplorerDlg::OnOpen)
	ON_COMMAND(ID_OPEN_WITH, &CIOSExplorerDlg::OnOpenWith)
	ON_COMMAND(ID_COPYTO, &CIOSExplorerDlg::OnCopyTo)
	ON_COMMAND(ID_DEL, &CIOSExplorerDlg::OnDel)
	ON_COMMAND(ID_RENAME, &CIOSExplorerDlg::OnRename)
	ON_COMMAND(ID_PASTE, &CIOSExplorerDlg::OnPaste)
	ON_COMMAND(ID_COPYALLTO, &CIOSExplorerDlg::OnCopyAllTo)
	ON_COMMAND(ID_DELALL, &CIOSExplorerDlg::OnDelAll)
	ON_COMMAND(ID_NEW, &CIOSExplorerDlg::OnNew)
	ON_COMMAND(ID_COPYPATH, &CIOSExplorerDlg::OnCopypath)
	ON_BN_CLICKED(IDC_FAV, &CIOSExplorerDlg::OnBnClickedFav)

	ON_COMMAND(ID_RINGTONE, &CIOSExplorerDlg::GotoRingtones)
	ON_COMMAND(ID_THRIDRINGTONE, &CIOSExplorerDlg::GotoThridRingtones)
	ON_COMMAND(ID_UISound, &CIOSExplorerDlg::GotoUISounds)
	ON_COMMAND(ID_FONT, &CIOSExplorerDlg::GotoFonts)
	ON_COMMAND(ID_WALLPAPER, &CIOSExplorerDlg::GotoWallpaper)
	ON_COMMAND(ID_SPRING, &CIOSExplorerDlg::GotoSpringBoard)
	ON_COMMAND(ID_WINTER, &CIOSExplorerDlg::GotoThemes)
	ON_COMMAND(ID_SUMMER, &CIOSExplorerDlg::GotoSummerboard)

	ON_COMMAND(ID_HANZIFY, &CIOSExplorerDlg::Gotozh_CN)
	ON_COMMAND(ID_PXL, &CIOSExplorerDlg::GotoPXL)
	ON_COMMAND(ID_CRASHREPORT, &CIOSExplorerDlg::GotoCrashReporter)
	ON_COMMAND(ID_ADDBOOK, &CIOSExplorerDlg::GotoAddressBook)
	ON_COMMAND(ID_SMS, &CIOSExplorerDlg::GotoSMS)
	ON_COMMAND(ID_PROCESS, &CIOSExplorerDlg::GotoRun)

	ON_COMMAND(ID_MUSIC, &CIOSExplorerDlg::GotoMusic)
	ON_COMMAND(ID_PODCAST, &CIOSExplorerDlg::GotoPodcasts)
	ON_COMMAND(ID_PHOTO, &CIOSExplorerDlg::GotoPhotos)
	ON_COMMAND(ID_CAM, &CIOSExplorerDlg::GotoDCIM)
	ON_COMMAND(ID_PDF, &CIOSExplorerDlg::GotoPDF)
	ON_COMMAND(ID_RECORD, &CIOSExplorerDlg::GotoRecordings)
	ON_COMMAND_RANGE(ID_CUSTOM_MENU_MIN, ID_CUSTOM_MENU_MAX, &CIOSExplorerDlg::OnFavMenuClicled)
	ON_COMMAND_RANGE(ID_CUSTOM_MENU_DEL_MIN, ID_CUSTOM_MENU_DEL_MAX, &CIOSExplorerDlg::OnDelFavMenu)

	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_GO, &CIOSExplorerDlg::OnBnClickedGo)
	ON_BN_CLICKED(IDC_BACKWARD, &CIOSExplorerDlg::OnBnClickedBackward)
	ON_BN_CLICKED(IDC_FORWARD, &CIOSExplorerDlg::OnBnClickedForward)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILELIST, &CIOSExplorerDlg::OnNMClickFilelist)
END_MESSAGE_MAP()

BOOL CIOSExplorerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_ysplitter.BindWithControl(this, IDC_YSPLITTER);  // bind the splitter object to the puppet control
	m_ysplitter.AttachAsAbovePane(IDC_FILELIST);  // attach a control within the same dialogbox as a pane of the splitter
	m_ysplitter.AttachAsBelowPane(IDC_EXPLORER1);       // you can even attach another splitter as a pane
	m_ysplitter.SetMinHeight(50, 50);
	m_ysplitter.RecalcLayout();

	wchar_t appPath[MAX_PATH];
	::GetModuleFileName(NULL, appPath, MAX_PATH);
	size_t size = wcslen(appPath);
	while(size)
	{
		if(appPath[size] == _T('\\'))
		{
			appPath[size] = _T('\0');
			break;
		}
		--size;
	}

	m_indexList.SetAppPath(appPath);
	m_indexList.Init(&m_fileList, &m_path, &m_progress);

	std::wstring favConfile = std::wstring(appPath) + _T("\\favorite.ini");
	LoadFavFromFile(favConfile);

	m_statusThread = new boost::thread(boost::bind(&CIOSExplorerDlg::ShowStatus, this));

	std::wstring tempFilePath = std::wstring(appPath) + _T("\\temp\\temp.html");
	std::ofstream tempFile(tempFilePath.c_str(), std::ios::trunc | std::ios::binary);
	if (tempFile.is_open())
	{
		tempFile.write("\xFF\xFE", 2);
		std::wstring content = L"<html><head><title>temp</title></head><body><center><h3>单击某项显示预览</h3></center></body></html>";
		tempFile.write((char *)content.c_str(), (std::streamsize)content.size() * 2);
		tempFile.close();
	}
	m_preview.Navigate(tempFilePath.c_str(), NULL, NULL, NULL, NULL);

	return TRUE;
}

void CIOSExplorerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CIOSExplorerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CIOSExplorerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIOSExplorerDlg::ShowStatus()
{
	while (true)
	{
		if(!m_hWnd)
		{
			continue;
		}
		if(!IsWindowVisible())
		{
			continue;
		}
		else
		{
			m_status.SetWindowText(m_indexList.GetLastStatus().c_str());
		}
		Sleep(2000);
	}
}

void CIOSExplorerDlg::OnSize(UINT nType, int cx, int cy)
{
	if (IsWindowVisible())
	{
		// ugly?
		CRect rect;
		rect.top = 0;
		rect.bottom = 21;
		rect.left = cx - 68;
		rect.right = cx;
		m_favBtn.MoveWindow(rect);

		rect.left = rect.left - 21;
		rect.right = rect.left + 21;
		this->GetDlgItem(IDC_GO)->MoveWindow(rect);

		rect.right = rect.left;
		rect.left = 210 + 21 + 21;
		m_path.MoveWindow(rect);

		rect.left = 210;
		rect.right = rect.left + 21;
		m_backWardBtn.MoveWindow(rect);
		rect.left = rect.right;
		rect.right = rect.left + 21;
		m_farWardBtn.MoveWindow(rect);

		rect.left = 0;
		rect.right = 210;
		rect.top = 0;
		rect.bottom = cy - 21;
		m_indexList.MoveWindow(rect);

		CRect previewRc;
		m_preview.GetClientRect(previewRc);
		rect.top = 21;
		rect.bottom = cy - 21 - previewRc.Height() - 2;
		rect.left = rect.right;
		rect.right = cx;
		m_fileList.MoveWindow(rect);

		rect.top = rect.bottom;
		rect.bottom = rect.top + 2;
		m_ysplitter.MoveWindow(rect);

		rect.top = rect.bottom;
		rect.bottom = rect.top + previewRc.Height();
		m_preview.MoveWindow(rect);

		rect.top = rect.bottom;
		rect.bottom = cy;
		rect.left = cx - 110;
		rect.right = cx;
		m_progress.MoveWindow(rect);

		rect.right = rect.left;
		rect.left = 0;
		m_status.MoveWindow(rect);

		m_fileList.GetClientRect(rect);
		int width = (int)(rect.Width() * 0.15);
		m_fileList.SetColumnWidth(0, rect.Width() - width * 4);
		m_fileList.SetColumnWidth(1, width);
		m_fileList.SetColumnWidth(2, width);
		m_fileList.SetColumnWidth(3, width);
		m_fileList.SetColumnWidth(4, width);
	}
	Invalidate();
	CDialog::OnSize(nType, cx, cy);
}

void CIOSExplorerDlg::OnNMClickFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	CIOSDirectoryTree::m_askIsTransPng = false;
	CIOSDirectoryTree::m_askIsTransPlist = false;

	int iItem = m_fileList.GetNextItem(-1,LVNI_SELECTED);
	if(iItem == -1)
	{
		return ;
	}

	CString name = m_fileList.GetItemText(iItem, 0);
	if (!name.Compare(_T("..")))
	{
		return ;
	}

	if(!m_indexList.IsFolderItem(name.GetString()))
	{
		std::wstring tempPreviewPath = m_indexList.CopyItemToTemp(iItem);
		if(!tempPreviewPath.empty())
		{
			m_preview.Navigate(tempPreviewPath.c_str(), NULL, NULL, NULL, NULL);
		}
	}
	*pResult = 0;

	CIOSDirectoryTree::m_askIsTransPng = true;
	CIOSDirectoryTree::m_askIsTransPlist = true;
}

void CIOSExplorerDlg::OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_indexList.OpenSelectedFileItem();
	*pResult = 0;
}

void CIOSExplorerDlg::OnNMRclickFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iItem = m_fileList.GetNextItem(-1,LVNI_SELECTED);
	int subMenuIndex = 0;;
	if( iItem != -1)
	{
		iItem = m_fileList.GetNextItem(iItem,LVNI_SELECTED);
		if(iItem == -1)
		{	
			subMenuIndex = 1;
		}
		else
		{
			subMenuIndex = 2;
		}
	}

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_FILELIST_MENU));
	CMenu* pPopup = menu.GetSubMenu(subMenuIndex);
	ASSERT(pPopup != NULL);

	POINT clickPoint;
	::GetCursorPos(&clickPoint);

	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, clickPoint.x,  clickPoint.y, this);

	*pResult = 0;
}

void CIOSExplorerDlg::OnOpen()
{
	m_indexList.OpenSelectedFileItem();
}

void CIOSExplorerDlg::OnOpenWith()
{
	m_indexList.OpenSelectedFileItem(true);
}

void CIOSExplorerDlg::OnCopyTo()
{
	CIOSDirectoryTree::m_askIsTransPng = true;
	CIOSDirectoryTree::m_askIsTransPlist = true;
	int iItem = m_fileList.GetNextItem(-1,LVNI_SELECTED);
	if(iItem == -1)
	{
		return ;
	}

	CString name = m_fileList.GetItemText(iItem, 0);
	if (!name.Compare(_T("..")))
	{
		return ;
	}

	if(!m_indexList.IsFolderItem(name.GetString()))
	{
		CFileDialog fileDlg(FALSE, NULL, name, OFN_ENABLESIZING|OFN_OVERWRITEPROMPT ,_T("所有文件(*.*)|*.*||"), NULL);
		if(fileDlg.DoModal() == IDOK)
		{
			CString target = fileDlg.GetPathName();
			if(!m_indexList.CopySelectedItemTo(target.GetString()))
			{
				::MessageBox(GetSafeHwnd(), _T("复制失败"), _T("失败"), MB_OK|MB_ICONWARNING);
				
			}
			else
			{
				// ::MessageBox(GetSafeHwnd(), _T("复制成功"), _T("成功"), MB_OK|MB_ICONINFORMATION);
			}
		}
	}
	else
	{
		OnCopyAllTo();
	}
	CIOSDirectoryTree::m_askIsTransPng = true;
	CIOSDirectoryTree::m_askIsTransPlist = true;
}

void CIOSExplorerDlg::OnDel()
{
	int iItem = m_fileList.GetNextItem(-1,LVNI_SELECTED);
	if(iItem == -1)
	{
		return ;
	}

	CString name = m_fileList.GetItemText(iItem, 0);
	if (!name.Compare(_T("..")))
	{
		return ;
	}

	if(::MessageBox(GetSafeHwnd(), _T("确实要把此文件删除吗?"), _T("删除文件"), MB_YESNO|MB_ICONINFORMATION) == IDYES)
	{
		if(!m_indexList.RemoveSelectedFile())
		{
			::MessageBox(GetSafeHwnd(), _T("删除失败"), _T("失败"), MB_OK|MB_ICONWARNING);
		}
		else
		{
			//::MessageBox(GetSafeHwnd(), _T("删除成功"), _T("成功"), MB_OK|MB_ICONINFORMATION);
		}
	}
}

void CIOSExplorerDlg::OnRename()
{
	int iItem = m_fileList.GetNextItem(-1,LVNI_SELECTED);
	if(iItem == -1)
	{
		return ;
	}

	CString name = m_fileList.GetItemText(iItem, 0);
	if (!name.Compare(_T("..")))
	{
		return ;
	}

	CRenameDialog dlg;
	dlg.SetName(name);
	if(dlg.DoModal() == IDOK)
	{
		if(!m_indexList.RenameSelectedFile(dlg.GetName().GetString()))
		{
			::MessageBox(GetSafeHwnd(), _T("重命名失败"), _T("失败"), MB_OK|MB_ICONWARNING);
		}
		else
		{
			// ::MessageBox(GetSafeHwnd(), _T("重命名成功"), _T("成功"), MB_OK|MB_ICONINFORMATION);
		}
	}
}

void CIOSExplorerDlg::DropFilesToIOSDevice( HDROP hDrop )
{
	UINT uDropEffect=RegisterClipboardFormat(_T("Preferred DropEffect"));
	if( hDrop) {
		DWORD dwEffect,*dw;
		dw=(DWORD*)(GetClipboardData( uDropEffect));
		if(dw==NULL)
			dwEffect=DROPEFFECT_COPY;
		else
			dwEffect=*dw;

		UINT cFiles = DragQueryFile( hDrop, (UINT) -1, NULL, 0);
		wchar_t szFile[ MAX_PATH];
		for( UINT count = 0; count < cFiles; count++ )
		{
			DragQueryFile(hDrop, count, szFile, sizeof(szFile));

			if(!m_indexList.PasteToSelectedFolder(szFile))
			{
				::MessageBox(GetSafeHwnd(), _T("移动失败"), _T("失败"), MB_OK|MB_ICONWARNING);
			}
			else
			{
				// ::MessageBox(GetSafeHwnd(), _T("粘贴成功"), _T("成功"), MB_OK|MB_ICONINFORMATION);
			}
		}
	}
}

void CIOSExplorerDlg::DropFiles(HDROP hDropInfo)
{
	DropFilesToIOSDevice(hDropInfo);
	DragFinish(hDropInfo);
}

void CIOSExplorerDlg::OnPaste()
{
	if(OpenClipboard())
	{
		HDROP hDrop = HDROP( GetClipboardData( CF_HDROP));
		DropFilesToIOSDevice(hDrop);
		CloseClipboard();
	}
}

void CIOSExplorerDlg::OnCopyAllTo()
{
	CIOSDirectoryTree::m_askIsTransPng = true;
	CIOSDirectoryTree::m_askIsTransPlist = true;
	BROWSEINFO bi;
	wchar_t Buffer[MAX_PATH];
	bi.hwndOwner = NULL;
	bi.pidlRoot =NULL;
	bi.pszDisplayName = Buffer;
	bi.lpszTitle = _T("选择路径");
	bi.ulFlags = BIF_EDITBOX;
	bi.lpfn = NULL;
	bi.iImage=IDR_MAINFRAME;
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);

	std::wstring sFolderPath;
	if(pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);
		sFolderPath = Buffer;
	}
	LPMALLOC lpMalloc;
	if(FAILED(SHGetMalloc(&lpMalloc)))
		return ;

	if(!pIDList)
	{
		lpMalloc->Free(pIDList);
		return ;
	}
	lpMalloc->Free(pIDList);
	lpMalloc->Release();

	if(!m_indexList.CopySelectedItemTo(sFolderPath + _T("\\"), true))
	{
		::MessageBox(GetSafeHwnd(), _T("复制失败"), _T("失败"), MB_OK|MB_ICONWARNING);
	}
	else
	{
		// ::MessageBox(GetSafeHwnd(), _T("复制成功"), _T("成功"), MB_OK|MB_ICONINFORMATION);
	}
	CIOSDirectoryTree::m_askIsTransPng = true;
	CIOSDirectoryTree::m_askIsTransPlist = true;
}

void CIOSExplorerDlg::OnDelAll()
{
	if(::MessageBox(GetSafeHwnd(), _T("确实要把这些文件删除吗?"), _T("删除文件"), MB_YESNO|MB_ICONINFORMATION) == IDYES)
	{
		if(!m_indexList.RemoveSelectedFile())
		{
			::MessageBox(GetSafeHwnd(), _T("删除失败"), _T("失败"), MB_OK|MB_ICONWARNING);
		}
		else
		{
			// ::MessageBox(GetSafeHwnd(), _T("删除成功"), _T("成功"), MB_OK|MB_ICONINFORMATION);
		}
	}
}

void CIOSExplorerDlg::OnNew()
{
	if(!m_indexList.CreateAtSelectedFolder())
	{
		::MessageBox(GetSafeHwnd(), _T("创建失败"), _T("失败"), MB_OK|MB_ICONWARNING);
	}
	else
	{
		// ::MessageBox(GetSafeHwnd(), _T(创建成功"), _T("成功"), MB_OK|MB_ICONINFORMATION);
	}
}

void CIOSExplorerDlg::OnCopypath()
{
	std::wstring path = m_indexList.GetSelectedFileItemPath();
	HGLOBAL hGlobal=GlobalAlloc(GHND|GMEM_SHARE, path.size() * 2 + 1);
	wchar_t *pGlobal=(wchar_t*)GlobalLock(hGlobal);
	for(size_t i=0; i < path.size(); i++)  
		pGlobal[i]= path[i];
	GlobalUnlock(hGlobal);
	if(OpenClipboard())
	{
		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT, hGlobal);
		CloseClipboard();
	}
}

void CIOSExplorerDlg::OnBnClickedFav()
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_FAV_MENU));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	POINT clickPoint;
	::GetCursorPos(&clickPoint);

	// Tip
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, clickPoint.x,  clickPoint.y, this);   
}

void CIOSExplorerDlg::GotoRingtones()
{
	m_indexList.Goto(_T("/Library/Ringtones"));
}
void CIOSExplorerDlg::GotoThridRingtones()
{
	m_indexList.Goto(_T("/private/var/root/Library/Ringtones"));
}
void CIOSExplorerDlg::GotoUISounds()
{
	m_indexList.Goto(_T("/System/Library/Audio/UISounds"));
}
void CIOSExplorerDlg::GotoFonts()
{
	m_indexList.Goto(_T("/System/Library/Fonts"));
}
void CIOSExplorerDlg::GotoWallpaper()
{
	m_indexList.Goto(_T("/Library/Wallpaper"));
}
void CIOSExplorerDlg::GotoSpringBoard()
{
	m_indexList.Goto(_T("/System/Library/CoreServices/SpringBoard.app"));
}
void CIOSExplorerDlg::GotoThemes()
{
	m_indexList.Goto(_T("/Library/Themes"));
}
void CIOSExplorerDlg::GotoSummerboard()
{
	m_indexList.Goto(_T("/private/var/mobile/Library/Summerboard"));
}

void CIOSExplorerDlg::Gotozh_CN()
{
	m_indexList.Goto(_T("/Applications/Preferences.app/zh_CN.lproj"));
}
void CIOSExplorerDlg::GotoPXL()
{
	m_indexList.Goto(_T("/private/var/root/Media/PXL"));
}
void CIOSExplorerDlg::GotoCrashReporter()
{
	m_indexList.Goto(_T("/private/var/logs/CrashReporter"));
}
void CIOSExplorerDlg::GotoAddressBook()
{
	m_indexList.Goto(_T("/private/var/mobile/Library/AddressBook"));
}
void CIOSExplorerDlg::GotoSMS()
{
	m_indexList.Goto(_T("/private/var/mobile/Library/SMS"));
}
void CIOSExplorerDlg::GotoRun()
{
	m_indexList.Goto(_T("/private/var/run"));
}

void CIOSExplorerDlg::GotoMusic()
{
	m_indexList.Goto(_T("/private/var/mobile/Media/iTunes_Control/Music"));
}
void CIOSExplorerDlg::GotoPodcasts()
{
	m_indexList.Goto(_T("/private/var/mobile/Media/Podcasts"));
}
void CIOSExplorerDlg::GotoPhotos()
{
	m_indexList.Goto(_T("/private/var/mobile/Media/Photos"));
}
void CIOSExplorerDlg::GotoDCIM()
{
	m_indexList.Goto(_T("/private/var/mobile/Media/DCIM"));
}
void CIOSExplorerDlg::GotoPDF()
{
	m_indexList.Goto(_T("/var/root/Media/PDF"));
}
void CIOSExplorerDlg::GotoRecordings()
{
	m_indexList.Goto(_T("/var/root/Library/Recordings"));
}

void CIOSExplorerDlg::OnDestroy()
{
	if (m_statusThread)
	{
		m_statusThread->interrupt();
		delete m_statusThread;
	}
	SavFavMenu();
	CDialog::OnDestroy();
}

BOOL CIOSExplorerDlg::PreTranslateMessage( MSG* pMsg )
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN   )    
	{  
		CString path;
		m_path.GetWindowText(path);
		m_indexList.Goto((LPCTSTR)path);
		return TRUE;
	}  
	return CDialog::PreTranslateMessage(pMsg);   
}

void CIOSExplorerDlg::OnFavMenuClicled(UINT nID)
{
	CString name;
	m_menu.GetMenuString(nID, name, MF_BYCOMMAND);
	if (nID == ID_CUSTOM_MENU_MIN)
	{
		AddFavMenu();
	}
	else if (nID == ID_CUSTOM_MENU_MAX)
	{
	}
	else if (m_favList.count(nID))
	{
		m_indexList.Goto(m_favList[nID].second.c_str());
	}
}

void CIOSExplorerDlg::SavFavMenu()
{
	std::ofstream oFile(m_favConfFile.c_str(), std::ios::binary|std::ios::out|std::ios::trunc);
	if (oFile.is_open())
	{
		oFile.write("\xFF\xFE", 2);
		for (std::map<int, std::pair<std::wstring, std::wstring> >::const_iterator iter = m_favList.begin(); 
			iter != m_favList.end(); ++iter)
		{
			std::wstring content = iter->second.first + _T("\r\n") + iter->second.second  + _T("\r\n");
			oFile.write((char *)content.c_str(), (std::streamsize)content.size() * 2);
		}
		oFile.close();
	}
}

void CIOSExplorerDlg::AddFavMenu()
{
	CAddFavDialog dlg;
	CString path;
	m_path.GetWindowText(path);
	dlg.SetFavAddress(path);
	boost::filesystem::wpath fPath(path.GetString());
	std::wstring fileName = fPath.filename().wstring();
	if (!fileName.compare(_T(".")))
	{
		fileName = fPath.parent_path().filename().wstring();
	}
	dlg.SetFavName(fileName.c_str());

	if (dlg.DoModal() == IDOK)
	{
		CMenu* pPopup = m_menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CMenu* pSubPopup = m_menu.GetSubMenu(1);
		ASSERT(pSubPopup != NULL);
		CString name = dlg.GetFavName(), address = dlg.GetFavAddress();
		pPopup->AppendMenu(MF_STRING, ID_CUSTOM_MENU_MIN + (++m_favCount) + 1,(LPCTSTR)name);
		pSubPopup->AppendMenu(MF_STRING, ID_CUSTOM_MENU_DEL_MIN + m_favCount + 1,(LPCTSTR)name);
		std::wstring nameStdStr = (LPCTSTR)name, addressStdStr = (LPCTSTR)address;
		m_favList[ID_CUSTOM_MENU_MIN + m_favCount + 1] = make_pair(nameStdStr, addressStdStr);
	}
}

void CIOSExplorerDlg::OnDelFavMenu( UINT nID )
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CMenu* pSubPopup = m_menu.GetSubMenu(1);
	ASSERT(pSubPopup != NULL);

	if (nID == ID_CUSTOM_MENU_DEL_MIN)
	{
		if (MessageBox(_T("确定要清空收藏夹?"), _T("清空收藏夹"), MB_OKCANCEL|MB_ICONWARNING) == IDCANCEL)
		{
			return ;
		}

		m_favList.clear();
		for (unsigned int i = 0; i < m_favCount; ++i)
		{
			pPopup->DeleteMenu(ID_CUSTOM_MENU_MIN + 2 + i, MF_BYCOMMAND);
			pSubPopup->DeleteMenu(ID_CUSTOM_MENU_DEL_MIN + 2 +i, MF_BYCOMMAND);
			DrawMenuBar();
		}
	}
	else
	{
		CString tip;
		pSubPopup->GetMenuString(nID, tip, MF_BYCOMMAND);
		tip = _T("确定要删除收藏夹:\r\n名称: ") + tip + _T("\r\n路径: ");
		unsigned int favID = nID - ID_CUSTOM_MENU_DEL_MIN + ID_CUSTOM_MENU_MIN;
		if (m_favList.count(favID))
		{
			tip += m_favList[favID].second.c_str();
		}
		
		if (MessageBox(tip, _T("删除收藏夹"), MB_OKCANCEL|MB_ICONWARNING) == IDCANCEL)
		{
			return ;
		}
		for (std::map<int, std::pair<std::wstring, std::wstring> >::iterator iter = m_favList.begin(); 
			iter != m_favList.end(); ++iter)
		{
			if (iter->first == favID)
			{
				m_favList.erase(iter);
				break;
			}
		}

		pSubPopup->DeleteMenu(nID, MF_BYCOMMAND);
		pPopup->DeleteMenu(favID, MF_BYCOMMAND);
		DrawMenuBar();
	}
}

void CIOSExplorerDlg::LoadFavFromFile(const std::wstring &favConfile)
{
	m_favConfFile = favConfile;
	VERIFY(m_menu.LoadMenu(IDR_FAVB_MENU));
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CMenu* pSubPopup = m_menu.GetSubMenu(1);
	ASSERT(pSubPopup != NULL);
	pPopup->AppendMenu(MF_POPUP,(UINT_PTR)(pSubPopup->m_hMenu),_T("删除"));
	pPopup->AppendMenu(MF_SEPARATOR, ID_CUSTOM_MENU_MIN + 1, (LPCTSTR)NULL);
	pSubPopup->AppendMenu(MF_SEPARATOR, ID_CUSTOM_MENU_DEL_MIN + 1, (LPCTSTR)NULL);

	std::ifstream favFile(favConfile.c_str(), std::ios::binary|std::ios::in);
	if (favFile.is_open())
	{
		favFile.seekg(0, std::ios::end);
		int fileLen = favFile.tellg();
		favFile.seekg(2, std::ios::beg);
		char *buf = new char[fileLen];
		favFile.read(buf, fileLen);
		char *p = buf;
		char *lastPos = p;
		bool isName = true;
		std::wstring name, address;
		while (p - buf < fileLen)
		{
			wchar_t uniStr = *(wchar_t *)p;
			if (uniStr == _T('\r') || p - buf == fileLen - 2)
			{
				if (isName)
				{
					name = std::wstring((wchar_t *)lastPos, (p - lastPos)/2);
					isName = false;
				}
				else
				{
					address = std::wstring((wchar_t *)lastPos, (p - lastPos)/2);
					isName = true;
					pPopup->AppendMenu(MF_STRING, ID_CUSTOM_MENU_MIN + (++m_favCount) + 1, name.c_str());
					pSubPopup->AppendMenu(MF_STRING, ID_CUSTOM_MENU_DEL_MIN + m_favCount + 1, name.c_str());
					m_favList[ID_CUSTOM_MENU_MIN + m_favCount + 1] = make_pair(name, address);
				}
				do
				{
					p += 2;
					uniStr = *(wchar_t *)p;
				}while ((uniStr == _T('\r') || uniStr== _T('\n')) && p - buf < fileLen);

				lastPos = p;
			}
			p += 2;
		}
		delete [] buf;
	}
}

void CIOSExplorerDlg::OnBnClickedGo()
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	POINT clickPoint;
	::GetCursorPos(&clickPoint);

	// Tip
	pPopup->TrackPopupMenu(TPM_TOPALIGN|TPM_LEFTBUTTON, clickPoint.x,  clickPoint.y, this);   
}

void CIOSExplorerDlg::OnBnClickedBackward()
{
	m_indexList.BackWard();
}

void CIOSExplorerDlg::OnBnClickedForward()
{
	m_indexList.ForWard();
}

