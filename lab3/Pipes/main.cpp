#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>

class Pipe
{
public:
	Pipe()
	{
		if (pipe(fds) == -1)
		{
			throw std::system_error(errno, std::generic_category());
		}
	}

	~Pipe()
	{
		close();
	}

	[[nodiscard]] int read_fd() const { return fds[0]; }
	[[nodiscard]] int write_fd() const { return fds[1]; }

	void close_read()
	{
		close();
		fds[0] = -1;
	}

	void close_write()
	{
		close();
		fds[1] = -1;
	}

private:
	int fds[2]{};

	void close()
	{
		if (fds[0] != -1)
		{
			::close(fds[0]);
			fds[0] = -1;
		}
		if (fds[1] != -1)
		{
			::close(fds[1]);
			fds[1] = -1;
		}
	}
};

void child_process(int read_fd, int write_fd)
{
	close(write_fd);

	char buffer[256];
	while (true)
	{
		ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer));
		if (bytes_read <= 0)
		{
			break;
		}

		buffer[bytes_read] = '\0';
		std::string command(buffer);
		std::cout << "lol: \"" << command << "\"" << std::endl;

		if (command.substr(0, 4) == "add ")
		{
			std::istringstream iss(command.substr(4));
			int number;
			int sum = 0;
			while (iss >> number)
			{
				sum += number;
			}
			std::ostringstream oss;
			oss << "sum is " << sum << "\n";
			write(write_fd, oss.str().c_str(), oss.str().size());
		}
		else if (command.substr(0, 13) == "longest_word ")
		{
			std::string filename = command.substr(13);
			std::ifstream file(filename);
			if (!file.is_open())
			{
				write(write_fd, "File not found\n", 16);
				continue;
			}

			std::string longest_word;
			std::string word;
			while (file >> word)
			{
				if (word.length() > longest_word.length())
				{
					longest_word = word;
				}
			}
			write(write_fd, longest_word.c_str(), longest_word.size());
			write(write_fd, "\n", 1);
		}
		else if (command == "exit\n")
		{
			break;
		}
	}

	close(read_fd);
}

int main()
{
	const Pipe pipe_to_child;
	const Pipe pipe_from_child;

	const pid_t pid = fork();

	if (pid == -1)
	{
		perror("Fork failed");
		return EXIT_FAILURE;
	}
	if (pid == 0)
	{
		child_process(pipe_to_child.read_fd(), pipe_from_child.write_fd());
		exit(0);
	}

	close(pipe_to_child.read_fd());
	close(pipe_from_child.write_fd());

	std::string command;

	while (true)
	{
		std::cout << "> ";
		std::getline(std::cin, command);

		write(pipe_to_child.write_fd(), command.c_str(), command.size());

		if (command == "exit")
			break;

		char buffer[256];
		const ssize_t bytes_read = read(pipe_from_child.read_fd(), buffer, sizeof(buffer));

		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			std::cout << buffer;
		}
	}

	close(pipe_to_child.write_fd());
	close(pipe_from_child.read_fd());

	wait(nullptr);

	return EXIT_SUCCESS;
}