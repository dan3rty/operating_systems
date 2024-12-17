#include "HandleDesc.h"
#include <iostream>
#include <string>

class FileSystemMonitor
{
public:
	explicit FileSystemMonitor(const std::string& path)
		: m_directoryPath(path)
		, m_handleDesc(CreateFileA(
			  path.c_str(),
			  FILE_LIST_DIRECTORY,
			  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			  nullptr,
			  OPEN_EXISTING,
			  FILE_FLAG_BACKUP_SEMANTICS,
			  nullptr))
	{
		if (!m_handleDesc.isValid() && !IsDirectoryValid(path))
		{
			throw std::runtime_error("Error opening directory: " + std::to_string(GetLastError()));
		}
	}

	void StartMonitoring()
	{
		std::cout << "Monitoring changes in directory: " << m_directoryPath << std::endl;
		std::cout << "Press CTRL+C to exit the program.\n"
				  << std::endl;

		while (true)
		{
			if (ReadDirectoryChangesW(
					m_handleDesc.GetHandle(),
					m_buffer,
					m_bufferSize,
					false,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_ATTRIBUTES,
					&m_bytesReturned,
					nullptr,
					nullptr))
			{
				ProcessNotifications();
			}
			else
			{
				throw std::runtime_error("Error reading changes: " + std::to_string(GetLastError()));
			}
		}
	}

private:
	static bool IsDirectoryValid(const std::string& path)
	{
		DWORD fileAttr = GetFileAttributesA(path.c_str());

		return (fileAttr == INVALID_FILE_ATTRIBUTES || !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
	}

	void ProcessNotifications()
	{
		auto* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer);

		do
		{
			std::wstring fileName(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
			std::wcout << L"Change detected: " << fileName << L" - ";

			switch (fni->Action)
			{
			case FILE_ACTION_ADDED:
				std::wcout << L"File/Directory added.";
				break;
			case FILE_ACTION_REMOVED:
				std::wcout << L"File/Directory removed.";
				break;
			case FILE_ACTION_MODIFIED:
				std::wcout << L"File/Directory modified.";
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				std::wcout << L"Renamed (old name).";
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				std::wcout << L"Renamed (new name).";
				break;
			default:
				std::wcout << L"Unknown change.";
			}
			std::wcout << std::endl;

			if (fni->NextEntryOffset)
			{
				fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
					reinterpret_cast<BYTE*>(fni) + fni->NextEntryOffset);
			}
			else
			{
				fni = nullptr;
			}

		} while (fni);
	}

	static constexpr DWORD m_bufferSize = 1024;
	std::string m_directoryPath;
	HandleDesc m_handleDesc;
	BYTE m_buffer[m_bufferSize]{};
	DWORD m_bytesReturned{};
};