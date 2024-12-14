#pragma once
#include <csignal>
#include <list>

class ChildKiller
{
public:
	void SpawnChild();

	void KillChild();

	void KillAllChildren();

	~ChildKiller();

private:
	std::list<pid_t> m_childProcesses;
};
