#pragma once
#include <cstddef>
#include "Node.h"

template <class T>
class NodeAllocator
{
	typedef Node<T> NodeValue;

	int _nodeCounter;
	std::size_t _totalSize;

public:
	NodeAllocator() noexcept : _nodeCounter(0), _totalSize(0)
	{
	}

	NodeValue * allocate(std::size_t n, void const * = 0)
	{
		auto size = n * sizeof(NodeValue);
		NodeValue * t = (NodeValue*)malloc(size);
		_totalSize += size;
		return t;
	}

	void deallocate(NodeValue * p, std::size_t = 0)
	{
		if (p)
		{
			_totalSize -= sizeof(*p);
			free(p);
		}
	}

	void construct(NodeValue * p, T & value)
	{
		new ((void*)p) NodeValue(value);
		_nodeCounter += 1;
	}

	void destroy(NodeValue * p)
	{
		if (p)
		{
			p->~NodeValue();
			_nodeCounter -= 1;
		}
	}

	int getNodeCount()
	{
		return _nodeCounter;
	}

};