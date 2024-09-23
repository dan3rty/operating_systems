#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	pid_t pid = fork();

	if (pid < 0)
	{
		throw std::runtime_error("fork failed");
	}

	if (pid == 0)
	{
		exit(0);
	}

	pid_t child_pid;

	std::cout << "Введите PID дочернего процесса: ";
	std::cin >> child_pid;

	int status;
	while (true)
	{
		if (waitpid(child_pid, &status, 0) == -1)
		{
			std::cerr << "Не удалось дождаться процесса с PID " << child_pid << ". Попробуйте снова." << std::endl;
			std::cout << "Введите PID дочернего процесса: ";
			std::cin >> child_pid;
		}
		else
		{
			std::cout << "Дочерний процесс с PID " << child_pid << " завершился." << std::endl;
				break;
		}
	}

	return 0;
}