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
#ifndef IOSIO_IOSIO_H_
#define IOSIO_IOSIO_H_

#include <vector>
#include <string>

#include <Windows.h>

#include "iTunesMobileDevice.h"

namespace iOsIO
{
	enum FileTypes
	{
		kDirectory, // 目录
		kSymbolLink, // 符号连接
		kRegularFile, // 正常文件
		kBlockLink, //S_IFBLK
		kCHR, //S_IFCHR
		kFIFO, //S_IFIFO
		kMT, //S_IFMT
		kSock, //S_IFSOCK
		kUnkownFileType,
	};

	struct FileInfo
	{
		long		fileLen;
		int			linkCount;
		int			blockSize;
		FileTypes	fileType;
		FileInfo()
		{
			fileLen = 0;
			linkCount = 0;
			blockSize = 0;
			fileType = kUnkownFileType;
		}
	};

	void NotifyCallBack(am_device_notification_callback_info *deviceInfo);
	typedef afc_file_ref iOsFile;
	typedef afc_directory * iOsDirectory;

	class CiOsIO
	{
	public:
		CiOsIO(const unsigned short waitConnectDelay = 5);
		CiOsIO(const wchar_t* iTuneDllPath, const unsigned short waitConnectDelay = 5);

		virtual ~CiOsIO(void);

		void Connect();
		bool IsConnected();

		// AFC文件系统服务
		void StartAFCService();
		void StopAFCService();

		// 文件信息
		FileInfo GetFileInfoA(const char *filePath);
		FileInfo GetFileInfoW(const wchar_t *filePath);

		bool IsLnkToDirectoryA(const char *filePath);
		bool IsLnkToDirectoryW(const wchar_t *filePath);

		iOsFile OpenFileA(const char *filePath);
		iOsFile OpenFileW(const wchar_t *filePath);

		void ReadFile(iOsFile file, void *buf, unsigned int &len);
		void ReadFile(iOsFile file, void *buf, unsigned int &len, const unsigned int pos);

		void WriteFile(iOsFile file, void *buf, const unsigned int len);
		void WriteFile(iOsFile file, void *buf, const unsigned int len, const unsigned int pos);

		void SeekFile(iOsFile file, const unsigned int offset, const unsigned int pos);

		void CloseFile(iOsFile file);

		std::vector<std::string> EnumDirectoryA(const char *filePath);
		std::vector<std::wstring> EnumDirectoryW(const wchar_t *filePath);

		iOsDirectory OpenDirectoryA(const char *filePath);
		iOsDirectory OpenDirectoryW(const wchar_t *filePath);

		std::string ReadDirectoryA(iOsDirectory dic);
		std::wstring ReadDirectoryW(iOsDirectory dic);

		void CloseDirectory(iOsDirectory dic);

		void CreateDirectoryA(const char *filePath);
		void CreateDirectoryW(const wchar_t *filePath);

		void RemovePathA(const char *filePath);
		void RemovePathW(const wchar_t *filePath);

		void RenamePathA(const char *from, const char *to);
		void RenamePathW(const wchar_t *from, const wchar_t *to);

	protected:

		friend void NotifyCallBack(am_device_notification_callback_info *deviceInfo);
		void Init(const wchar_t* iTuneDllPath);
		static unsigned char * GenerateCFString(const char *normalStr);
		void EnumDirectory(std::vector<std::string> &names, const char *filePath);

		static am_device    *m_device;
		static bool    m_isConnected;

		am_device_notification    *m_devicenotify;

		unsigned short    m_waitConnectDelay;
		HMODULE    m_itunesDllHmodule;
		afc_connection    *m_afcConnection;
		bool    m_afcConnected;

		typedef unsigned int (*NotifySubFunc)(am_device_notification_callback, unsigned int, unsigned int, unsigned int, am_device_notification **);
		typedef unsigned int (*NotifyUnSubFunc)(am_device_notification *);

		typedef unsigned int (*ConnectFunc)(am_device *);
		typedef int (*IsPairedFunc)(am_device *);
		typedef unsigned int (*ValidatePairingFunc)(am_device *);
		typedef unsigned int (*PairingFunc)(am_device *);
		typedef unsigned int (*UnpairingFunc)(am_device *);

