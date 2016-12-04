#pragma once
#include <memory>

/// <summary>
/// Typ zmiany w wezle drzewa
/// </summary>
enum ChangeType
{
	None, LeftChild, RightChild
};

/// <summary>
/// Struktura reprezentujaca pojedynczy wezel w historii drzewa
/// </summary>
template<class Type>
class Node
{
	typedef std::shared_ptr<Node<Type>> NodePtr;
public:
	// pole zmiany
	ChangeType _changeType;
	int _changeTime;
	NodePtr _changeChild;
private:
	// pole drzewa
	NodePtr _rightChild;
	Type _value;
	NodePtr _leftChild;

public:
	Node(Type value)
	{
		// brak zmiany
		_changeType = None;
		_changeTime = 0;
		_changeChild = nullptr;
		// wezel bez dzieci i z wartoscia
		_rightChild = _leftChild = nullptr;
		_value = value;
	}

	~Node()
	{
		_changeChild = _rightChild = _leftChild = nullptr;
	}

	/// <summary>
	/// Zwraca lewe dziecko zgodnie z podana wersja.
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	NodePtr getLeftChild(int version) const
	{
		NodePtr left = _changeType == LeftChild && version >= _changeTime ? _changeChild : _leftChild;
		return left;
	}

	/// <summary>
	/// Zwraca prawe dziecko zgodnie z podana wersja.
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	NodePtr getRightChild(int version) const
	{
		NodePtr right = _changeType == RightChild && version >= _changeTime ? _changeChild : _rightChild;
		return right;
	}

	/// <summary>
	/// Zwraca wartosc przechowywana przez wezel.
	/// </summary>
	/// <returns></returns>
	Type & getValue()
	{
		return _value;
	}

	void setLeftChild(NodePtr child)
	{
		_leftChild = child;
	}

	void setRightChild(NodePtr child)
	{
		_rightChild = child;
	}
};