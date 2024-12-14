#pragma once
#include <functional>
#include <mutex>
#include <unordered_set>

template <typename T, typename H = std::hash<T>, typename Comp = std::equal_to<T>>
class ThreadSafeSet
{
public:
	void Insert(const T& value)
	{
		std::lock_guard lock(m_mutex);
		m_set.insert(value);
	}

	bool Contains(const T& value)
	{
		std::lock_guard lock(m_mutex);
		return m_set.contains(value);
	}

	std::vector<T> GetElements()
	{
		std::lock_guard lock(m_mutex);
		return std::vector<T>(m_set.begin(), m_set.end());
	}

private:
	std::unordered_set<T, H, Comp> m_set;
	mutable std::mutex m_mutex{};
};