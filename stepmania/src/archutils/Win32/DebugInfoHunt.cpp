#include "global.h"
#include "DebugInfoHunt.h"
#include "RageLog.h"
#include "RageUtil.h"
#include "VideoDriverInfo.h"
#include "arch/ArchHooks/ArchHooks.h"


void LogVideoDriverInfo( VideoDriverInfo info )
{
	LOG->Info("Video Driver Information:");
	LOG->Info("%-15s:\t%s", "Provider", info.sProvider.c_str());
	LOG->Info("%-15s:\t%s", "Description", info.sDescription.c_str());
	LOG->Info("%-15s:\t%s", "Version", info.sVersion.c_str());
	LOG->Info("%-15s:\t%s", "Date", info.sDate.c_str());
	LOG->Info("%-15s:\t%s", "DeviceID", info.sDeviceID.c_str());
}

static void GetMemoryDebugInfo()
{
	MEMORYSTATUS mem;
	GlobalMemoryStatus(&mem);
	
	LOG->Info("Memory: %imb total, %imb swap (%imb swap avail)",
		mem.dwTotalPhys / 1048576, 
		mem.dwTotalPageFile / 1048576, 
		mem.dwAvailPageFile / 1048576);
}

static void GetDisplayDriverDebugInfo()
{
	CString sPrimaryDeviceName = GetPrimaryVideoName();
	
	if( sPrimaryDeviceName == "" )
		LOG->Info( "Primary display driver could not be determined." );
	else
		LOG->Info( "Primary display driver: %s", sPrimaryDeviceName.c_str() );

	for( int i=0; true; i++ )
	{
		VideoDriverInfo info;
		if( !GetVideoDriverInfo(i, info) )
			break;
		
		if( sPrimaryDeviceName == "" )	// failed to get primary display name (NT4)
		{
			LogVideoDriverInfo( info );
		}
		else if( info.sDescription == sPrimaryDeviceName )
		{
			LogVideoDriverInfo( info );
			break;
		}
	}
}

static CString wo_ssprintf( MMRESULT err, const char *fmt, ...)
{
	char buf[MAXERRORLENGTH];
	waveOutGetErrorText(err, buf, MAXERRORLENGTH);

    va_list	va;
    va_start(va, fmt);
    CString s = vssprintf( fmt, va );
    va_end(va);

	return s += ssprintf( "(%s)", buf );
}

static void GetWindowsVersionDebugInfo()
{
	/* Detect operating system. */
	OSVERSIONINFO ovi;
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&ovi))
	{
		LOG->Info("GetVersionEx failed!");
		return;
	}

	CString Ver = ssprintf("Windows %i.%i (", ovi.dwMajorVersion, ovi.dwMinorVersion);
	if(ovi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if(ovi.dwMinorVersion == 0)
			Ver += "Win95";
		else if(ovi.dwMinorVersion == 10)
			Ver += "Win98";
		else if(ovi.dwMinorVersion == 90)
			Ver += "WinME";
		else 
			Ver += "unknown 9x-based";
	}
	else if(ovi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if(ovi.dwMajorVersion == 4 && ovi.dwMinorVersion == 0)
			Ver += "WinNT 4.0";
		else if(ovi.dwMajorVersion == 5 && ovi.dwMinorVersion == 0)
			Ver += "Win2000";
		else if(ovi.dwMajorVersion == 5 && ovi.dwMinorVersion == 1)
			Ver += "WinXP";
		else
			Ver += "unknown NT-based";
	} else Ver += "???";

	Ver += ssprintf(") build %i [%s]", ovi.dwBuildNumber & 0xffff, ovi.szCSDVersion);
	LOG->Info("%s", Ver.c_str());
}

