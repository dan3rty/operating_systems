#include <windows.h>

class HandleDesc
{
public:
	explicit HandleDesc(HANDLE h)
		: m_handle(h)
	{
	}

	~HandleDesc()
	{
		if (m_handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_handle);
		}
	}

	[[nodiscard]] HANDLE GetHandle() const
	{
		return m_handle;
	}

	[[nodiscard]] bool isValid() const
	{
		return m_handle != INVALID_HANDLE_VALUE;
	}

private:
	HANDLE m_handle;
};