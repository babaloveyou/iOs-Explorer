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
#include "ios_io.h"

#include <exception>
#include <string>

#include "utility/codepage.h"

namespace iOsIO
{
	am_device* CiOsIO::m_device = NULL;
	bool CiOsIO::m_isConnected = false;

	CiOsIO::NotifySubFunc			CiOsIO::sNotifySubFunc;
	CiOsIO::NotifyUnSubFunc			CiOsIO::sNotifyUnSubFunc;

	CiOsIO::ConnectFunc			    CiOsIO::sConnectFunc;
	CiOsIO::IsPairedFunc			    CiOsIO::sIsDevicePaireFunc;
	CiOsIO::ValidatePairingFunc	    CiOsIO::sValidatePairingFunc;
	CiOsIO::PairingFunc			    CiOsIO::sPairFunc;
	CiOsIO::UnpairingFunc			CiOsIO::sUnPairFunc;

	CiOsIO::StartSessionFunc		    CiOsIO::sStartSessionFunc;
	CiOsIO::StartServiceFunc		    CiOsIO::sStartServiceFunc;
	CiOsIO::StopSessionFunc		    CiOsIO::sStopSessionFunc;
  
	CiOsIO::OpenConnectionFunc		CiOsIO::sAFCConnectFunc;
	CiOsIO::OpenDeviceInfoFunc		CiOsIO::sAFCOpenDeviceFunc;

	CiOsIO::OpenDirectoryFunc		CiOsIO::sAFCOpenDirectoryFunc;
	CiOsIO::ReadDirectoryFunc		CiOsIO::sAFCReadDirectoryFunc;
	CiOsIO::CloseDirectoryFunc		CiOsIO::sAFCCloseDirectoryFunc;

	CiOsIO::CreateDirectoryFunc   	CiOsIO::sAFCCreateDirectoryFunc;

	CiOsIO::OpenFileInfoFunc		    CiOsIO::sAFCOpenFileInfFunc;
	CiOsIO::ReadKeyValueFunc		    CiOsIO::sAFCReadFileInfFunc;
	CiOsIO::CloseKeyValueFunc		CiOsIO::sAFCCloseFileInfFunc;

	CiOsIO::OpenFileRefFunc		    CiOsIO::sAFCOpenFileFunc;
	CiOsIO::ReadFileRefFunc		    CiOsIO::sAFCReadFileFunc;
	CiOsIO::WriteFileRefFunc		    CiOsIO::sAFCWriteFileFunc;
	CiOsIO::SeekFileRefFunc		    CiOsIO::sAFCSeekFileFunc;
	CiOsIO::CloseFileRefFunc		    CiOsIO::sAFCCloseFileFunc;

	CiOsIO::RemovePathFunc			CiOsIO::sAFCRemovePathFunc;
	CiOsIO::RenamePathFunc			CiOsIO::sAFCRenamePathFunc;

	CiOsIO::CloseConnectionFunc	    CiOsIO::sAFCDisconnectFunc;

	CiOsIO::DisConnectFunc			CiOsIO::sDisconnectFunc;
	CiOsIO::ReleaseDeviceFunc		CiOsIO::sReleaseDeviceFunc;

	void NotifyCallBack(am_device_notification_callback_info *deviceInfo)
	{
		if (deviceInfo->msg == 1)
		{
			CiOsIO::m_isConnected = true;
		}
		else
		{
			CiOsIO::m_isConnected = false;
		}

		CiOsIO::m_device = deviceInfo->dev;
		return ;
	}

	CiOsIO::CiOsIO(const unsigned short waitConnectDelay): m_afcConnection(NULL), m_devicenotify(NULL), m_afcConnected(false)
	{
		m_waitConnectDelay = waitConnectDelay;
		CiOsIO::m_device = NULL;
		try
		{
			Init(L"iTunesMobileDevice.dll");
		}
		catch(const std::wstring &e)
		{
			throw e;
		}
	}

