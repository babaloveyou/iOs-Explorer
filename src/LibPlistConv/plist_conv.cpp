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
#include "plist_conv.h"

#include "utility/codepage.h"

HMODULE													CPlistConv::m_coreFundationDllHandle = NULL;
bool													CPlistConv::m_isInited = false;
CPlistConv::tf_CFWriteStreamCreateWithFile				CPlistConv::sCFWriteStreamCreateWithFile = NULL;
CPlistConv::tf_CFReadStreamCreateWithFile				CPlistConv::sCFReadStreamCreateWithFile = NULL;
CPlistConv::tf_CFStringCreateWithCString				CPlistConv::sCFStringCreateWithCString = NULL;
CPlistConv::tf_CFURLCreateWithFileSystemPath			CPlistConv::sCFURLCreateWithFileSystemPath = NULL;
CPlistConv::tf_CFReadStreamOpen							CPlistConv::sCFReadStreamOpen = NULL;
CPlistConv::tf_CFWriteStreamOpen						CPlistConv::sCFWriteStreamOpen = NULL;
CPlistConv::tf_CFPropertyListCreateFromStream			CPlistConv::sCFPropertyListCreateFromStream = NULL;
CPlistConv::tf_CFReadStreamClose						CPlistConv::sCFReadStreamClose = NULL;
CPlistConv::tf_CFPropertyListIsValid					CPlistConv::sCFPropertyListIsValid = NULL;
CPlistConv::tf_CFPropertyListWriteToStream				CPlistConv::sCFPropertyListWriteToStream = NULL;
CPlistConv::tf_CFWriteStreamClose						CPlistConv::sCFWriteStreamClose = NULL;
CPlistConv::tf_CFRelease								CPlistConv::sCFRelease = NULL;
CPlistConv::tf_CFURLCreateDataAndPropertiesFromResource	CPlistConv::sCFURLCreateDataAndPropertiesFromResource = NULL;
CPlistConv::tf_CFPropertyListCreateFromXMLData          CPlistConv::sCFPropertyListCreateFromXMLData = NULL;
CPlistConv::tf_CFPropertyListCreateXMLData              CPlistConv::sCFPropertyListCreateXMLData = NULL;
CPlistConv::tf_CFURLWriteDataAndPropertiesToResource    CPlistConv::sCFURLWriteDataAndPropertiesToResource = NULL;

