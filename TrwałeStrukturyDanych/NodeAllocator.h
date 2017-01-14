#pragma once
#include <cstddef>
#include "Node.h"

/// <summary>
/// Alokator dla wezlow drzewa o szablonowyn parametrze T.
/// </summary>
template <class T>
class NodeAllocator
{
	typedef Node<T> NodeValue;

	/// <summary>
	/// Liczba zaalokowanych wezlow
	/// </summary>
	int _nodeCounter;


	/// <summary>
	/// Suma zaalokowanej pamieci w bajtach
	/// </summary>
	std::size_t _totalSize;

public:
	/// <summary>
	/// Konstruktor klasy <see cref="NodeAllocator"/>.
	/// </summary>
	NodeAllocator() noexcept : _nodeCounter(0), _totalSize(0)
	{
	}

	/// <summary>
	/// Alokuje pamiec na wskazana liczbe wezlow
	/// </summary>
	/// <param name="n">Liczba wezlow.</param>
	/// <param name=""></param>
	/// <returns></returns>
	NodeValue * allocate(std::size_t n, void const * = 0)
	{
		auto size = n * sizeof(NodeValue);
		NodeValue * t = (NodeValue*)malloc(size);
		_totalSize += size;
		return t;
	}

	/// <summary>
	/// Zwalnia pamiec we wskazanym miejscu
	/// </summary>
	/// <param name="p">Wskaznik do wezla.</param>
	/// <param name=""></param>
	void deallocate(NodeValue * p, std::size_t = 0)
	{
		if (p)
		{
			_totalSize -= sizeof(*p);
			free(p);
		}
	}

	/// <summary>
	/// Uruchamia konstruktor podanego wezla
	/// </summary>
	/// <param name="p">Wskaznik do wezla.</param>
	/// <param name="value">Wartosc wezla.</param>
	void construct(NodeValue * p, T & value)
	{
		new ((void*)p) NodeValue(value);
		_nodeCounter += 1;
	}

	/// <summary>
	/// Uruchamia destruktor podanego wezla
	/// </summary>
	/// <param name="p">Wskaznik do wezla.</param>
	void destroy(NodeValue * p)
	{
		if (p)
		{
			p->~NodeValue();
			_nodeCounter -= 1;
		}
	}

	/// <summary>
	/// Zwraca liczbe zaalokowanych wezlow
	/// </summary>
	/// <returns></returns>
	int getNodeCount()
	{
		return _nodeCounter;
	}

};