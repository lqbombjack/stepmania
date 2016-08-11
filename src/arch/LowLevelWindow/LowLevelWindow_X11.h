/* LowLevelWindow_X11 - OpenGL GLX window driver. */

#ifndef LOW_LEVEL_WINDOW_X11_H
#define LOW_LEVEL_WINDOW_X11_H

#include "RageDisplay.h" // VideoModeParams
#include "LowLevelWindow.h"

class LowLevelWindow_X11 : public LowLevelWindow
{
public:
	LowLevelWindow_X11();
	~LowLevelWindow_X11();

	void *GetProcAddress(std::string s);
	std::string TryVideoMode(const VideoModeParams &p, bool &bNewDeviceOut);
	void LogDebugInformation() const;
	bool IsSoftwareRenderer( std::string &sError );
	void SwapBuffers();

	const ActualVideoModeParams GetActualVideoModeParams() const { return CurrentParams; }

	void GetDisplaySpecs(DisplaySpecs &out) const;

	bool SupportsFullscreenBorderlessWindow() const;

private:
	void RestoreOutputConfig();

	bool m_bWasWindowed;
	ActualVideoModeParams CurrentParams;
};

#ifdef ARCH_LOW_LEVEL_WINDOW
#error "More than one LowLevelWindow selected!"
#endif
#define ARCH_LOW_LEVEL_WINDOW LowLevelWindow_X11

#endif

/*
 * (c) 2005 Ben Anderson
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