	CiOsIO::CiOsIO(const wchar_t* iTuneDllPath, const unsigned short waitConnectDelay): m_afcConnection(NULL), m_devicenotify(NULL), m_afcConnected(false)
	{
		m_waitConnectDelay = waitConnectDelay;
		try
		{
			Init(iTuneDllPath);
		}
		catch(const std::wstring &e)
		{
			throw e;
		}
	}

	CiOsIO::~CiOsIO(void)
	{
		if(m_afcConnected)
		{
			sAFCDisconnectFunc(m_afcConnection);
		}
		sDisconnectFunc(m_device);
		sReleaseDeviceFunc(m_device);
		int result = sNotifyUnSubFunc(m_devicenotify);
		::FreeLibrary(m_itunesDllHmodule);
	}

	void CiOsIO::Init(const wchar_t* iTuneDllPath)
	{
		m_itunesDllHmodule = ::LoadLibrary(iTuneDllPath);
		//if (!m_itunesDllHmodule)
		{
			HKEY hSetting = NULL;
			if (::RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Apple Inc.\\Apple Mobile Device Support\\Shared", &hSetting) == ERROR_SUCCESS)
			{
				DWORD length = 0;
				if (::RegQueryValueEx(hSetting, L"iTunesMobileDeviceDLL", NULL, NULL, NULL, &length) == ERROR_SUCCESS)
				{
					wchar_t *iTuneDllPathTemp = new wchar_t[length + 1];
					::RegQueryValueEx(hSetting, L"iTunesMobileDeviceDLL", NULL, NULL, (LPBYTE)iTuneDllPathTemp, &length);
					m_itunesDllHmodule = ::LoadLibrary(iTuneDllPathTemp);
				}
			}
			if (!m_itunesDllHmodule)
			{
				throw std::wstring(L"加载iTunesDevice.dll失败, 请检查你的路径");
			}
		}

		sNotifySubFunc = (NotifySubFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceNotificationSubscribe");
		sNotifyUnSubFunc = (NotifyUnSubFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceNotificationUnsubscribe");

		sConnectFunc = (ConnectFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceConnect");
		sIsDevicePaireFunc =  (IsPairedFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceIsPaired");
		sValidatePairingFunc = (ValidatePairingFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceValidatePairing");
		sPairFunc = (PairingFunc)GetProcAddress(m_itunesDllHmodule, "AMDevicePair");
		sUnPairFunc = (UnpairingFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceUnpair");

		sStartSessionFunc = (StartSessionFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceStartSession");
		sStartServiceFunc = (StartServiceFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceStartService");
		sStopSessionFunc = (StopSessionFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceStopSession");

		sAFCConnectFunc = (OpenConnectionFunc)GetProcAddress(m_itunesDllHmodule, "AFCConnectionOpen");
		sAFCOpenDeviceFunc = (OpenDeviceInfoFunc)GetProcAddress(m_itunesDllHmodule, "AFCDeviceInfoOpen");

		sAFCOpenDirectoryFunc = (OpenDirectoryFunc)GetProcAddress(m_itunesDllHmodule, "AFCDirectoryOpen");
		sAFCReadDirectoryFunc = (ReadDirectoryFunc)GetProcAddress(m_itunesDllHmodule, "AFCDirectoryRead");
		sAFCWriteFileFunc = (WriteFileRefFunc)GetProcAddress(m_itunesDllHmodule, "AFCFileRefWrite");
		sAFCCloseDirectoryFunc = (CloseDirectoryFunc)GetProcAddress(m_itunesDllHmodule, "AFCDirectoryClose"); 
		sAFCCreateDirectoryFunc = (CreateDirectoryFunc)GetProcAddress(m_itunesDllHmodule, "AFCDirectoryCreate"); 

		sAFCOpenFileInfFunc = (OpenFileInfoFunc)GetProcAddress(m_itunesDllHmodule, "AFCFileInfoOpen");
		sAFCReadFileInfFunc = (ReadKeyValueFunc)GetProcAddress(m_itunesDllHmodule, "AFCKeyValueRead");
		sAFCCloseFileInfFunc = (CloseKeyValueFunc)GetProcAddress(m_itunesDllHmodule, "AFCKeyValueClose");

		sAFCOpenFileFunc = (OpenFileRefFunc)GetProcAddress(m_itunesDllHmodule, "AFCFileRefOpen");
		sAFCReadFileFunc = (ReadFileRefFunc)GetProcAddress(m_itunesDllHmodule, "AFCFileRefRead");
		sAFCSeekFileFunc = (SeekFileRefFunc)GetProcAddress(m_itunesDllHmodule, "AFCFileRefSeek");
		sAFCCloseFileFunc = (CloseFileRefFunc)GetProcAddress(m_itunesDllHmodule, "AFCFileRefClose");
		sAFCRemovePathFunc = (RemovePathFunc)GetProcAddress(m_itunesDllHmodule, "AFCRemovePath");
		sAFCRenamePathFunc = (RenamePathFunc)GetProcAddress(m_itunesDllHmodule, "AFCRenamePath");

		sAFCDisconnectFunc = (CloseConnectionFunc)GetProcAddress(m_itunesDllHmodule, "AFCConnectionClose");

		sDisconnectFunc = (DisConnectFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceDisconnect");
		sReleaseDeviceFunc = (ReleaseDeviceFunc)GetProcAddress(m_itunesDllHmodule, "AMDeviceRelease");

		if( !sNotifyUnSubFunc || !sNotifySubFunc || 
			!sConnectFunc || !sIsDevicePaireFunc || !sValidatePairingFunc || !sPairFunc  || !sUnPairFunc || 
			!sStartSessionFunc || !sStartServiceFunc || !sStopSessionFunc || 
			!sAFCConnectFunc || !sAFCOpenDeviceFunc || 
			!sAFCOpenDirectoryFunc || !sAFCReadDirectoryFunc || !sAFCCloseDirectoryFunc || !sAFCCreateDirectoryFunc ||
			!sAFCOpenFileInfFunc || !sAFCReadFileInfFunc || !sAFCCloseFileInfFunc || 
			!sAFCOpenFileFunc || !sAFCReadFileFunc || !sAFCCloseFileFunc || !sAFCWriteFileFunc || !sAFCSeekFileFunc || 
			!sAFCRemovePathFunc || !sAFCRenamePathFunc ||
			!sAFCDisconnectFunc || !sDisconnectFunc || !sReleaseDeviceFunc)
		{
			throw std::wstring(L"加载DLL中的函数失败");
		}
   
		unsigned int result = sNotifySubFunc(NotifyCallBack, 0, 0, 0, &m_devicenotify);
		if(result)
		{
			throw std::wstring(L"注册回调函数失败");
		}
	}

	void CiOsIO::Connect()
	{
		int i = 0;
		while (!m_isConnected)
		{
			Sleep(100);
			i++;
			if(m_waitConnectDelay)
			{
				if(i > m_waitConnectDelay * 10)
				{
					throw std::wstring(L"连接超时");
				}
			}
		}

		unsigned int result = sConnectFunc(m_device);
		if(result)
		{
			throw std::wstring(L"连接失败");
		}

		int resultInt = sIsDevicePaireFunc(m_device);
		if(resultInt =! 1)
		{
			result = sPairFunc(m_device);
			if (result)
				throw std::wstring(L"检查配对失败");
		}

		result = sValidatePairingFunc(m_device);
		if(result)
		{
			result = sPairFunc(m_device);
			if (result)
				throw std::wstring(L"配对失败");
		}
	}

	unsigned char* CiOsIO::GenerateCFString(const char *normalStr)
	{
		size_t strLen = (unsigned char)strlen(normalStr);
		unsigned char* pCFString = new unsigned char[strLen + 10];
		pCFString[4] = 0x8c;
		pCFString[5] = 07;
		pCFString[6] = 01;
		pCFString[8] = (unsigned char)strLen;
		memcpy(pCFString + 9, normalStr, strLen);;
		return pCFString;
	}

	void CiOsIO::StartAFCService()
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		unsigned int result = sStartSessionFunc(m_device);
		if (result)
		{
			throw std::wstring(L"开始会话失败");
		}

		unsigned char *pCFString = this->GenerateCFString("com.apple.afc2");
		afc_connection *afcConnection = NULL;
		unsigned int unkown = 0;
		result = sStartServiceFunc(m_device, pCFString, &afcConnection, &unkown);
		delete [] pCFString;
		if (result)
		{
			// 未越狱
			pCFString = this->GenerateCFString("com.apple.afc");
			result = sStartServiceFunc(m_device, pCFString, &afcConnection, &unkown);
			delete  [] pCFString;
			if (result)
				throw std::wstring(L"开始AFC文件服务失败");
		}

		result = sStopSessionFunc(m_device);
		if (result)
		{
			throw std::wstring(L"关闭会话失败");
		}

		result = sAFCConnectFunc(afcConnection, 0, &m_afcConnection);
		if (result)
		{
			throw std::wstring(L"连接AFC文件系统服务失败");
		}
		m_afcConnected = true;
	}

	void CiOsIO::StopAFCService()
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			return ;
		}

		unsigned int result = sAFCDisconnectFunc(m_afcConnection);
		if (result)
		{
			throw std::wstring(L"断开AFC文件系统服务失败");
		}
		m_afcConnected = false;
	}

	FileInfo CiOsIO::GetFileInfoW(const wchar_t *filePath)
	{
		std::string filePathA = utility::UTF16ToUTF8(filePath);
		return GetFileInfoA(filePathA.c_str());
	}

	FileInfo CiOsIO::GetFileInfoA(const char *filePath)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		FileInfo inf;

		afc_Directory *dic = NULL;
		unsigned int result = sAFCOpenFileInfFunc(m_afcConnection,(char *)filePath, &dic);
		if (result)
		{
			throw std::wstring(L"准备查询该文件信息时失败: ") + utility::UTF8ToUTF16(filePath);
		}

		char *key = NULL, *value = NULL;
		result = sAFCReadFileInfFunc(dic, &key, &value);
		while (key && value)
		{
			if (result)
			{
				throw std::wstring(L"查询该文件信息时失败: ") + utility::UTF8ToUTF16(filePath);
			}
			std::string keyStr = key, valueStr = value;
			if (!keyStr.compare("st_size"))
			{
				inf.fileLen = atol(value);
			}
			else if (!keyStr.compare("st_blocks"))
			{
				inf.blockSize = atoi(value);
			}
			else if (!keyStr.compare("st_nlink"))
			{
				inf.linkCount = atoi(value);
			}
			else if (!keyStr.compare("st_ifmt"))
			{
				if (!valueStr.compare("S_IFDIR"))
				{
					inf.fileType = kDirectory;
				}
				else if (!valueStr.compare("S_IFLNK"))
				{
					inf.fileType = kSymbolLink;
				}
				else if (!valueStr.compare("S_IFREG"))
				{
					inf.fileType = kRegularFile;
				}
				else if (!valueStr.compare("S_IFBLK"))
				{
					inf.fileType = kBlockLink;
				}
				else if (!valueStr.compare("S_IFCHR"))
				{
					inf.fileType = kCHR;
				}
				else if (!valueStr.compare("S_IFIFO"))
				{
					inf.fileType = kFIFO;
				}
				else if (!valueStr.compare("S_IFMT"))
				{
					inf.fileType = kMT;
				}
				else if (!valueStr.compare("S_IFSOCK"))
				{
					inf.fileType = kSock;
				}
			}
			result = sAFCReadFileInfFunc(dic, &key, &value);
		} 

		result = sAFCCloseFileInfFunc(dic);
		return inf;
	}

	bool CiOsIO::IsLnkToDirectoryA(const char *filePath)
	{
		afc_directory *dic = NULL;
		unsigned int result = sAFCOpenDirectoryFunc(m_afcConnection, (char *)filePath, &dic);
		if(result)
		{
			return false;
		}
		else
		{
			sAFCCloseDirectoryFunc(m_afcConnection, dic);
			return true;
		}
	}

	bool CiOsIO::IsLnkToDirectoryW(const wchar_t *filePath)
	{
		afc_directory *dic = NULL;
		std::string filePathA = utility::UTF16ToUTF8(filePath);
		return IsLnkToDirectoryA(filePathA.c_str());
	}

	iOsDirectory CiOsIO::OpenDirectoryA(const char *filePath)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		afc_directory *dic = NULL;
		unsigned int result = sAFCOpenDirectoryFunc(m_afcConnection, (char *)filePath, &dic);
		if (result)
		{
			throw std::wstring(L"打开该目录信息失败: ") + utility::UTF8ToUTF16(filePath);
		}
		return dic;
	}

	iOsDirectory CiOsIO::OpenDirectoryW(const wchar_t *filePath)
	{
		std::string filePathA = utility::UTF16ToUTF8(filePath);
		return OpenDirectoryA(filePathA.c_str());
	}

	std::string CiOsIO::ReadDirectoryA(iOsDirectory dic)
	{
		char *subName = NULL;
		int result = sAFCReadDirectoryFunc(m_afcConnection, dic, &subName);
		std::string subNameStr;
		if(subName)
		{
			while (!strcmp(subName, "..") || !strcmp(subName, "."))
			{
				result = sAFCReadDirectoryFunc(m_afcConnection, dic, &subName);
				if(!subName)
				{
					break;
				}
			}
			if(subName)
				subNameStr = subName;
		}
		return subNameStr;
	}

	std::wstring CiOsIO::ReadDirectoryW(iOsDirectory dic)
	{
		std::string subName = ReadDirectoryA(dic);
		return utility::UTF8ToUTF16(subName);
	}

	void CiOsIO::CloseDirectory(iOsDirectory dic)
	{
		sAFCCloseDirectoryFunc(m_afcConnection, dic);
	}

	void CiOsIO::EnumDirectory(std::vector<std::string> &names, const char *filePath)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		afc_directory *dic = NULL;
		unsigned int result = sAFCOpenDirectoryFunc(m_afcConnection, (char *)filePath, &dic);
		if (result)
		{
			throw std::wstring(L"打开该目录信息失败: ") + utility::UTF8ToUTF16(filePath);
		}

		char *subName = NULL;
		result = sAFCReadDirectoryFunc(m_afcConnection, dic, &subName);
		while(subName)
		{
			if (result)
			{
				throw std::wstring(L"读取该目录信息失败: ") + utility::UTF8ToUTF16(filePath);
			}
			std::string subNameStr = subName;
			if (!subNameStr.compare("..") || !subNameStr.compare("."))
			{
				result = sAFCReadDirectoryFunc(m_afcConnection, dic, &subName);
				continue;
			}

			names.push_back(subName);

			result = sAFCReadDirectoryFunc(m_afcConnection, dic, &subName);
		}

		sAFCCloseDirectoryFunc(m_afcConnection, dic);
	}

	std::vector<std::string> CiOsIO::EnumDirectoryA(const char *filePath)
	{
		std::vector<std::string> paths;
		EnumDirectory(paths, filePath);
		return paths;
	}

	std::vector<std::wstring> CiOsIO::EnumDirectoryW(const wchar_t *filePath)
	{
		std::vector<std::string> paths;
		std::string filePathA = utility::UTF16ToUTF8(filePath);
		EnumDirectory(paths, filePathA.c_str());

		std::vector<std::wstring> wpaths;
		for (size_t i = 0; i < paths.size(); ++i)
		{
			wpaths.push_back(utility::UTF8ToUTF16(paths[i]));
		}
		return wpaths;
	}

	iOsFile CiOsIO::OpenFileW(const wchar_t *filePath)
	{
		std::string filePathA = utility::UTF16ToUTF8(filePath);
		return OpenFileA(filePathA.c_str());
	}

	iOsFile CiOsIO::OpenFileA(const char *filePath)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		iOsFile file = 0;
		unsigned int result = sAFCOpenFileFunc(m_afcConnection, (char *)filePath, 2, &file);
		if (result)
		{
			throw std::wstring(L"打开文件失败");
		}
		return file;
	}

	void CiOsIO::ReadFile(iOsFile file, void *buf, unsigned int &len, const unsigned int pos/* = 0*/)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		unsigned int result = sAFCSeekFileFunc(m_afcConnection, file, pos, 0);
		result = sAFCReadFileFunc(m_afcConnection, file, buf, &len);
		if (result)
		{
			throw std::wstring(L"读取文件出错");
		}
	}

	void CiOsIO::ReadFile(iOsFile file, void *buf, unsigned int &len)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		unsigned int  result = sAFCReadFileFunc(m_afcConnection, file, buf, &len);
		if (result == 11)
		{
			throw std::wstring(L"尝试读取目录");
		}
		else if (result)
		{
			throw std::wstring(L"读取文件出错");
		}
	}

	void CiOsIO::WriteFile(iOsFile file, void *buf, const unsigned int len, const unsigned int pos/* = 0*/)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		unsigned int result = sAFCSeekFileFunc(m_afcConnection, file, pos, 0);
		if (result)
		{
			throw std::wstring(L"设置文件读写位置失败");
		}
		result = sAFCWriteFileFunc(m_afcConnection, file, buf, len);
		if (result)
		{
			throw std::wstring(L"写入文件出错");
		}
	}

	void CiOsIO::WriteFile(iOsFile file, void *buf, const unsigned int len)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		unsigned int result = sAFCWriteFileFunc(m_afcConnection, file, buf, len);
		if (result)
		{
			throw std::wstring(L"写入文件出错");
		}
	}

