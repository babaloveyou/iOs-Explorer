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
#include "ios_directory_tree.h"

#include <string>
#include <vector>

#include "boost/lexical_cast.hpp"
#include "boost/thread.hpp"
#include "boost/filesystem/fstream.hpp"

#include "mac_png_conv.h"
#include "plist_conv.h"

#include "Resource.h"

#pragma warning(push)
#pragma warning(disable:4996)
#pragma warning(disable:4311)

bool CIOSDirectoryTree::m_askIsTransPng = true;
bool CIOSDirectoryTree::m_askIsTransPlist = true;
CIOSDirectoryTree::CIOSDirectoryTree(): m_iOsDevice(NULL), m_fileList(NULL), m_expandThread(NULL),m_visitedPos(0),m_isWarding(false)
{
	
}

void CIOSDirectoryTree::SetAppPath(const wchar_t* appPath)
{
	m_appFolder = appPath;

	boost::filesystem::wpath tempFolder(m_appFolder + L"\\temp");
	try
	{
		boost::filesystem::create_directories(tempFolder);
	}
	catch(...)
	{
		return ;
	}
}

CIOSDirectoryTree::~CIOSDirectoryTree(void)
{
	boost::filesystem::wpath tempFolder(m_appFolder + L"\\temp");
	try
	{
		boost::filesystem::remove_all(tempFolder);
	}
	catch(...)
	{
		return ;
	}

	if(m_iOsDevice)
	{
		delete m_iOsDevice;
	}
}

bool CIOSDirectoryTree::PasteFile(boost::filesystem::wpath &inPath, const std::wstring &outPath, bool needInsert)
{
	if (IsBinaryPlist(outPath.c_str(), true))
	{
		CPlistConv plist((m_appFolder + L"\\CoreFoundation.dll").c_str());
		if ( plist.XMLToBinary( inPath.wstring().c_str(), (m_appFolder + L"\\temp\\" + inPath.filename().wstring()).c_str() ) )
		{
			inPath = m_appFolder + L"\\temp\\" + inPath.filename().wstring();
		}
	}
	boost::filesystem::ifstream inFile(inPath, std::ios::binary);
	if (inFile.is_open())
	{
		iOsFile outFile = 0;
		try
		{
			outFile = m_iOsDevice->OpenFileW((outPath + inPath.filename().wstring()).c_str());
		}
		catch (std::wstring &e)
		{
			m_status = e;
			return false;
		}

		inFile.seekg(0, std::ios::end);
		std::streamsize fileLen = inFile.tellg();
		if(fileLen)
		{
			inFile.seekg(0, std::ios::beg);
			std::streamsize bufLen = fileLen;
			std::streamsize readLen = 0;
			if(bufLen > 10 * 1024 * 1024)
			{
				bufLen = 10 * 1024 * 1024;
			}
			do
			{
				char *buf = new char[bufLen];
				inFile.read(buf, bufLen);
				try
				{
					m_iOsDevice->WriteFile(outFile, buf, bufLen);
				}
				catch (std::wstring &e)
				{
					m_iOsDevice->CloseFile(outFile);
					m_status = e;
					return false;
				}
				delete [] buf;
				readLen += bufLen;
			}while(readLen < fileLen);
		}
		m_iOsDevice->CloseFile(outFile);
		if (needInsert)
		{
			InsertFileItem(outPath + inPath.filename().wstring(), inPath.filename().wstring(), 0);
		}
		
		return true;
	}
	return false;
}

bool CIOSDirectoryTree::CreateAtSelectedFolder()
{
	std::wstring newName = _T("新建文件夹");
	unsigned short  snail = 0;
	while (GetItemByName(newName.c_str(), LVNI_ALL) != -1)
	{
		newName = _T("新建文件夹") + boost::lexical_cast<std::wstring>(++snail);
	}

	std::wstring outPath = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + newName.c_str();
	try
	{
		m_iOsDevice->CreateDirectoryW(outPath.c_str());
	}
	catch (std::wstring &e)
	{
		m_status = e;
		return false;
	}

	this->InsertItem(newName.c_str(), 1, 1, m_fileList->GetHTreeItem());
	InsertFileItem(outPath, newName, 0);
	return true;
}

bool CIOSDirectoryTree::PasteDirectory(boost::filesystem::wpath &inPath, const std::wstring &outPath, HTREEITEM lastItem)
{
	try
	{
		m_iOsDevice->CreateDirectoryW((outPath + inPath.filename().wstring()).c_str());
	}
	catch (std::wstring &e)
	{
		m_status = e;
		return false;
	}

	if (lastItem == m_fileList->GetHTreeItem())
	{
		InsertFileItem(outPath, inPath.filename().wstring(), 0);
	}

	HTREEITEM subItem = NULL;
	if (lastItem)
	{
		subItem = this->InsertItem(inPath.filename().c_str(), 1, 1, lastItem);
	}
	
	bool isSuc = true;
	boost::filesystem::directory_iterator end_itr;
	for ( boost::filesystem::directory_iterator itr(inPath); itr != end_itr; ++itr)
	{
		boost::filesystem::wpath itrPath = *itr;
		boost::filesystem::wpath subInPath(inPath.wstring() + L"\\" + itrPath.leaf().wstring());
		if (boost::filesystem::is_directory(itr->status()) )
		{
			if (!PasteDirectory(subInPath, outPath + inPath.filename().wstring() + L"/", subItem))
			{
				isSuc = false;
			}
		}
		else if (boost::filesystem::is_regular(itr->status()))
		{
			if(!PasteFile(subInPath, outPath + inPath.filename().wstring() + _T("/"), false))
			{
				isSuc = false;
			}
		}
	}
	return isSuc;
}