static void GetSoundDriverDebugInfo()
{
	int cnt = waveOutGetNumDevs();

	for(int i = 0; i < cnt; ++i)
	{
		WAVEOUTCAPS caps;
	
		MMRESULT ret = waveOutGetDevCaps(i, &caps, sizeof(caps));
		if(ret != MMSYSERR_NOERROR)
		{
			LOG->Info(wo_ssprintf(ret, "waveOutGetDevCaps(%i) failed", i));
			continue;
		}
		LOG->Info("Sound device %i: %s, %i.%i, MID %i, PID %i %s", i, caps.szPname,
			HIBYTE(caps.vDriverVersion),
			LOBYTE(caps.vDriverVersion),
			caps.wMid, caps.wPid,
			caps.dwSupport & WAVECAPS_SAMPLEACCURATE? "":"(INACCURATE)");
	}
}

#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

static CString FourCCToString(int fcc)
{
	char c[4];
	c[0] = char((fcc >> 0) & 0xFF);
	c[1] = char((fcc >> 8) & 0xFF);
	c[2] = char((fcc >> 16) & 0xFF);
	c[3] = char((fcc >> 24) & 0xFF);

	CString s;
	for( int i = 0; i < 4; ++i )
		s += clamp( c[i], '\x20', '\x7e' );

	return s;
}

static void CheckCodecVersion( CString codec, CString desc )
{
	if( !codec.CompareNoCase("DIVX") )
	{
		/* "DivX 5.0.5 Codec" */
		Regex GetDivXVersion;

		int major, minor, rev;
		if( sscanf( desc, "DivX %i.%i.%i", &major, &minor, &rev ) != 3 &&
			sscanf( desc, "DivX Pro %i.%i.%i", &major, &minor, &rev ) != 3 )
		{
			LOG->Warn( "Couldn't parse DivX version \"%s\"", desc.c_str() );
			return;
		}

		/* 5.0.0 through 5.0.4 are old and cause crashes. Warn. */
		if( major == 5 && minor == 0 && rev < 5 )
		{
			HOOKS->MessageBoxOK(
				ssprintf("The version of DivX installed, %i.%i.%i, is out of date and may\n"
				"cause instability.  Please upgrade to DivX 5.0.5 or newer, available at:\n"
				"\n"
				"http://www.divx.com/", major, minor, rev),
				desc );
			return;
		}
	}
}


static void GetVideoCodecDebugInfo()
{
	ICINFO info = { sizeof(ICINFO) };
	int i;
	LOG->Info("Video codecs:");
	CHECKPOINT;
	for(i=0; ICInfo(ICTYPE_VIDEO, i, &info); ++i)
	{
		CHECKPOINT;
		if( FourCCToString(info.fccHandler) == "ASV1" )
		{
			/* Broken. */
			LOG->Info("%i: %s: skipped", i, FourCCToString(info.fccHandler).c_str());
			continue;
		}

		LOG->Trace( "Scanning codec %s", FourCCToString(info.fccHandler).c_str() );
		CHECKPOINT;
		HIC hic;
		hic = ICOpen(info.fccType, info.fccHandler, ICMODE_DECOMPRESS);
		if(!hic)
		{
			LOG->Info("Couldn't open video codec %s",
				FourCCToString(info.fccHandler).c_str());
			continue;
		}

		CHECKPOINT;
		if (ICGetInfo(hic, &info, sizeof(ICINFO)))
		{
			CheckCodecVersion( FourCCToString(info.fccHandler), WStringToCString(info.szDescription) );
			CHECKPOINT;

			LOG->Info("    %s: %ls (%ls)",
				FourCCToString(info.fccHandler).c_str(), info.szName, info.szDescription);
		}
		else
			LOG->Info("ICGetInfo(%s) failed",
				FourCCToString(info.fccHandler).c_str());

		CHECKPOINT;
		ICClose(hic);
	}

	if(!i)
		LOG->Info("    None found");
}

void SearchForDebugInfo()
{
	GetWindowsVersionDebugInfo();
	GetMemoryDebugInfo();
	GetDisplayDriverDebugInfo();
	GetSoundDriverDebugInfo();
	GetVideoCodecDebugInfo();
}

/*
 * Copyright (c) 2003 by the person(s) listed below.  All rights reserved.
 *
 * Glenn Maynard
 */
