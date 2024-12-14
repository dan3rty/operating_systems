#pragma once

class Child
{
public:
	static void Live();

private:
	static void SignalHandler(int signum);
};