int CIOSDirectoryTree::GetItemByName(LPCTSTR name, int flag)
{
	int iItem = m_fileList->GetNextItem(-1,LVNI_ALL|flag);
	while (iItem != -1)
	{
		CString itemName = m_fileList->GetItemText(iItem, 0);
		if (!itemName.Compare(name))
		{
			break;
		}
		iItem = m_fileList->GetNextItem(iItem,LVNI_ALL|flag);
	}
	return iItem;
}

bool CIOSDirectoryTree::PasteToSelectedFolder(const std::wstring &src)
{
	boost::filesystem::wpath inPath(src);
	if (!boost::filesystem::exists(inPath))
	{
		return true;
	}
	int iItem = GetItemByName(inPath.filename().c_str(), LVNI_ALL);
	if(iItem != -1)
	{
		if(::MessageBox(GetSafeHwnd(), _T("文件已存在, 是否覆盖?"), _T("覆盖文件"), MB_YESNO|MB_ICONINFORMATION) == IDNO)
		{
			return true;
		}
		m_fileList->DeleteItem(iItem);
	}

	std::wstring outPath = GetPathByHTreeItem(m_fileList->GetHTreeItem());
	if (boost::filesystem::is_regular(inPath))
	{
		return PasteFile(inPath, outPath);
	}
	else if (boost::filesystem::is_directory(inPath))
	{
		return PasteDirectory(inPath, outPath, m_fileList->GetHTreeItem());
	}
	return false;
}

bool CIOSDirectoryTree::CopyTo(const std::wstring &target, const std::wstring &src, const bool transLater, const bool toTxt)
{
	FileInfo inf;
	try
	{
		inf = m_iOsDevice->GetFileInfoW(src.c_str());
	}
	catch(std::wstring &e)
	{
		m_status = e;
		return false;
	}

	return CopyTo(target, src, inf, transLater, toTxt);
}

bool CIOSDirectoryTree::CopyTo(const std::wstring &target, const std::wstring &src, const FileInfo inf, const bool transLater, const bool toTxt)
{
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));
	boost::filesystem::ofstream outFile(target, std::ios::binary|std::ios::out|std::ios::trunc);

	if (outFile.is_open())
	{
		iOsFile inFile = 0;
		try
		{
			inFile = m_iOsDevice->OpenFileW(src.c_str());
			unsigned int len = inf.fileLen;
			long readLen = 0;
			if(len > 10 * 1024 * 1024)
			{
				len = 10 * 1024 * 1024;
			}
			do
			{
				char *buf = new char[len];
				m_iOsDevice->ReadFile(inFile, buf, len);
				outFile.write(buf, len);
				delete [] buf;
				readLen += len;
			}while( readLen <  inf.fileLen);
		}
		catch(std::wstring &e)
		{
			if(inFile)
				m_iOsDevice->CloseFile(inFile);
			outFile.close();
			m_status = e;
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			return false;
		}
		if(inFile)
				m_iOsDevice->CloseFile(inFile);
		outFile.close();

		if (transLater)
		{
			boost::filesystem::wpath targetPath(target);
			if (!_wcsicmp(_T(".png"), targetPath.extension().c_str()))
			{
				static bool needTrans = true;
				if (m_askIsTransPng)
				{
					if (MessageBox(_T("该文件为PNG图片, 是否尝试转换?"), _T("是否转换图片"), MB_YESNO) != IDYES)
					{
						needTrans = false;
					}
					needTrans = true;
					m_askIsTransPng = false;
				}
				if (needTrans)
				{
					std::wstring newTarget = targetPath.parent_path().wstring() + L"\\" + targetPath.stem().wstring() + L"_translated.png";
					CPngConv pngConv;
					pngConv.Convert((char *)UTF16ToGB(target).c_str(), (char *)UTF16ToGB(newTarget).c_str());
				}	
			}
			else if (IsBinaryPlist(src,  true))
			{
				static bool needTrans = true;
				if (m_askIsTransPlist)
				{
					if (MessageBox(_T("该文件为二进制Plist, 是否尝试转换?"), _T("是否转换Plist"), MB_YESNO) != IDYES)
					{
						needTrans = false;
					}
					needTrans = true;
					m_askIsTransPlist = false;
				}
				if (needTrans)
				{
					std::wstring newTarget = targetPath.parent_path().wstring() + _T("\\") + targetPath.stem().wstring() + _T("_translated.plist");
					if (toTxt)
					{
						newTarget += L".txt";
					}
					CPngConv pngConv;
					CPlistConv plist( (m_appFolder + L"\\CoreFoundation.dll").c_str() );
					plist.BinaryToXML(target.c_str(), newTarget.c_str());
				}
			}
		}
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return true;
	}
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	return false;
}

bool CIOSDirectoryTree::CopyDicTo(const std::wstring &target, const std::wstring &src)
{
	bool isSuc = true;
	std::vector<std::wstring> names;
	try
	{
		names = m_iOsDevice->EnumDirectoryW(src.c_str()); 
	}
	catch(std::wstring &e)
	{
		m_status = e;
		return false;
	}

	for (size_t i = 0; i < names.size(); ++i)
	{
		std::wstring subPath = src + names[i].c_str();
		std::wstring _target = target + names[i].c_str();
		int type = IsFolderItem(subPath, true);
		if(type == 0)
		{
			if(!CopyTo(_target, subPath))
			{
				isSuc = false;
			}
		}
		else if(type == 1)
		{
			boost::filesystem::wpath subFolder(subPath);
			try
			{
				boost::filesystem::create_directories(subFolder);
			}
			catch(...)
			{
				return false;
			}
			if(!CopyDicTo(_target + _T("\\"), subPath + _T("/")))
			{
				isSuc = false;
			}
		}
	}
	return isSuc;
}

