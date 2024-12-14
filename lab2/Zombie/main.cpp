#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	const pid_t pid = fork();

	if (pid < 0)
	{
		std::cerr << "fork failed" << std::endl;
		return EXIT_FAILURE;
	}

	if (pid == 0)
	{
		return EXIT_SUCCESS;
	}

	pid_t childPid;

	std::cout << "Введите PID дочернего процесса: ";
	std::cin >> childPid;

	int status;
	while (true)
	{
		// выяснить, что будет если в waitpid прокинуть -1
		if (waitpid(childPid, &status, 0) == -1)
		{
			std::cerr << "Не удалось дождаться процесса с PID " << childPid << ". Попробуйте снова." << std::endl;
			std::cout << "Введите PID дочернего процесса: ";
			std::cin >> childPid;
		}
		else
		{
			std::cout << "Дочерний процесс с PID " << childPid << " завершился." << std::endl;
			return EXIT_SUCCESS;
		}
	}
}