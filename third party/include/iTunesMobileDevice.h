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
#ifndef MOBILEDEVICE_H
#define MOBILEDEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

class unknown
{
};

typedef unknown* CFMutableDirectoryRef;
typedef unknown* CFDirectoryRef;

typedef struct __CFReadStream * CFReadStreamRef;
typedef struct __CFWriteStream * CFWriteStreamRef;
typedef const struct __CFURL * CFURLRef;
typedef const struct __CFAllocator * CFAllocatorRef;
typedef const struct __CFData * CFDataRef;
typedef struct __CFString* CFStringRef;
typedef const struct __CFArray * CFArrayRef;
typedef const struct __CFDictionary * CFDictionaryRef;

enum {
	kCFURLPOSIXPathStyle = 0,
	kCFURLHFSPathStyle,
	kCFURLWindowsPathStyle
};

enum {
	kCFStringEncodingMacRoman = 0,
	kCFStringEncodingWindowsLatin1 = 0x0500,
	kCFStringEncodingISOLatin1 = 0x0201,
	kCFStringEncodingNextStepLatin = 0x0B01,
	kCFStringEncodingASCII = 0x0600,
	kCFStringEncodingUnicode = 0x0100,
	kCFStringEncodingUTF8 = 0x08000100,
	kCFStringEncodingNonLossyASCII = 0x0BFF
};

enum {
	kCFPropertyListImmutable = 0,
	kCFPropertyListMutableContainers,
	kCFPropertyListMutableContainersAndLeaves
};

/* System/Library/Lockdown/Services.plist */
// ("com.apple.afc") 文件服务
// ("com.apple.mobilebackup") 备份
// ("com.apple.crashreportcopy") 错误报告
// ("com.apple.mobile.debug_image_mount") 调试镜像
// ("com.apple.mobile.notification_proxy")
// ("com.apple.purpletestr")
// ("com.apple.mobile.software_update")
// ("com.apple.mobilesync")
// ("com.apple.screenshotr")
// ("com.apple.syslog_relay")
// ("com.apple.mobile.system_profiler")

struct am_recovery_device;
struct am_dfu_device;

#pragma pack(1)
struct __CFString
{
        unsigned long        unk0;
        unsigned char        sign[4];
        unsigned char        length;
        char*       string;
        void        Init()
        {
				unk0 = 0;
                sign[0] = 0x8c;
                sign[1] = 07;
                sign[2] = 01;
                sign[3] = 0;
        }
};
typedef __CFString* CFStringRef;

struct am_device_notification_callback_info {
    struct am_device *dev;
    unsigned int msg;
};

typedef void (*am_restore_device_notification_callback)(struct am_recovery_device *);

typedef void (*am_dfu_device_notification_callback)(struct am_dfu_device *);

struct am_recovery_device {
    unsigned char unknown0[8];
    am_restore_device_notification_callback callback;
    void *user_info;
    unsigned char unknown1[12];
    unsigned int readwrite_pipe;
    unsigned char read_pipe;
    unsigned char write_ctrl_pipe;
    unsigned char read_unknown_pipe;
    unsigned char write_file_pipe;
    unsigned char write_input_pipe;
};

struct am_restore_device {
    unsigned char unknown[32];
    int port;
};

struct am_dfu_device {
    unsigned char unknown[32];
    int port;
};

typedef void(*am_device_notification_callback)(struct am_device_notification_callback_info *);

typedef void *amd_device_attached_callback;

typedef void (*am_restore_device_notification_callback)(struct
    am_recovery_device *);

struct am_device {
    unsigned char unknown0[16];
    unsigned int device_id;
    unsigned int product_id;
    char *serial;
    unsigned int unknown1;
    unsigned char unknown2[4]; 
    unsigned int lockdown_conn;
    unsigned char unknown3[8];
};

struct am_device_notification {
    unsigned int unknown0;
    unsigned int unknown1;
    unsigned int unknown2;
    am_device_notification_callback callback;
    unsigned int unknown3;
};

struct afc_connection {
    unsigned int handle;
    unsigned int unknown0;
    unsigned char unknown1;
    unsigned char padding[3];
    unsigned int unknown2;
    unsigned int unknown3;
    unsigned int unknown4;
    unsigned int fs_block_size;
    unsigned int sock_block_size;
    unsigned int io_timeout;
    void *afc_lock;
    unsigned int context;
};

struct afc_device_info {
    unsigned char unknown[12];
};

struct afc_directory {
    unsigned char unknown[1];
};

struct afc_Directory {
    unsigned char unknown[1];
};

typedef unsigned long long afc_file_ref;

struct usbmux_listener_1 {
    unsigned int unknown0;
    unsigned char *unknown1;
    amd_device_attached_callback callback; 
    unsigned int unknown3;
    unsigned int unknown4;
    unsigned int unknown5;
};

struct usbmux_listener_2 {
    unsigned char unknown0[4144];
};

struct am_bootloader_control_packet {
    unsigned char opcode;
    unsigned char length;
    unsigned char magic[2];
    unsigned char payload[1];
};

#pragma pack()

#ifdef __cplusplus
}
#endif
#endif