int CIOSDirectoryTree::IsFolderItem(const std::wstring &name, const bool isFullPath)
{
	std::wstring src;
	if(!isFullPath)
	{
		src = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + name;
	}
	else
	{
		src = name;
	}

	FileInfo inf;
	try
	{
		inf = m_iOsDevice->GetFileInfoW(src.c_str());
	}
	catch(std::wstring &e)
	{
		m_status = e;
		return 0;
	}

	if(inf.fileType == kDirectory)
	{
		return 1;
	}
	else if (inf.fileType == kRegularFile)
	{
		return 0;
	}
	return -1;
}

bool CIOSDirectoryTree::IsBinaryPlist(const std::wstring &name, const bool isFullPath)
{
	if (IsFolderItem(name, isFullPath) == 0)
	{
		//bplist00
		using namespace iOsIO;
		iOsFile inFile = 0;
		try
		{
			std::wstring src;
			if(!isFullPath)
			{
				src = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + name;
			}
			else
			{
				src = name;
			}
			inFile = m_iOsDevice->OpenFileW(src.c_str());
			char buf[8];
			unsigned int len = 8;
			m_iOsDevice->ReadFile(inFile, buf, len);
			if (!memcmp(buf, "bplist00", 8))
			{
				m_iOsDevice->CloseFile(inFile);
				return true;
			}
		}
		catch(std::wstring &e)
		{
			m_status = e;
		}
		m_iOsDevice->CloseFile(inFile);
	}
	return false;
}

bool CIOSDirectoryTree::CopyItemTo(int iItem, const std::wstring &target, const bool isFolder, const bool toTxt)
{
	if(iItem == -1)
	{
		return false;
	}

	if(isFolder)
	{
		bool isSuc = true;
		do
		{
			CString name = m_fileList->GetItemText(iItem, 0);
			if (!name.Compare(_T("..")))
			{
				iItem = m_fileList->GetNextItem(iItem,LVNI_SELECTED);
				continue;
			}


			std::wstring src = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + name.GetString();
			int type = IsFolderItem(src, true);
			if(type == 0)
			{
				std::wstring _target = target +name.GetString();
				if(!CopyTo(_target, src))
				{
					isSuc = false;
				}
			}
			else if(type == 1)
			{
				std::wstring subPath = target + name.GetString();
				boost::filesystem::wpath subFolder(subPath);
				try
				{
					boost::filesystem::create_directories(subFolder);
				}
				catch(...)
				{
					return false;
				}

				if(!CopyDicTo(subPath + _T("\\"), src + _T("/")))
				{
					isSuc = false;
				}
			}
			iItem = m_fileList->GetNextItem(iItem,LVNI_SELECTED);
		}while(iItem != -1);
		return isSuc;
	}
	else
	{
		std::wstring src = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + m_fileList->GetItemText(iItem, 0).GetString();
		return CopyTo(target, src, true, toTxt);
	}
}

// 如为目录, 加"\\或/"
bool CIOSDirectoryTree::CopySelectedItemTo(const std::wstring &target, const bool isFolder)
{
	int iItem = m_fileList->GetNextItem(-1,LVNI_SELECTED);
	return CopyItemTo(iItem, target, isFolder);
}

bool CIOSDirectoryTree::CopyNormalFileToCFile(const std::wstring &src, CFile *pFile)
{
	std::ifstream inFile(src.c_str(), std::ios::in|std::ios::binary);
	if (inFile.is_open())
	{
		inFile.seekg(0, std::ios::end);
		size_t fileLen = inFile.tellg();
		size_t len =  fileLen;
		inFile.seekg(0, std::ios::beg);
		unsigned long readLen = 0;
		if(len > 10 * 1024 * 1024)
		{
			len = 10 * 1024 * 1024;
		}
		do
		{
			HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, len);
			if(hGlob)
			{
				LPBYTE pBytes = (LPBYTE)GlobalLock(hGlob);          
				if(pBytes)
				{
					inFile.read((char *)pBytes, (std::streamsize)len);
					pFile->Write(pBytes, (UINT)len);
				}
				GlobalUnlock(hGlob);
			}
			GlobalFree(hGlob);
			readLen += (long)len;
		}while( readLen <  fileLen);
		return true;
	}
	return false;
}

bool CIOSDirectoryTree::CopyItemToCFile(int iItem, CFile *pFile)
{
	std::wstring path = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + m_fileList->GetItemText(iItem, 0).GetString();

	iOsFile inFile = 0;
	try
	{
		FileInfo inf;
		inf = m_iOsDevice->GetFileInfoW(path.c_str());
		inFile = m_iOsDevice->OpenFileW(path.c_str());
		unsigned int len = inf.fileLen;
		long readLen = 0;
		if(len > 10 * 1024 * 1024)
		{
			len = 10 * 1024 * 1024;
		}
		do
		{
			HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, len);
			if(hGlob)
			{
				LPBYTE pBytes = (LPBYTE)GlobalLock(hGlob);          
				if(pBytes)
				{
					m_iOsDevice->ReadFile(inFile, pBytes, len);
					pFile->Write(pBytes, len);
				}
				GlobalUnlock(hGlob);
			}
			GlobalFree(hGlob);
			readLen += len;
		}while( readLen <  inf.fileLen);
	}
	catch(std::wstring &e)
	{
		if(inFile)
			m_iOsDevice->CloseFile(inFile);
		m_status = e;
		return false;
	}
	if(inFile)
		m_iOsDevice->CloseFile(inFile); 

	return true;
}