		typedef unsigned int (*StartSessionFunc)(am_device *);
		typedef unsigned int (*StartServiceFunc)(am_device *, unsigned char *, afc_connection **, unsigned int *);
		typedef unsigned int (*StopSessionFunc)(am_device *);

		typedef unsigned int (*OpenConnectionFunc)(afc_connection *, unsigned int, afc_connection **);
		typedef unsigned int (*OpenDeviceInfoFunc)(afc_connection *, afc_Directory **);

		typedef unsigned int (*OpenDirectoryFunc)(afc_connection *, char *, afc_directory **);
		typedef unsigned int (*ReadDirectoryFunc)(afc_connection *, afc_directory *,char **);
		typedef unsigned int (*CloseDirectoryFunc)(afc_connection *, afc_directory *);
		typedef unsigned int (*CreateDirectoryFunc)(afc_connection *, char *);

		typedef unsigned int (*OpenFileInfoFunc)(afc_connection *, char *, afc_Directory **);
		typedef unsigned int (*ReadKeyValueFunc)(afc_Directory *, char **, char **);
		typedef unsigned int (*CloseKeyValueFunc)(afc_Directory *);

		typedef unsigned int (*OpenFileRefFunc)(afc_connection *, char *, unsigned long long int, afc_file_ref *);
		typedef unsigned int (*ReadFileRefFunc)(afc_connection *, afc_file_ref, void *, unsigned int *);
		typedef unsigned int (*WriteFileRefFunc)(afc_connection *, afc_file_ref ,void *, unsigned int);
		typedef unsigned int (*SeekFileRefFunc)(afc_connection *, afc_file_ref, unsigned long long, unsigned long long);
		typedef unsigned int (*CloseFileRefFunc)(afc_connection *, afc_file_ref);

		typedef unsigned int (*RemovePathFunc)(afc_connection *, char *);
		typedef unsigned int (*RenamePathFunc)(afc_connection *, char *, char *);

		typedef unsigned int (*CloseConnectionFunc)(afc_connection *);
		typedef unsigned int (*DisConnectFunc)(am_device *);
		typedef unsigned int (*ReleaseDeviceFunc)(am_device *);

		static NotifySubFunc    	sNotifySubFunc;
		static NotifyUnSubFunc		sNotifyUnSubFunc;

		static ConnectFunc			sConnectFunc;
		static IsPairedFunc		    sIsDevicePaireFunc;
		static ValidatePairingFunc	sValidatePairingFunc;
		static PairingFunc			sPairFunc;
		static UnpairingFunc		sUnPairFunc;

		static StartSessionFunc	    sStartSessionFunc;
		static StartServiceFunc	    sStartServiceFunc;
		static StopSessionFunc		sStopSessionFunc;

		static OpenConnectionFunc	sAFCConnectFunc;
		static OpenDeviceInfoFunc	sAFCOpenDeviceFunc;

		static OpenDirectoryFunc	sAFCOpenDirectoryFunc;
		static ReadDirectoryFunc	sAFCReadDirectoryFunc;
		static CloseDirectoryFunc	sAFCCloseDirectoryFunc;

		static CreateDirectoryFunc	sAFCCreateDirectoryFunc;

		static OpenFileInfoFunc	    sAFCOpenFileInfFunc;
		static ReadKeyValueFunc   	sAFCReadFileInfFunc;
		static CloseKeyValueFunc	sAFCCloseFileInfFunc;

		static OpenFileRefFunc		sAFCOpenFileFunc;
		static ReadFileRefFunc		sAFCReadFileFunc;
		static WriteFileRefFunc 	sAFCWriteFileFunc;
		static SeekFileRefFunc		sAFCSeekFileFunc;
		static CloseFileRefFunc 	sAFCCloseFileFunc;

		static RemovePathFunc		sAFCRemovePathFunc;
		static RenamePathFunc		sAFCRenamePathFunc;

		static CloseConnectionFunc	sAFCDisconnectFunc;

		static DisConnectFunc		sDisconnectFunc;
		static ReleaseDeviceFunc	sReleaseDeviceFunc;

	};
}

#endif // IOSIO_IOSIO_H_

