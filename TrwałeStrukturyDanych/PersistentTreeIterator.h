#pragma once
#include <iterator>
#include <stack>
#include "Node.h"

/// <summary>
/// Iterator typu forward, sluzacy do przechodzenia przez cale drzewo poszukiwan binarnych we wskazanej wersji.
/// </summary>
template<class Type, class UnqualifiedType = std::remove_cv_t<Type>>
class PersistentTreeIterator : public std::iterator<std::forward_iterator_tag, UnqualifiedType, std::ptrdiff_t, Type*, Type&>
{
	Node<UnqualifiedType> * itr;
	std::stack<Node<UnqualifiedType>*> stack;
	int version;

public:

	/// <summary>
	/// Domyslny konstruktor, rownoznaczny koncowi kolekcji
	/// </summary>
	/// <returns></returns>
	PersistentTreeIterator() : itr(nullptr)
	{
	}

	/// <summary>
	/// Konstruktor przyjmujacy korzen, rownoznaczy z poczatkiem kolekcji
	/// </summary>
	/// <param name="root">Korzen drzewa poszukiwan.</param>
	/// <param name="version">Wersja po ktorej nalezy przeszukiwac.</param>
	PersistentTreeIterator(Node<UnqualifiedType> * root, int version) : version(version)
	{
		bool end = false;
		Node<Type> * currentNode = root;
		stack.push(root);
		while (!end)
		{
			Node<Type> * left = currentNode->getLeftChild(version);
			if (left != nullptr)
			{
				stack.push(left);
				currentNode = left;
			}
			else
			{
				end = true;
			}
		}
		itr = stack.top();
		stack.pop();
	}

	/// <summary>
	/// Preinkrementacja iteratora
	/// </summary>
	/// <returns></returns>
	PersistentTreeIterator& operator ++ ()
	{
		if (stack.empty())
		{
			itr = nullptr;
			return *this;
		}
		Node<Type> * node = stack.top();
		stack.pop();
		itr = node;
		Node<Type> * right = node->getRightChild(version);
		if (right != nullptr)
		{
			node = right;
			while (node != nullptr)
			{
				stack.push(node);
				Node<Type> * left = node->getLeftChild(version);
				node = left;
			}
		}
		return *this;
	}

	/// <summary>
	/// Postinkrementacja iteratora
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	PersistentTreeIterator operator ++ (int)
	{
		PersistentTreeIterator tmp(*this);
		operator++();
		return tmp;
	}

	/// <summary>
	/// Operator rownosci
	/// </summary>
	/// <param name="rhs">Iterator do porownania.</param>
	/// <returns></returns>
	template<class OtherType>
	bool operator == (PersistentTreeIterator<OtherType> const & rhs) const
	{
		if (itr == nullptr && rhs.itr == nullptr)
			return true;
		if ((itr != nullptr && rhs.itr == nullptr) || (itr == nullptr && rhs.itr != nullptr))
			return false;
		return itr->getValue() == rhs.itr->getValue();
	}

	/// <summary>
	/// Operator nierownosci
	/// </summary>
	/// <param name="rhs">Iterator do porownania.</param>
	/// <returns></returns>
	template<class OtherType>
	bool operator != (PersistentTreeIterator<OtherType> const & rhs) const
	{
		return !(*this == rhs);
	}

	/// <summary>
	/// Operator dereferencji
	/// </summary>
	/// <returns></returns>
	Type & operator * () const
	{
		return itr->getValue();
	}

	/// <summary>
	/// Operator dostepu do przechowywanej wartosci
	/// </summary>
	/// <returns></returns>
	Type & operator -> () const
	{
		return itr->getValue();
	}
};