std::wstring CIOSDirectoryTree::CopyItemToTemp(int iItem, const bool isFolder)
{
	std::wstring itemName = m_fileList->GetItemText(iItem, 0).GetString();
	std::wstring target = m_appFolder + _T("\\temp\\") + itemName;
	if (isFolder)
	{
		target += _T("\\");
	}

	bool isPic = false;
	bool isVideo = false;
	bool needTrans = false;
	bool isPlist = false;

	std::wstring name = m_fileList->GetItemText(iItem, 0).GetString();
	std::wstring src = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + name;
	iOsIO::FileInfo inf = m_iOsDevice->GetFileInfoW(src.c_str());
	if (inf.fileLen > 10 * 1024 * 1024)
	{
		return _T("");
	}

	boost::filesystem::wpath targetPath(target);
	if (!_wcsicmp(_T(".png"), targetPath.extension().c_str()))
	{
		needTrans = true;
		isPic = true;
	}
	else if (!_wcsicmp(_T(".plist"), targetPath.extension().c_str()))
	{
		if (IsBinaryPlist(name))
		{
			isPlist = true;
			needTrans = true;
		}
		else
		{
			target += _T(".txt");
		}
	}
	else if ( !(_wcsicmp(_T(".jpg"), targetPath.extension().c_str()) &&  _wcsicmp(_T(".jpeg"), targetPath.extension().c_str()) && _wcsicmp(_T(".bmp"), targetPath.extension().c_str()) && 
		_wcsicmp(_T(".gif"), targetPath.extension().c_str()) &&  _wcsicmp(_T(".wmf"), targetPath.extension().c_str())) )
	{
		isPic = true;
	}
	else if ( !( _wcsicmp(_T(".mp3"), targetPath.extension().c_str()) && _wcsicmp(_T(".mov"), targetPath.extension().c_str()) && 
		_wcsicmp(_T(".wav"), targetPath.extension().c_str()) && _wcsicmp(_T(".ape"), targetPath.extension().c_str())  && 
		_wcsicmp(_T(".wma"), targetPath.extension().c_str()) && 
		_wcsicmp(_T(".midi"), targetPath.extension().c_str()) && _wcsicmp(_T(".m4r"), targetPath.extension().c_str()) ) )
	{
		isVideo = true;
	}
	else
	{
		target += _T(".txt");
	}

	CopyItemTo(iItem, target, isFolder, needTrans && isPlist);

	if (needTrans)
	{
		target = targetPath.parent_path().wstring() + _T("\\") + targetPath.stem().wstring() + _T("_translated") + targetPath.extension().wstring();
		if (isPlist)
		{
			target += _T(".txt");
		}
	}
	if (isPic || isVideo)
	{
		std::wstring tempFilePath = targetPath.parent_path().wstring() + _T("\\temp.html");
		std::ofstream tempFile(tempFilePath.c_str(), std::ios::trunc | std::ios::binary);
		if (tempFile.is_open())
		{
			tempFile.write("\xFF\xFE", 2);
			
			std::wstring content = L"<html><head><title>temp</title></head><body><center>";
			if (isPic)
			{
				content += L"<img src=\"";
			}
			else
			{
				content += L"<embed src=\"";
			}
			content += L"file:///";
			content += target;
			if (isPic)
			{
				content += L"\" />";
			}
			else
			{
				content += L"\"></embed>";
			}
			content += L"</center></body></html>";
			tempFile.write((char *)content.c_str(), (std::streamsize)content.size() * 2);
			tempFile.close();
			return tempFilePath.c_str();
		}
	}
	return target;
}

void CIOSDirectoryTree::OpenSelectedFileItem(const bool openWith)
{
	int iItem = m_fileList->GetNextItem(-1,LVNI_SELECTED);
	while (iItem != -1)
	{
		OpenFileItem(iItem, openWith);
		iItem = m_fileList->GetNextItem(iItem,LVNI_SELECTED);
	}
}

std::string CIOSDirectoryTree::UTF16ToGB(const std::wstring &utf16Str)
{
	int u8Len = ::WideCharToMultiByte(CP_ACP, NULL, utf16Str.c_str(), (int)utf16Str.size(), NULL, 0, NULL, NULL);
	char* strBuf = new char[u8Len + 1];
	::WideCharToMultiByte(CP_ACP, NULL, utf16Str.c_str(), (int)utf16Str.size(), strBuf, u8Len, NULL, NULL);
	strBuf[u8Len] = '\0';
	std::string str = strBuf;
	delete [] strBuf;
	return str;
}

