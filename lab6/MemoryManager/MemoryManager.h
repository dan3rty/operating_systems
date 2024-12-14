#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <stdexcept>

struct BlockHeader
{
	size_t size;
	BlockHeader* next;
};

class MemoryManager
{
public:
	MemoryManager(void* start, const size_t size)
	{
		if (reinterpret_cast<std::uintptr_t>(start) % alignof(std::max_align_t) != 0)
		{
			throw std::invalid_argument("Start address must be aligned to std::max_align_t");
		}

		m_freeList = static_cast<BlockHeader*>(start);
		m_freeList->size = size;
		m_freeList->next = nullptr;
	}

	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	void* Allocate(const size_t size, const size_t align = alignof(std::max_align_t)) noexcept
	{
		std::lock_guard lock(m_mutex);

		if (size == 0 || align == 0 || (align & (align - 1)) != 0 || align < sizeof(void*))
		{
			return nullptr;
		}

		BlockHeader** prev = &m_freeList;
		BlockHeader* current = m_freeList;

		while (current != nullptr)
		{
			constexpr auto headerSize = sizeof(BlockHeader);
			const uintptr_t addr = reinterpret_cast<uintptr_t>(current) + headerSize;
			const uintptr_t alignedAddr = (addr + align - 1) & ~(align - 1);

			const size_t offset = alignedAddr - reinterpret_cast<uintptr_t>(current);

			// устранить дублирование кода
			if (current->size >= offset + size)
			{
				if (current->size > offset + size + headerSize)
				{
					// assert((alignedAddr + size) & (alignof(BlockHeader) - 1) == 0);
					// header должен распологаться по выровненному адресу
					auto* newBlock = reinterpret_cast<BlockHeader*>(alignedAddr + size);
					newBlock->size = current->size - offset - size;
					newBlock->next = current->next;

					current->size = offset;
					current->next = newBlock;
				}

				*prev = current->next;
				auto* allocatedBlock = reinterpret_cast<BlockHeader*>(alignedAddr - headerSize);
				allocatedBlock->size = size + headerSize;

				return reinterpret_cast<void*>(alignedAddr);
			}

			prev = &current->next;
			current = current->next;
		}

		return nullptr;
	}

	void Free(void* addr) noexcept
	{
		std::lock_guard lock(m_mutex);

		if (!addr)
		{
			return;
		}
		// правильно определять размер header при освобождении

		auto* blockToFree = reinterpret_cast<BlockHeader*>(reinterpret_cast<uintptr_t>(addr) - sizeof(BlockHeader));

		blockToFree->next = m_freeList;
		m_freeList = blockToFree;

		MergeBlocks();
	}

private:
	BlockHeader* m_freeList = nullptr;
	std::mutex m_mutex;

	// ускорить слияние
	void MergeBlocks() const noexcept
	{
		BlockHeader* current = m_freeList;

		while (current && current->next)
		{
			const uintptr_t currentEnd = reinterpret_cast<uintptr_t>(current) + sizeof(BlockHeader) + current->size;
			auto nextStart = reinterpret_cast<uintptr_t>(current->next);

			if (currentEnd == nextStart)
			{
				current->size += sizeof(BlockHeader) + current->next->size;
				current->next = current->next->next;
			}
			else
			{
				current = current->next;
			}
		}
	}
};
