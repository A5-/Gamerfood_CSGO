#pragma once

class CInput
{
public:
	CInput()
	{};

	void GetClicked();
	bool Holding( int, int, int, int );
	bool Hovering( int, int, int, int );
	bool Clicked( int, int, int, int );
private:
	bool bClicked, bMouseD;
};

extern CInput* Input;