void CIOSDirectoryTree::OpenFileItem(int iItem, const bool openWith)
{
	if(iItem == -1)
	{
		return ;
	}

	// 目录
	CString itemName = m_fileList->GetItemText(iItem, 0);
	if(!itemName.Compare(_T("..")))
	{
		HTREEITEM parentItem = GetNextItem(m_fileList->GetHTreeItem(), TVGN_PARENT);
		SelectItem(parentItem);
		SetFocus();
		// AddToVisitedPath(GetPathByHTreeItem(parentItem));
		return ;
	}

	std::wstring path = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + itemName.GetString();
	FileInfo inf;
	try
	{
		inf = m_iOsDevice->GetFileInfoW(path.c_str());
	}
	catch(std::wstring &e)
	{
		m_status = e;
		return ;
	}

	if(inf.fileType == kDirectory || (inf.fileType == kSymbolLink && IsLnkToDirectory(path.c_str())))
	{
		Expand(m_fileList->GetHTreeItem(), TVE_EXPAND);
		HTREEITEM hChildItem = GetChildItem(m_fileList->GetHTreeItem());
		while (hChildItem != NULL)
		{ 
			CString childName = GetItemText(hChildItem);
			if(!itemName.Compare(childName))
			{
				break;
			}

			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}

		if(hChildItem)
		{
			Expand(hChildItem, TVE_EXPAND);
			SelectItem(hChildItem);
			// AddToVisitedPath(path);
		}
	}
	// 文件
	else if (inf.fileType == kRegularFile)
	{
		std::wstring target = m_appFolder + _T("\\temp\\") + itemName.GetString();
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));
		bool status = CopyTo(target, path, inf, false);
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		if (status)
		{
			boost::filesystem::wpath targetPath(target);
			if (!_wcsicmp(_T(".png"), targetPath.extension().c_str()))
			{
				if (MessageBox(_T("该文件为PNG图片, 是否尝试转换?"), _T("是否转换图片"), MB_YESNO) == IDYES)
				{
					std::wstring newTarget = targetPath.parent_path().wstring() + _T("\\") + targetPath.stem().wstring() + _T("_translated.png");
					CPngConv pngConv;
					int ret = pngConv.Convert((char *)UTF16ToGB(target).c_str(), (char *)UTF16ToGB(newTarget).c_str());
					if (!ret)
					{
						target = newTarget.c_str();
					}
				}
			}
			else if (IsBinaryPlist(path,  true))
			{
				if (MessageBox(_T("该文件为二进制Plist, 是否尝试转换?"), _T("是否转换Plist"), MB_YESNO) == IDYES)
				{
					std::wstring newTarget = targetPath.parent_path().wstring() + _T("\\") + targetPath.stem().wstring() + _T("_translated.plist");
					CPlistConv plist( (m_appFolder + L"\\CoreFoundation.dll").c_str());
					bool ret = plist.BinaryToXML(target.c_str(), newTarget.c_str());
					if (ret)
					{
						target = newTarget.c_str();
					}
				}
				
			}

			if(openWith)
			{
				std::wstring openWithCmd = _T("shell32.dll,OpenAs_RunDLL \"") + target + _T("\"");
				ShellExecute(NULL, NULL, _T("rundll32.exe"), openWithCmd.c_str(), NULL, SW_SHOW);   
			}
			else if ((int)ShellExecute(NULL,_T("open"), target.c_str(),NULL, NULL, SW_SHOW) == SE_ERR_NOASSOC)
			{
				std::wstring openWithCmd = _T("shell32.dll,OpenAs_RunDLL \"") + target + _T("\"");
				ShellExecute(NULL, NULL, _T("rundll32.exe"), openWithCmd.c_str(), NULL, SW_SHOW);   
			}
		}
	}
}

bool CIOSDirectoryTree::RemoveItem(const std::wstring &src)
{
	if (src == L"/")
	{
		return false;
	}

	bool isSuc = true;
	FileInfo inf;
	try
	{
		inf = m_iOsDevice->GetFileInfoW(src.c_str());
	}
	catch(std::wstring &e)
	{
		m_status = e;
		return false;
	}

	if (inf.fileType == kDirectory)
	{
		std::vector<std::wstring> subNames = m_iOsDevice->EnumDirectoryW(src.c_str());
		for (size_t i = 0; i < subNames.size(); ++i)
		{
			isSuc = RemoveItem((src + (_T("/") + subNames[i]).c_str()));
		}
	}

	try
	{
		m_iOsDevice->RemovePathW(src.c_str());
	}
	catch(std::wstring &e)
	{
		m_status = e;
		isSuc = false;
	}
	return isSuc;
}

bool CIOSDirectoryTree::RemoveSelectedFile()
{
	POSITION  pos = m_fileList->GetFirstSelectedItemPosition();
	int iItem = m_fileList->GetNextSelectedItem(pos);
	bool isSuc = true;
	if(pos != NULL || iItem != -1)
	{
		do
		{
			CString name = m_fileList->GetItemText(iItem, 0);
			if (!name.Compare(_T("..")) || !name.Compare(_T("")))
			{
				iItem = m_fileList->GetNextSelectedItem(pos);
				continue;
			}
			std::wstring src = GetPathByHTreeItem(m_fileList->GetHTreeItem()) + name.GetString();
			isSuc = RemoveItem(src);
			if (isSuc)
			{
				m_fileList->DeleteItem(iItem);

				HTREEITEM hChildItem = GetChildItem(m_fileList->GetHTreeItem());
				while (hChildItem != NULL)
				{ 
					CString childName = GetItemText(hChildItem);
					if(!name.Compare(childName))
					{
						DeleteItem(hChildItem);
						break;
					}
					hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
				}
			}
			iItem =  m_fileList->GetNextSelectedItem(pos);
		}while(iItem != -1 || pos != NULL);
	}
	else
	{
		isSuc = false;
	}
	return isSuc;
}

