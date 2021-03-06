#pragma once
#include <memory>

/// <summary>
/// Typ zmiany w wezle drzewa
/// </summary>
enum class ChangeType
{
	None, LeftChild, RightChild, Value
};

/// <summary>
/// Struktura reprezentujaca pojedynczy wezel w historii drzewa
/// </summary>
template<class Type>
class Node
{
	typedef Node<Type>* NodePtr;
public:
	template<class Type>	
	union ChangeField
	{
		NodePtr child;
		Type * value;
		ChangeField() : child(nullptr) { }
		ChangeField(ChangeField const & origin) {  };
		~ChangeField() {}
	};

private:
	// pole zmiany
	ChangeType _changeType;
	int _changeTime;
	ChangeField<Type> _change;
	// pole drzewa
	NodePtr _rightChild;
	Type * _value;
	NodePtr _leftChild;

public:
	Node() : _change()
	{
		init();
	}

	Node(Type & value) : _change()
	{
		init();
		setValue(&value);
	}

	~Node()
	{
		_change.child = nullptr;
		_rightChild = _leftChild = nullptr;
	}

	/// <summary>
	/// Inicjalizacja wezla domyslnymi wartosciami.
	/// </summary>
	void init()
	{
		// brak zmiany
		_changeType = ChangeType::None;
		_changeTime = 0;
		// wezel bez dzieci i z wartoscia
		_rightChild = _leftChild = nullptr;
	}

	/// <summary>
	/// Zwraca lewe dziecko zgodnie z podana wersja.
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	NodePtr getLeftChild(int version) const
	{
		NodePtr left = _changeType == ChangeType::LeftChild && version >= _changeTime ? _change.child : _leftChild;
		return left;
	}

	/// <summary>
	/// Zwraca prawe dziecko zgodnie z podana wersja.
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	NodePtr getRightChild(int version) const
	{
		NodePtr right = _changeType == ChangeType::RightChild && version >= _changeTime ? _change.child : _rightChild;
		return right;
	}

	/// <summary>
	/// Zwraca wartosc przechowywana przez wezel.
	/// </summary>
	/// <returns></returns>
	Type * getValue(int version)
	{
		Type * value = _changeType == ChangeType::Value && version >= _changeTime ? _change.value : _value;
		return value;
	}

	void setLeftChild(NodePtr child)
	{
		_leftChild = child;
	}

	void setRightChild(NodePtr child)
	{
		_rightChild = child;
	}

	void setValue(Type * value)
	{
		_value = value;
	}

	/// <summary>
	/// Ustawia zmiane w postaci zmiany wskaznika na dziecko.
	/// </summary>
	/// <param name="type">Typ zmiany.</param>
	/// <param name="child">Dziecko.</param>
	/// <param name="time">Wersja drzewa.</param>
	void setChange(ChangeType type, NodePtr child, int time)
	{
		_changeType = type;
		_changeTime = time;
		_change.child = child;
	}

	/// <summary>
	/// Ustawia zmiane w postaci zmiany wartosci w wezle.
	/// </summary>
	/// <param name="type">Typ zmiany.</param>
	/// <param name="value">Wartosc.</param>
	/// <param name="time">Wersja drzewa.</param>
	void setChange(ChangeType type, Type & value, int time)
	{
		_changeType = type;
		_changeTime = time;
		_change.value = &value;
	}

	ChangeType getChangeType()
	{
		return _changeType;
	}

	int getChangeTime()
	{
		return _changeTime;
	}

	ChangeField<Type> & getChange()
	{
		return _change;
	}
};
