#pragma once
#include "InotifyFileDesc.h"
#include <iostream>
#include <linux/limits.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (EVENT_SIZE + NAME_MAX + 1)

// разделить хранение дескриптора наблюдения и логику обработки наблюдений
class FileSystemMonitor
{
public:
	// грохнуть копирование и присваивание
	// можно порождение дескриптора делегировать на файловы дескриптор т.к. низкоуровневую функцию пониже сунуть бы
	explicit FileSystemMonitor(const std::string& path)
	{
		m_watchDesc = inotify_add_watch(m_fileDesc.GetFileDesc(), path.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_ATTRIB);
		if (m_watchDesc < 0)
		{
			throw std::runtime_error("Ошибка добавления наблюдения для пути: " + path);
		}
	}

	~FileSystemMonitor()
	{
		if (m_watchDesc >= 0)
		{
			inotify_rm_watch(m_fileDesc.GetFileDesc(), m_watchDesc);
		}
	}

	void StartMonitoring() const
	{
		char buffer[BUF_LEN];

		while (true)
		{
			// инкапсулировать работу с чтением поглубже
			const long length = read(m_fileDesc.GetFileDesc(), buffer, BUF_LEN);
			if (length < 0)
			{
				std::cerr << "Ошибка чтения" << std::endl;
				break;
			}

			size_t i = 0;
			while (i < length)
			{
				const auto* event = reinterpret_cast<struct inotify_event*>(&buffer[i]);
				PrintEvent(event);
				i += EVENT_SIZE + event->len;
			}
		}
	}

private:
	static void PrintEvent(const inotify_event* event)
	{
		std::cout << "Файл: " << event->name << ", Событие: ";

		if (event->mask & IN_CREATE)
		{
			std::cout << "создан";
		}
		else if (event->mask & IN_DELETE)
		{
			std::cout << "удален";
		}
		else if (event->mask & IN_MODIFY)
		{
			std::cout << "изменен";
		}
		else if (event->mask & IN_ATTRIB)
		{
			std::cout << "атрибуты изменены";
		}

		std::cout << std::endl;
	}

	InotifyFileDesc m_fileDesc;
	int m_watchDesc;
};