bool CIOSDirectoryTree::RenameSelectedFile(const std::wstring &toName)
{
	POSITION pos = m_fileList->GetFirstSelectedItemPosition();
	int iItem = m_fileList->GetNextSelectedItem(pos);
	bool isSuc = true;
	if(iItem != -1)
	{
		CString name = m_fileList->GetItemText(iItem, 0);
		std::wstring folderPath = GetPathByHTreeItem(m_fileList->GetHTreeItem());
		try
		{
			m_iOsDevice->RenamePathW((folderPath + name.GetString()).c_str(), (folderPath + toName).c_str());
			m_fileList->SetItemText(iItem, 0, toName.c_str());

			HTREEITEM hChildItem = GetChildItem(m_fileList->GetHTreeItem());
			while (hChildItem != NULL)
			{ 
				CString childName = GetItemText(hChildItem);
				if(!name.Compare(childName))
				{
					SetItemText(hChildItem, toName.c_str());
					break;
				}
				hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
		catch(std::wstring &e)
		{
			m_status = e;
			isSuc = false;
		}
	}
	else
	{
		isSuc = false;
	}
	return isSuc;
}


bool CIOSDirectoryTree::IsLnkToDirectory(const wchar_t *path)
{
	return m_iOsDevice->IsLnkToDirectoryW(path);
}

bool CIOSDirectoryTree::CacheFolder(const std::wstring &folderPath, HTREEITEM folderHTreeItem, std::wstring &error)
{
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));
	m_cachedItem[folderHTreeItem] = true;

	using namespace iOsIO;
	std::vector<std::wstring> names;
	try
	{
		if(!m_iOsDevice)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			return false;
		}
		names = m_iOsDevice->EnumDirectoryW(folderPath.c_str());
	}
	catch (std::wstring &e)
	{
		error = e;
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return false;
	}

	std::wstring childText;
	HTREEITEM childItem = GetChildItem(folderHTreeItem);
	if(childItem)
	{
		DeleteItem(childItem);
	}

	for(size_t i = 0; i < names.size(); ++i)
	{
		std::wstring path = folderPath + names[i];
		FileInfo inf;
		try
		{
			if(!m_iOsDevice)
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
				return false;
			}
			inf = m_iOsDevice->GetFileInfoW(path.c_str());
		}
		catch (std::wstring &e)
		{
			error = e;
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			return false;
		}

		if (inf.fileType == kDirectory || inf.fileType == kSymbolLink)
		{
			HTREEITEM hTreeItemSubRoot = NULL;
			if (inf.fileType == kDirectory)
			{
				hTreeItemSubRoot = InsertItem(names[i].c_str(), 1, 1, folderHTreeItem);
			}
			else if(inf.fileType == kSymbolLink)
			{
				if(IsLnkToDirectory(path.c_str()))
				{
					hTreeItemSubRoot = InsertItem(names[i].c_str(), 3, 3, folderHTreeItem);
				}
			}
			if(hTreeItemSubRoot)
				InsertItem(_T(".."), 1, 1, hTreeItemSubRoot);
		}
	}
	error = _T("展开该目录成功: ") + folderPath;
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	return true;
}

bool CIOSDirectoryTree::Init(CIOSFileList *list, CEdit *path, CProgressCtrl *progress)
{
	m_fileList = list;
	m_progress = progress;
	m_fileList->Init(this);

	m_path = path;

	SetBkColor(RGB(255,255,255));

	m_imageList.Create(16,16,ILC_COLOR8 | ILC_MASK,2,2);
	m_imageList.SetBkColor(RGB(255,255,255)); 

	CBitmap bitmap[5];
	bitmap[0].LoadBitmap(IDB_IOS);
	m_imageList.Add(&bitmap[0], RGB(255,255,255));

	bitmap[1].LoadBitmap(IDB_FOLDER);
	m_imageList.Add(&bitmap[1], RGB(255,255,255));

	bitmap[2].LoadBitmap(IDB_OPENED_FOLDER);
	m_imageList.Add(&bitmap[2], RGB(255,255,255)); 

	bitmap[3].LoadBitmap(IDB_SYMBOL_LINK);
	m_imageList.Add(&bitmap[3], RGB(255,255,255));

	bitmap[4].LoadBitmap(IDB_OPENED_SYMBOL_LINK);
	m_imageList.Add(&bitmap[4], RGB(255,255,255));
	
	SetImageList(&m_imageList, TVSIL_NORMAL);

	m_expandThread = new boost::thread(boost::bind(&CIOSDirectoryTree::ExpandRoot, this));
	return true;
}

void CIOSDirectoryTree::ExpandRoot()
{
	while (true)
	{
		if(!m_hWnd)
		{
			continue;
		}
		if(!IsWindowVisible() && !IsWindow(m_hWnd))
		{
			continue;
		}

		if (m_iOsDevice)
		{
			if (!m_iOsDevice->IsConnected())
			{
				EnableWindow(FALSE);
				m_fileList->EnableWindow(FALSE);
				using namespace iOsIO;
				try
				{
					unsigned short timeOut = 0;
					if(!m_iOsDevice)
					{
						std::wstring ituneDllPath = m_appFolder + _T("\\iTunesMobileDevice.dll");
						m_iOsDevice = new CiOsIO(ituneDllPath.c_str(), timeOut);
					}
					m_status = _T("正在尝试连接到iOs设备");
					m_iOsDevice->Connect();
					m_status = _T("正在尝试开启AFC文件服务");
					m_iOsDevice->StartAFCService();
				}
				catch (std::wstring &e)
				{
					m_status = e;
					continue;
				}

				m_cachedItem.clear();
				DeleteAllItems();
				m_hTreeItemRoot = InsertItem(_T("iOs"), 0, 0);
				m_status = _T("正在展开根目录");
				CacheFolder(_T("/"), m_hTreeItemRoot, m_status);
				Expand(m_hTreeItemRoot, TVE_EXPAND);
				for (unsigned short i = 0; i < 7; ++i)
				{
					m_visitedPaths[i] == L"";
				}
				// AddToVisitedPath(_T("/"));
				EnumHTreeItem(m_hTreeItemRoot);
				EnableWindow(TRUE);
				m_fileList->EnableWindow(TRUE);
			}
			else
			{
				Sleep(1000);
			}
		}
		else
		{
			Sleep(1000);
		}
	}
}

