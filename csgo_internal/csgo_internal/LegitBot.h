#pragma once

class CLegitBot
{
public:
	void Run();
	bool EntityIsValid( int i );

private:
	void GoToTarget();
	void FindTarget();
	void DropTarget();

	void Triggerbot();
	void RCS();

private:
	//int m_fbestfov;
	//int m_ibesttarget;
};
