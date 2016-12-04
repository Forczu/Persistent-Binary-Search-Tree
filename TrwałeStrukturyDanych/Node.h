#pragma once

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
public:
	// pole zmiany
	ChangeType _changeType;
	int _changeTime;
	Node<Type> * _changeChild;
private:
	// pole drzewa
	Node<Type> * _rightChild;
	Type _value;
	Node<Type> * _leftChild;
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

	/// <summary>
	/// Zwraca lewe dziecko zgodnie z podana wersja.
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	Node<Type> * getLeftChild(int version) const
	{
		Node<Type> * left = _changeType == LeftChild && version >= _changeTime ? _changeChild : _leftChild;
		return left;
	}

	/// <summary>
	/// Zwraca prawe dziecko zgodnie z podana wersja.
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	Node<Type> * getRightChild(int version) const
	{
		Node<Type> * right = _changeType == RightChild && version >= _changeTime ? _changeChild : _rightChild;
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

	void setLeftChild(Node<Type> * child)
	{
		_leftChild = child;
	}

	void setRightChild(Node<Type> * child)
	{
		_rightChild = child;
	}
};