std::wstring CIOSDirectoryTree::GetPathByHTreeItem(HTREEITEM hTreeItem)
{
	CString path;
	HTREEITEM hRootItem =hTreeItem;
	while (m_hTreeItemRoot != hRootItem && hTreeItem)
	{
		path = GetItemText(hRootItem) + _T("/") + path;
		hRootItem = GetParentItem(hRootItem);
	}
	path = _T("/") + path;
	return path.GetString();
}

BEGIN_MESSAGE_MAP(CIOSDirectoryTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &CIOSDirectoryTree::OnTvnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CIOSDirectoryTree::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CIOSDirectoryTree::OnTvnItemexpanded)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CIOSDirectoryTree::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_iOsDevice)
	{
		return ;
	}

	if (!m_iOsDevice->IsConnected())
	{
		return ;
	}

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if( m_hTreeItemRoot != hItem)
	{
		if(!m_cachedItem.count(hItem))
		{
			std::wstring path = GetPathByHTreeItem(hItem);
			CacheFolder(path, hItem, m_status);
		}
	}

	*pResult = 0;
}

void CIOSDirectoryTree::InsertFileItem(const std::wstring &subPath, const std::wstring &name, const size_t index)
{
	FileInfo inf;
	try
	{
		inf = m_iOsDevice->GetFileInfoW(subPath.c_str());
	}
	catch (std::wstring &e)
	{
		m_status = e;
		return ;
	}

	std::wstring fileTypeStr;
	int imageIndex = -1;
	switch (inf.fileType)
	{
	case kDirectory:
		{
			fileTypeStr = _T("目录");
			imageIndex = 0;
		}
		break;
	case kSymbolLink:
		{
			fileTypeStr = _T("符号链接");
			if(IsLnkToDirectory(subPath.c_str()))
			{
				imageIndex = 1;
			}
			else
			{
				imageIndex = -2;
			}
		}
		break;
	case kRegularFile:
		{
			fileTypeStr = _T("正常文件");
		}
		break;
	case kBlockLink:
		{
			fileTypeStr = _T("组设备");
		}
		break;
	case kCHR:
		{
			fileTypeStr = _T("字符定义表");
		}
		break;
	case kFIFO:
		{
			fileTypeStr = _T("FIFO命名管道");
		}
		break;
	case kMT:
		{
			fileTypeStr = _T("文件类型掩码");
		}
		break;
	case kSock:
		{
			fileTypeStr = _T("Socket");
		}
		break;
	case kUnkownFileType:
		{
			fileTypeStr = _T("未知");
		}
	default: 
		{
		}
		break;
	}

	SItemData *itemData = new SItemData;

	int nRow = m_fileList->InsertItem((int)(index + 1), name.c_str(), imageIndex);
	itemData->Insert(name.c_str());

	m_fileList->SetItemText(nRow, 1, fileTypeStr.c_str());
	itemData->Insert(fileTypeStr.c_str());

	std::wstring fileLenStr;
	if(inf.fileLen < 1024)
	{
		fileLenStr = boost::lexical_cast<std::wstring>(inf.fileLen);
	}
	else
	{
		fileLenStr = boost::lexical_cast<std::wstring>(inf.fileLen/1024) + _T("KB");
	}
	m_fileList->SetItemText(nRow, 2, fileLenStr.c_str());
	itemData->Insert(inf.fileLen);

	m_fileList->SetItemText(nRow, 3, boost::lexical_cast<std::wstring>(inf.blockSize).c_str());
	itemData->Insert(inf.blockSize);

	m_fileList->SetItemText(nRow, 4, boost::lexical_cast<std::wstring>(inf.linkCount).c_str());
	itemData->Insert(inf.linkCount);

	m_fileList->SetSItemData(nRow, itemData);
}

void CIOSDirectoryTree::EnumHTreeItem(HTREEITEM hItem)
{
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));
	std::wstring path = GetPathByHTreeItem(hItem);

	if (!m_isWarding)
	{
		AddToVisitedPath(path.c_str());
	}
	else
	{
		m_isWarding = false;
	}

	m_path->SetWindowText(path.c_str());

	std::vector<std::wstring> files;
	try
	{
		m_status = _T("正在枚举目录: ") + path;
		if(!m_iOsDevice)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			return ;
		}
		files = m_iOsDevice->EnumDirectoryW(path.c_str());
	}
	catch (std::wstring &e)
	{
		m_status = e;
	}
	
	m_fileList->DeleteAllItems();
	m_fileList->SetHTreeItem(hItem);
	m_fileList->InsertItem(0, _T(".."), 0);
	for (size_t i = 0; i < files.size(); ++i)
	{
		using namespace iOsIO;
		std::wstring subPath = path;
		subPath += files[i];
		InsertFileItem(subPath, files[i], i);
	}
	m_status = _T("枚举目录成功: ") + path;
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
}

