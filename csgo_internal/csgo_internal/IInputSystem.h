#pragma once

class IInputSystem
{
public:
	void EnableInput( bool bEnable );
	void ResetInputState();
	bool IsButtonDown( ButtonCode_t code );
	void GetCursorPosition( int* m_pX, int* m_pY );
	ButtonCode_t VirtualKeyToButtonCode( int nVirtualKey );
	int ButtonCodeToVirtualKey( ButtonCode_t code );
};
