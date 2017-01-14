#pragma once
#include <iterator>
#include <stack>
#include "Node.h"
#include "PersistentTree.h"
#include <vector>

/// <summary>
/// Iterator typu forward, sluzacy do przechodzenia przez cale drzewo poszukiwan binarnych we wskazanej wersji.
/// </summary>
template<class Type, class UnqualifiedType = std::remove_cv_t<Type>>
class PersistentTreeIterator : public std::iterator<std::forward_iterator_tag, UnqualifiedType, std::ptrdiff_t, Type*, Type&>
{
	typedef Node<UnqualifiedType>* NodePtrType;

	/// <summary>
	/// Stos wezlow po ktorych nalezy iterowac.
	/// </summary>
	std::stack<NodePtrType, std::vector<NodePtrType>> stack;
	int version;

	typedef Node<Type>* NodePtr;

public:

	/// <summary>
	/// Domyslny konstruktor, rownoznaczny koncowi kolekcji
	/// </summary>
	/// <returns></returns>
	PersistentTreeIterator()
	{
	}

	/// <summary>
	/// Konstruktor przyjmujacy korzen, rownoznaczy z poczatkiem kolekcji
	/// </summary>
	/// <param name="root">Korzen drzewa poszukiwan.</param>
	/// <param name="version">Wersja po ktorej nalezy przeszukiwac.</param>
	PersistentTreeIterator(NodePtrType root, int version) : version(version)
	{
		if (root == nullptr)
			return;
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
				end = true;
			}
		}
	}

	template<class OrderFunctor>
	PersistentTreeIterator(NodePtrType node, NodePtrType root, OrderFunctor orderFunctor, int version) : version(version)
	{
		if (root == nullptr || node == nullptr)
			return;
		bool found = false;
		NodePtr currentNode = root;
		Type value = node->getValue(version);
		while (!found)
		{
			Type currNodeValue = currentNode->getValue(version);
			if (orderFunctor(value, currNodeValue))
			{
				stack.push(currentNode);
				currentNode = currentNode->getLeftChild(version);
			}
			else if (orderFunctor(currNodeValue, value))
			{
				currentNode = currentNode->getRightChild(version);
			}
			else
			{
				stack.push(currentNode);
				found = true;
			}
		}
	}

	/// <summary>
	/// Preinkrementacja iteratora
	/// </summary>
	/// <returns></returns>
	PersistentTreeIterator& operator ++ ()
	{
		if (!stack.empty())
			next(version);
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
		if (stack.empty() && rhs.stack.empty())
			return true;
		if ((!stack.empty() && rhs.stack.empty()) || (stack.empty() && !rhs.stack.empty()))
			return false;
		return stack.top()->getValue(version) == rhs.stack.top()->getValue(version);
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
	Type operator * ()
	{
		return stack.top()->getValue(version);
	}

	/// <summary>
	/// Zwraca wezel drzewa.
	/// </summary>
	/// <returns></returns>
	NodePtr getNode()
	{
		return stack.top();
	}

private:
	/// <summary>
	/// Zwraca kolejny wezel drzewa
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	NodePtr next(int version)
	{
		NodePtr node(stack.top());
		stack.pop();
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
			return node;
		}
		return nullptr;
	}
};