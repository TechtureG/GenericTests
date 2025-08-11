#pragma once
#include "Core.h"
#include "DirectXCore.h"
#include <XInput.h>
#pragma comment(lib, "XInput.lib")

class GamePadController
{
public:
	GamePadController();
	void AreControllersEnabled(bool controllersEnabled[4]);
	void ProcessGameController();

private:
	DWORD		 _lastPacketNumber;
	bool		 _firstTime;

	DWORD		_leftThumbDeadZoneSquared;
	DWORD		_rightThumbDeadZoneSquared;
};

