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
#ifndef LIBPLISTCONV_PLISTCONV_H_
#define LIBPLISTCONV_PLISTCONV_H_

#include <string>

#include <Windows.h>

#include "iTunesMobileDevice.h"

class CPlistConv
{

public:
	CPlistConv(const std::wstring &coreFundationPath);
	~CPlistConv(void);

	bool BinaryToXML(const wchar_t *src, const wchar_t *dst);
	bool XMLToBinary(const wchar_t *src, const wchar_t *dst);
	
private:
	static HMODULE										m_coreFundationDllHandle;
	static bool											m_isInited;

	typedef CFWriteStreamRef (*tf_CFWriteStreamCreateWithFile)(CFAllocatorRef, CFURLRef);
	typedef void (*tf_CFWriteStreamClose)( CFWriteStreamRef);
	typedef CFReadStreamRef (*tf_CFReadStreamCreateWithFile)(CFAllocatorRef, CFURLRef);
	typedef CFStringRef (*tf_CFStringCreateWithCString)(CFAllocatorRef, const char *, unsigned int) ;
	typedef CFURLRef (*tf_CFURLCreateWithFileSystemPath)(CFAllocatorRef, CFStringRef, signed long, unsigned char);
	typedef unsigned char (*tf_CFReadStreamOpen)(CFReadStreamRef);
	typedef unsigned char (*tf_CFWriteStreamOpen)(CFWriteStreamRef);
	typedef const void * (*tf_CFPropertyListCreateFromStream)(CFAllocatorRef, CFReadStreamRef, signed long, unsigned long,signed long *,CFStringRef *);
	typedef void (*tf_CFReadStreamClose)(CFReadStreamRef);
	typedef unsigned char (*tf_CFPropertyListIsValid)( const void *,signed long);
	typedef signed long (*tf_CFPropertyListWriteToStream)( const void *,CFWriteStreamRef,signed long,CFStringRef *) ;
	typedef void (*tf_CFRelease)(const void * cf);
	typedef unsigned char (*tf_CFURLCreateDataAndPropertiesFromResource)(CFAllocatorRef alloc, CFURLRef url, CFDataRef *resourceData, CFDictionaryRef *properties, CFArrayRef desiredProperties, signed int *errorCode);
	typedef const void * (*tf_CFPropertyListCreateFromXMLData)(CFAllocatorRef allocator, CFDataRef xmlData, unsigned long mutabilityOption, CFStringRef *errorString);
	typedef CFDataRef (*tf_CFPropertyListCreateXMLData)(CFAllocatorRef allocator,  const void * propertyList);
	typedef unsigned char (*tf_CFURLWriteDataAndPropertiesToResource)(CFURLRef url, CFDataRef dataToWrite, CFDictionaryRef propertiesToWrite, signed int *errorCode);

	static tf_CFWriteStreamCreateWithFile				sCFWriteStreamCreateWithFile;
	static tf_CFReadStreamCreateWithFile				sCFReadStreamCreateWithFile;
	static tf_CFStringCreateWithCString					sCFStringCreateWithCString;
	static tf_CFURLCreateWithFileSystemPath				sCFURLCreateWithFileSystemPath;
	static tf_CFReadStreamOpen							sCFReadStreamOpen;
	static tf_CFWriteStreamOpen							sCFWriteStreamOpen;
	static tf_CFPropertyListCreateFromStream			sCFPropertyListCreateFromStream;
	static tf_CFReadStreamClose							sCFReadStreamClose;
	static tf_CFPropertyListIsValid						sCFPropertyListIsValid;
	static tf_CFPropertyListWriteToStream				sCFPropertyListWriteToStream;
	static tf_CFWriteStreamClose						sCFWriteStreamClose;
	static tf_CFRelease									sCFRelease;
	static tf_CFURLCreateDataAndPropertiesFromResource	sCFURLCreateDataAndPropertiesFromResource;
	static tf_CFPropertyListCreateFromXMLData           sCFPropertyListCreateFromXMLData;
	static tf_CFPropertyListCreateXMLData               sCFPropertyListCreateXMLData;
	static tf_CFURLWriteDataAndPropertiesToResource     sCFURLWriteDataAndPropertiesToResource;

};

#endif

