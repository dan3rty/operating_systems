#pragma once
#include <stdexcept>
#include <sys/inotify.h>
#include <unistd.h>

class InotifyFileDesc
{
public:
	// грохнуть копирвание и присвивание
	InotifyFileDesc()
	{
		m_fileDesc = inotify_init();
		if (m_fileDesc < 0)
		{
			throw std::runtime_error("Ошибка инициализации inotify");
		}
	}

	~InotifyFileDesc()
	{
		if (m_fileDesc >= 0)
		{
			close(m_fileDesc);
		}
	}

	[[nodiscard]] int GetFileDesc() const
	{
		return m_fileDesc;
	}

private:
	int m_fileDesc;
};