CPlistConv::CPlistConv(const std::wstring &coreFundationPath)
{
	if (!m_isInited)
	{
		std::wstring oriPath;
		m_coreFundationDllHandle = LoadLibrary(coreFundationPath.c_str());
		if (!m_coreFundationDllHandle)
		{
			HKEY hSetting = NULL;
			if (::RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Apple Inc.\\Apple Mobile Device Support\\Shared", &hSetting) == ERROR_SUCCESS)
			{
				DWORD length = 0;
				if (::RegQueryValueEx(hSetting, L"iTunesMobileDeviceDLL", NULL, NULL, NULL, &length) == ERROR_SUCCESS)
				{
					wchar_t *iTuneDllPathTemp = new wchar_t[length + 1];
					::RegQueryValueEx(hSetting, L"iTunesMobileDeviceDLL", NULL, NULL, (LPBYTE)iTuneDllPathTemp, &length);
					std::wstring pathTail = iTuneDllPathTemp;
					size_t pos = pathTail.rfind(L'\\');
					if (pos != std::wstring::npos)
					{
						pathTail = pathTail.substr(0, pos);
					}
					wchar_t pathTemp[MAX_PATH] = {0};
					oriPath = pathTemp;
					::GetEnvironmentVariable(L"PATH", pathTemp, MAX_PATH);
					pathTail = pathTemp + (L";" + pathTail);
					::SetEnvironmentVariable(L"PATH", pathTail.c_str());

					m_coreFundationDllHandle = LoadLibrary(L"CoreFoundation.dll");
				}
			}
		}
		if (m_coreFundationDllHandle)
		{
			sCFWriteStreamCreateWithFile = (CPlistConv::tf_CFWriteStreamCreateWithFile)GetProcAddress(m_coreFundationDllHandle, "CFWriteStreamCreateWithFile");
			sCFReadStreamCreateWithFile = (CPlistConv::tf_CFReadStreamCreateWithFile)GetProcAddress(m_coreFundationDllHandle, "CFReadStreamCreateWithFile");
			sCFStringCreateWithCString = (CPlistConv::tf_CFStringCreateWithCString)GetProcAddress(m_coreFundationDllHandle, "CFStringCreateWithCString");
			sCFURLCreateWithFileSystemPath = (CPlistConv::tf_CFURLCreateWithFileSystemPath)GetProcAddress(m_coreFundationDllHandle, "CFURLCreateWithFileSystemPath");
			sCFReadStreamOpen = (CPlistConv::tf_CFReadStreamOpen)GetProcAddress(m_coreFundationDllHandle, "CFReadStreamOpen");
			sCFWriteStreamOpen = (CPlistConv::tf_CFWriteStreamOpen)GetProcAddress(m_coreFundationDllHandle, "CFWriteStreamOpen");
			sCFPropertyListCreateFromStream = (CPlistConv::tf_CFPropertyListCreateFromStream)GetProcAddress(m_coreFundationDllHandle, "CFPropertyListCreateFromStream");
			sCFReadStreamClose = (CPlistConv::tf_CFReadStreamClose)GetProcAddress(m_coreFundationDllHandle, "CFReadStreamClose");
			sCFPropertyListIsValid = (CPlistConv::tf_CFPropertyListIsValid)GetProcAddress(m_coreFundationDllHandle, "CFPropertyListIsValid");
			sCFPropertyListWriteToStream = (CPlistConv::tf_CFPropertyListWriteToStream)GetProcAddress(m_coreFundationDllHandle, "CFPropertyListWriteToStream");
			sCFWriteStreamClose = (CPlistConv::tf_CFWriteStreamClose)GetProcAddress(m_coreFundationDllHandle, "CFWriteStreamClose");
			sCFRelease = (CPlistConv::tf_CFRelease)GetProcAddress(m_coreFundationDllHandle, "CFRelease");
			sCFURLCreateDataAndPropertiesFromResource = (CPlistConv::tf_CFURLCreateDataAndPropertiesFromResource)GetProcAddress(m_coreFundationDllHandle, "CFURLCreateDataAndPropertiesFromResource");
			sCFPropertyListCreateFromXMLData = (CPlistConv::tf_CFPropertyListCreateFromXMLData)GetProcAddress(m_coreFundationDllHandle, "CFPropertyListCreateFromXMLData");
			sCFPropertyListCreateXMLData = (CPlistConv::tf_CFPropertyListCreateXMLData)GetProcAddress(m_coreFundationDllHandle, "CFPropertyListCreateXMLData");
			sCFURLWriteDataAndPropertiesToResource = (CPlistConv::tf_CFURLWriteDataAndPropertiesToResource)GetProcAddress(m_coreFundationDllHandle, "CFURLWriteDataAndPropertiesToResource");
			if (sCFWriteStreamCreateWithFile&&sCFReadStreamCreateWithFile&&sCFStringCreateWithCString&&sCFURLCreateWithFileSystemPath&&sCFReadStreamOpen&&
				sCFWriteStreamOpen&&sCFPropertyListCreateFromStream&&sCFReadStreamClose&&sCFPropertyListIsValid&&sCFPropertyListWriteToStream&&
				sCFWriteStreamClose&&sCFRelease&&sCFURLCreateDataAndPropertiesFromResource&&sCFPropertyListCreateFromXMLData&&sCFPropertyListCreateXMLData&&
				sCFPropertyListCreateXMLData&&sCFURLWriteDataAndPropertiesToResource)
			{
				m_isInited = true;
			}
		}
		::SetEnvironmentVariable(L"PATH", oriPath.c_str());
	}
}

CPlistConv::~CPlistConv(void)
{
}

bool CPlistConv::BinaryToXML( const wchar_t *src, const wchar_t *dst )
{
	if (!m_isInited)
	{
		return false;
	}
	CFStringRef filePathCFString = sCFStringCreateWithCString(0, utility::UTF16ToUTF8(src).c_str(), kCFStringEncodingUTF8);
	if (!filePathCFString)
	{
		return false;
	}
	CFURLRef fileURL = sCFURLCreateWithFileSystemPath(0, filePathCFString, kCFURLWindowsPathStyle, false);
	sCFRelease(filePathCFString);
	if (!fileURL)
	{
		return false;
	}

	signed int errorCode;
	CFDataRef resourceData;
	unsigned char status = sCFURLCreateDataAndPropertiesFromResource(0, fileURL, &resourceData, NULL, NULL, &errorCode);
	sCFRelease(fileURL);

	bool bRtn = false;
	const void * propertyList = NULL;
	if (resourceData)
	{
		CFStringRef errorString;
		propertyList = sCFPropertyListCreateFromXMLData(0, resourceData,kCFPropertyListImmutable, &errorString);
		sCFRelease(resourceData);
		if (propertyList)
		{
			filePathCFString = sCFStringCreateWithCString(0, utility::UTF16ToUTF8(dst).c_str(), kCFStringEncodingUTF8);
			if (filePathCFString)
			{
				fileURL = sCFURLCreateWithFileSystemPath(0, filePathCFString, kCFURLWindowsPathStyle, false);
				sCFRelease(filePathCFString);
				if (fileURL)
				{
					CFDataRef xmlData = sCFPropertyListCreateXMLData(0, propertyList);
					sCFRelease(propertyList);
					if (xmlData)
					{
						bRtn = sCFURLWriteDataAndPropertiesToResource(fileURL, xmlData, NULL, &errorCode)?true:false;
						sCFRelease(fileURL);
						sCFRelease(xmlData);
					}
				}
			}
		}
	}
	return bRtn;
}

bool CPlistConv::XMLToBinary( const wchar_t *src, const wchar_t *dst )
{
	if (!m_isInited)
	{
		return false;
	}
	CFStringRef filePathCFString = sCFStringCreateWithCString(0, utility::UTF16ToUTF8(src).c_str(), kCFStringEncodingUTF8);
	if (!filePathCFString)
	{
		return false;
	}
	CFURLRef fileURL = sCFURLCreateWithFileSystemPath(0, filePathCFString, kCFURLWindowsPathStyle, false);
	sCFRelease(filePathCFString);
	if (!fileURL)
	{
		return false;
	}

	signed int errorCode;
	CFDataRef resourceData;
	unsigned char status = sCFURLCreateDataAndPropertiesFromResource(0, fileURL, &resourceData, NULL, NULL, &errorCode);
	sCFRelease(fileURL);

	bool bRtn = false;
	const void * propertyList = NULL;
	if (resourceData)
	{
		CFStringRef errorString;
		propertyList = sCFPropertyListCreateFromXMLData(0, resourceData,kCFPropertyListImmutable, &errorString);
		sCFRelease(resourceData);
		if (propertyList)
		{
			filePathCFString = sCFStringCreateWithCString(0, utility::UTF16ToUTF8(dst).c_str(), kCFStringEncodingUTF8);
			if (filePathCFString)
			{
				fileURL = sCFURLCreateWithFileSystemPath(0, filePathCFString, kCFURLWindowsPathStyle, false);
				sCFRelease(filePathCFString);
				if (fileURL)
				{
					CFWriteStreamRef writer = sCFWriteStreamCreateWithFile(0, fileURL);
					sCFRelease(fileURL);
					if (writer)
					{
						sCFWriteStreamOpen(writer);
						CFStringRef error;
						bRtn = sCFPropertyListWriteToStream(propertyList, writer, 200, &error) == 0?false:true;
						sCFWriteStreamClose(writer);
						sCFRelease(propertyList);
					}
				}
			}
		}
	}
	return bRtn;
}