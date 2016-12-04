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
	std::shared_ptr<Node<UnqualifiedType>> itr;
	std::stack<std::shared_ptr<Node<UnqualifiedType>>> stack;
	int version;

	typedef std::shared_ptr<Node<Type>> NodePtr;

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
	PersistentTreeIterator(std::shared_ptr<Node<UnqualifiedType>> root, int version) : version(version)
	{
		if (root == nullptr)
		{
			itr = nullptr;
			return;
		}
		bool end = false;
		NodePtr currentNode = root;
		stack.push(root);
		while (!end)
		{
			NodePtr left(currentNode->getLeftChild(version));
			if (left != nullptr)
			{
				stack.push(left);
				currentNode = left;
			}
			else
			{
				itr = next(version);
				end = true;
			}
		}
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
		itr = next(version);
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
	Type & operator * ()
	{
		return itr->getValue();
	}

	/// <summary>
	/// Operator dostepu do przechowywanej wartosci
	/// </summary>
	/// <returns></returns>
	Type & operator -> ()
	{
		return itr->getValue();
	}

private:
	/// <summary>
	/// Zwraca kolejny wezek drzewa
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	NodePtr next(int version)
	{
		NodePtr node(stack.top());
		stack.pop();
		NodePtr result(node);
		NodePtr right(node->getRightChild(version));
		if (right != nullptr)
		{
			node = right;
			while (node != nullptr)
			{
				stack.push(node);
				NodePtr left(node->getLeftChild(version));
				node = left;
			}
		}
		return result;
	}

};