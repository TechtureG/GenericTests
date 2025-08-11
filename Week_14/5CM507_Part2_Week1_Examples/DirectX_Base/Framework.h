#pragma once
#include "Core.h"

class Framework
{
public:
	Framework();
	Framework(unsigned int width, unsigned int height);
	virtual ~Framework();

	int Run(HINSTANCE hInstance, int nCmdShow);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Simple accessors that can be used to retrieve key pieces of informaiton. Note
	// the use of 'inline' which tells the compiler that it can generate the machine code
	// for these functions inline where it is used instead of a call to the function. This
	// tends not to happen in debug builds, but will happen in release builds.

	inline unsigned int GetWindowWidth() const { return _width; }
	inline unsigned int GetWindowHeight() const { return _height; }
	inline HWND GetHWnd() const { return _hWnd; }

	// Initialise the application.  Called after the window has been
	// created, but before the main loop starts
	//
	// Return false if the application cannot be initialised.
	virtual bool Initialise() {	return true; }

	// Perform any updates to the structures that will be used
	// to render the window (i.e. transformation matrices, etc).
	virtual void Update() {}

	// Render the contents of the window. 
	virtual void Render() {};

	// Perform any application shutdown or cleanup that is needed
	virtual void Shutdown() {}

	// Handlers for Windows messages. If you need more, add them
	// here and call them from MsgProc. The only one we will need to handle 
	// is WM_SIZE later in the module, but this provides an example of how other
	// handlers could be written if required.
	virtual void OnResize(WPARAM wParam) {}

private:
	HINSTANCE		_hInstance;
	HWND			_hWnd;
	unsigned int	_width;
	unsigned int	_height;

	// Used in timing loop
	double			_timeSpan;

	bool InitialiseMainWindow(int nCmdShow);
	int MainLoop();
};