	void CiOsIO::SeekFile(iOsFile file, const unsigned int offset, const unsigned int pos)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}

		unsigned int result = sAFCSeekFileFunc(m_afcConnection, file, offset, pos);
		if (result)
		{
			throw std::wstring(L"设置文件读写位置失败");
		}
	}

	void CiOsIO::CloseFile(iOsFile file)
	{
		sAFCCloseFileFunc(m_afcConnection, file);
	}

	void CiOsIO::RemovePathA(const char *filePath)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}
		unsigned int result = sAFCRemovePathFunc(m_afcConnection, (char *)filePath);
		if(result)
		{
			throw std::wstring(L"移除以下目录失败:") + utility::UTF8ToUTF16(filePath);;
		}
	}

	void CiOsIO::RemovePathW(const wchar_t *filePath)
	{
		std::string filePathA = utility::UTF16ToUTF8(filePath);
		RemovePathA(filePathA.c_str());
	}

	void CiOsIO::RenamePathA(const char *from, const char *to)
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}
		unsigned int result = sAFCRenamePathFunc(m_afcConnection, (char *)from, (char *)to);
		if(result)
		{
			throw std::wstring(L"重名名以下目录失败:") + utility::UTF8ToUTF16(from);
		}
	}

	void CiOsIO::RenamePathW(const wchar_t *from, const wchar_t *to)
	{
		std::string fromA = utility::UTF16ToUTF8(from), toA = utility::UTF16ToUTF8(to);
		RenamePathA(fromA.c_str(), toA.c_str());
	}

	void CiOsIO::CreateDirectoryA( const char *filePath )
	{
		if (!m_isConnected)
		{
			throw std::wstring(L"未连接到iPhone");
		}

		if (!m_afcConnected)
		{
			try
			{
				StartAFCService();
			}
			catch (std::wstring &e)
			{
				throw e;
			}
		}
		unsigned int result = sAFCCreateDirectoryFunc(m_afcConnection, (char *)filePath);
		if(result)
		{
			throw std::wstring(L"创建以下目录失败:") + utility::UTF8ToUTF16(filePath);
		}
	}

	void CiOsIO::CreateDirectoryW( const wchar_t *filePath )
	{
		std::string filePathA = utility::UTF16ToUTF8(filePath);
		CreateDirectoryA(filePathA.c_str());
	}

	bool CiOsIO::IsConnected()
	{
		return m_isConnected;
	}
}