void CIOSDirectoryTree::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_iOsDevice)
	{
		return ;
	}

	if (!m_iOsDevice->IsConnected())
	{
		return ;
	}

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	EnumHTreeItem(hItem);
	
	*pResult = 0;
}

std::wstring CIOSDirectoryTree::GetLastStatus()
{
	if (!m_iOsDevice || !m_iOsDevice->IsConnected())
	{
		std::wstring rtn = _T("未连接或意外断开");
		if (!m_status.empty())
		{
			rtn += _T(", 最后一次状态信息:") + m_status;
		}
		return rtn;
	}
	return m_status;
}
void CIOSDirectoryTree::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if(hItem == m_hTreeItemRoot)
	{
		return ;
	}
	int nImage, nSelectedImage;
	GetItemImage(hItem, nImage, nSelectedImage);

	if(pNMTreeView->action == 1)
	{
		if(nImage == 2)
		{
			SetItemImage(hItem, 1, nSelectedImage);
		}
		else if(nImage == 4)
		{
			SetItemImage(hItem, 3, nSelectedImage);
		}
	}
	else
	{
		if(nImage == 1)
		{
			SetItemImage(hItem, 2, nSelectedImage);
		}
		else if(nImage == 3)
		{
			SetItemImage(hItem, 4, nSelectedImage);
		}
	}

	*pResult = 0;
}

std::wstring CIOSDirectoryTree::GetSelectedFileItemPath()
{
	int iItem = m_fileList->GetNextItem(-1,LVNI_SELECTED);
	if(iItem == -1)
	{
		return false;
	}
	return GetPathByHTreeItem(m_fileList->GetHTreeItem()) + m_fileList->GetItemText(iItem, 0).GetString();
}

HTREEITEM CIOSDirectoryTree::GetHTreeItemByName(LPCTSTR name, HTREEITEM parentHTreeItem)
{
	HTREEITEM hChildItem = GetChildItem(parentHTreeItem);
	while (hChildItem != NULL)
	{ 
		CString childName = GetItemText(hChildItem);
		if(!childName.Compare(name))
		{
			break;
		}

		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
	return hChildItem;
}

void CIOSDirectoryTree::AddToVisitedPath(const wchar_t* path)
{
	if (m_visitedPos > 6)
	{
		if (m_visitedPaths[6] == path)
		{
			return ;
		}
		for (unsigned short i = 0; i < 6; ++i)
		{
			m_visitedPaths[i] = m_visitedPaths[i + 1];
		}
		m_visitedPaths[6] = path;
		m_visitedPos = 7;
	}
	else
	{
		if (m_visitedPos - 1 >= 0)
		{
			if (m_visitedPaths[m_visitedPos - 1] == path)
			{
				return ;
			}
		}
		m_visitedPaths[m_visitedPos++] = path;
		for (unsigned short i = m_visitedPos; i <  7; ++i)
		{
			m_visitedPaths[i] = L"";
		}
	}
}

void CIOSDirectoryTree::Goto( const wchar_t* path, bool addToVisited )
{
	if (!_wcsicmp(_T("/"), path))
	{
		Expand(m_hTreeItemRoot, TVE_EXPAND);
		Select(m_hTreeItemRoot, TVGN_FIRSTVISIBLE);
	}

	HTREEITEM iItem = m_hTreeItemRoot;
	bool needCollase = true;
	boost::filesystem::wpath sPath(path);
	std::wstring pathStr = _T("/");

	boost::filesystem::wpath::iterator itr;
	for (itr = sPath.begin(); itr != sPath.end(); ++itr)
	{
		if (itr != sPath.begin())
		{
			boost::filesystem::wpath ePath = *itr;
			std::wstring name = ePath.filename().wstring();
			if (!_wcsicmp(_T("."), name.c_str()))
			{
				break;
			}
			iItem = GetHTreeItemByName(name.c_str(), iItem);
			if (!iItem)
			{
				break;
			}

			pathStr += ePath.wstring() + _T("/"); 
			if(!m_cachedItem.count(iItem))
			{
				if (CacheFolder(pathStr.c_str(), iItem, m_status))
				{
					Expand(iItem, TVE_EXPAND);
				}
			}
			else
			{
				Expand(iItem, TVE_EXPAND);
			}
		}	
	}
	if (iItem)
	{
		Select(iItem, TVGN_FIRSTVISIBLE);
		SelectItem(iItem);
		if (addToVisited)
		{
			// AddToVisitedPath(path);
		}
	}
}

void CIOSDirectoryTree::OnDestroy()
{
	if (m_expandThread)
	{
		m_expandThread->interrupt();
		delete m_expandThread;
	}
	
	CTreeCtrl::OnDestroy();
}

void CIOSDirectoryTree::BackWard()
{
	if ( m_visitedPos - 2 >= 0 && m_visitedPos - 2 <= 6)
	{
		if (m_visitedPaths[m_visitedPos - 2] != L"")
		{
			m_isWarding = true;
			Goto(m_visitedPaths[m_visitedPos - 2], false);
			--m_visitedPos;
		}
	}
}

void CIOSDirectoryTree::ForWard()
{
	if ( m_visitedPos >= 0 && m_visitedPos <= 6)
	{
		if (m_visitedPaths[m_visitedPos] != L"")
		{
			m_isWarding = true;
			Goto(m_visitedPaths[m_visitedPos], false);
			++m_visitedPos;
		}
	}
}
#pragma warning(pop)