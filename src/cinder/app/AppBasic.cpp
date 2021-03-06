/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/app/AppBasic.h"
#include "cinder/app/Renderer.h"

#if defined( CINDER_MAC )
#	import "AppImplCocoaBasic.h"
#elif defined( CINDER_MSW )
#	include "cinder/app/AppImplMswBasic.h"
#endif

namespace cinder { namespace app {

// Global singleton
AppBasic*	AppBasic::sInstance;

AppBasic::AppBasic()
	: App()
{
	mImpl = 0;
}

AppBasic::~AppBasic()
{
#if defined( CINDER_MAC )
	[mImpl release];
#else
	delete mImpl;
#endif
}

void AppBasic::launch( const char *title, int argc, char * const argv[] )
{
	// We should parse args here eventually; on Windows we'll use ::GetCommandLineW & CommandLineToArgvW
	mSettings.setTitle( title );

	prepareSettings( &mSettings );
	if( ! mSettings.isPrepared() ) {
		return;
	}

#if defined( CINDER_COCOA )
	mImpl = [[::AppImplCocoaBasic alloc] init:this];
#else
	mImpl = new AppImplMswBasic( this );	
	mImpl->run();
#endif
// NOTHING AFTER THIS LINE RUNS
}

#if defined( CINDER_MAC )
void AppBasic::privateSetImpl__( AppImplCocoaBasic *aImpl )
{
	mImpl = aImpl;
}
#endif

int AppBasic::getWindowWidth() const
{
#if defined( CINDER_COCOA )
	return [mImpl getWindowWidth];
#elif defined( CINDER_MSW )
	return mImpl->getWindowWidth();
#endif
}

int AppBasic::getWindowHeight() const
{
#if defined( CINDER_COCOA )
	return [mImpl getWindowHeight];
#elif defined( CINDER_MSW )
	return mImpl->getWindowHeight();
#endif
}

void AppBasic::setWindowWidth( int windowWidth )
{
#if defined( CINDER_COCOA )
	[mImpl setWindowWidth:windowWidth];
#elif defined( CINDER_MSW )
	mImpl->setWindowWidth( windowWidth );
#endif
}

void AppBasic::setWindowHeight( int windowHeight )
{
#if defined( CINDER_COCOA )
	[mImpl setWindowHeight:windowHeight];
#elif defined( CINDER_MSW )
	mImpl->setWindowHeight( windowHeight );
#endif
}

void AppBasic::setWindowSize( int windowWidth, int windowHeight )
{
#if defined( CINDER_COCOA )
	[mImpl setWindowSizeWithWidth:windowWidth height:windowHeight];
#elif defined( CINDER_MSW )
	mImpl->setWindowSize( windowWidth, windowHeight );
#endif
}

float AppBasic::getFrameRate() const
{
#if defined( CINDER_COCOA )
	return [mImpl getFrameRate];
#elif defined( CINDER_MSW )
	return mImpl->getFrameRate();
#endif
}

void AppBasic::setFrameRate( float aFrameRate )
{
#if defined( CINDER_COCOA )
	[mImpl setFrameRate:aFrameRate];
#elif defined( CINDER_MSW )
	mImpl->setFrameRate( aFrameRate );
#endif
}

bool AppBasic::isFullScreen() const
{
#if defined( CINDER_COCOA )
	return [mImpl isFullScreen];
#elif defined( CINDER_MSW )
	return mImpl->isFullScreen();
#endif
}

void AppBasic::setFullScreen( bool aFullScreen )
{
	if( aFullScreen != isFullScreen() ) {
#if defined( CINDER_COCOA )
		if( aFullScreen )
			[mImpl enterFullScreen];
		else
			[mImpl exitFullScreen];
#else
		mImpl->toggleFullScreen();
#endif
	}
}

Vec2i AppBasic::getMousePos() const
{
#if defined( CINDER_MAC )
	CGPoint p = [mImpl mouseLocation];
	return Vec2i( (int)p.x, getWindowHeight() - (int)p.y );
#elif defined( CINDER_MSW )
	return mImpl->mouseLocation();
#else
	return Vec2i( -1, -1 );
#endif
}

std::string AppBasic::getAppPath()
{
#if defined( CINDER_COCOA )
	return [mImpl getAppPath];
#elif defined( CINDER_MSW )
	return AppImplMsw::getAppPath();
#endif
}

void AppBasic::hideCursor()
{
#if defined( CINDER_MAC )
	[NSCursor hide];
#elif defined( CINDER_MSW )
	AppImplMsw::hideCursor();
#endif
}

void AppBasic::showCursor()
{
#if defined( CINDER_MAC )
	[NSCursor unhide];
#elif defined( CINDER_MSW )
	AppImplMsw::showCursor();
#endif
}

void AppBasic::quit()
{
#if defined( CINDER_COCOA )
	[mImpl quit];
#else
	mImpl->quit();
#endif
}

#if defined( CINDER_MAC )
const Display& AppBasic::getDisplay()
{
	if( mImpl )
		return *[mImpl getDisplay];
	else
		return *(Display::getMainDisplay().get());
}

#else

const Display& AppBasic::getDisplay()
{
	if( mImpl )
		return *(mImpl->getDisplay());
	else
		return *(Display::getMainDisplay().get());
}

#endif

void AppBasic::privateResize__( int width, int height )
{	
#if defined( CINDER_MAC )
	[mImpl handleResizeWithWidth:width height:height];
#endif

	App::privateResize__( width, height );
}

void AppBasic::privateTouchesBegan__( const TouchEvent &event )
{	
	touchesBegan( event );
}

void AppBasic::privateTouchesMoved__( const TouchEvent &event )
{	
	touchesMoved( event );
}

void AppBasic::privateTouchesEnded__( const TouchEvent &event )
{	
	touchesEnded( event );
}

//////////////////////////////////////////////////////////////////////////////////////////////
// AppBasic::Settings
AppBasic::Settings::Settings()
	: App::Settings()
{
	mFullScreenSizeX = mFullScreenSizeY = 0;
	mDisplay = Display::getMainDisplay().get();
	mEnableMultiTouch = false;
#if defined( CINDER_MAC )
	mEnableSecondaryDisplayBlanking = true; 
#endif
}

void AppBasic::Settings::setShouldQuit( bool aShouldQuit )
{
	mShouldQuit = aShouldQuit;
}

void AppBasic::Settings::setFullScreenSize( int aFullScreenSizeX, int aFullScreenSizeY )
{
	mFullScreenSizeX = aFullScreenSizeX;
	mFullScreenSizeY = aFullScreenSizeY;
}

void AppBasic::Settings::setFullScreen( bool aFullScreen )
{
	mFullScreen = aFullScreen;
}

void AppBasic::Settings::setResizable( bool aResizable )
{
	mResizable = aResizable;
}

void AppBasic::Settings::setDisplay( shared_ptr<Display> aDisplay )
{
	mDisplay = aDisplay.get();
}

} } // namespace cinder::app