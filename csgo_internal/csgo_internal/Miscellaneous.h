#pragma once

class CMiscellaneous
{
public:
	void Initialize();
	void Run();
	void Think();
	const char* ReadInsult();
	void Bunnyhop();
	void AutoStrafe();
	void AchievementSpam();
	void ChangeName( const char* name );
	void Chatspam();
	void ReadChatspam( const char* fileName );
	void AirStuck();
	void Panic();
private:

private:
	bool m_bInitialized = false;
	std::vector< std::string > m_spamlines;
	std::vector< std::string > m_insults;
};
