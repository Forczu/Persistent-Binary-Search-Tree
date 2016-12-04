#pragma once
#include "PersistentTreeIterator.h"
#include "Node.h"
#include <map>

/// <summary>
/// Klasa reprezentujaca trwale drzewo poszukiwan binarnych.
/// Zastosowany algorytm to metoda Sleatora, Tarjana i innych
/// Parametr szablonowy Type okresla typ danych, jaki przechowywany w drzewie
/// </summary>
template<class Type>
class PersistentTree
{	
	/// <summary>
	/// Aktualna wersja drzewa. Zaczyna sie od jedynki, kazda nowa wersja skutkuje inkrementacja tej wartosci
	/// </summary>
	int _version;

	/// <summary>
	/// Punkty wejscia do drzewa. Klucz oznacza numer historii, wartoscia jest wskaznik na korzen
	/// </summary>
	std::map<int, Node<Type>*> _root;

public:
	typedef PersistentTreeIterator<Type> iterator;
	typedef PersistentTreeIterator<const Type> const_iterator;

	/// <summary>
	/// Tworzy nowe, puste drzewo bez historii.
	/// </summary>
	PersistentTree() : _version(0)
	{
	}

	/// <summary>
	/// Zwraca iterator na poczatek aktualnego drzewa
	/// </summary>
	/// <param name="version">Wersja drzewa, po ktorej nalezy iterowac. Zero oznacza wersje aktualna</param>
	/// <returns></returns>
	iterator begin(int version = 0) const
	{
		if (version == 0)
			version = getCurrentVersion();
		iterator it(_root.at(version), version);
		return it;
	}
	
	/// <summary>
	/// Usuwa zawartosc drzewa wraz z historia
	/// </summary>
	void clear()
	{

	}
	
	/// <summary>
	/// Zlicza liczba elementow we wskazanej wersji drzewa.
	/// </summary>
	/// <param name="version">Wersja drzewa, po ktorej nalezy iterowac. Zero oznacza wersje aktualna</param>
	/// <returns></returns>
	int count(int version = 0) const
	{
		if (version == 0)
			version = getCurrentVersion();
		int count = 0;
		for (iterator it = begin(version); it != end(); ++it, ++count) { }
		return count;
	}

	/// <summary>
	/// Zwraca koniec aktualnego drzewa
	/// </summary>
	/// <param name="version">Wersja drzewa, po ktorej nalezy iterowac. Zero oznacza wersje aktualna</param>
	/// <returns></returns>
	iterator end(int version = 0) const
	{
		if (version == 0)
			version = getCurrentVersion();
		iterator it;
		return it;
	}
	
	/// <summary>
	/// Usuwa element o podanej wartosci z drzewa. Skutkuje utworzeniem nowej wersji drzewa.
	/// </summary>
	/// <param name="value">Wartosc do usuniêcia.</param>
	void erase(Type value)
	{

	}
	
	/// <summary>
	/// Wyszukuje podana wartosc w drzewie o wskazanej wersji. Zero oznacza wersje aktualna drzewa.
	/// Jezeli wartosci nie ma w drzewie o podanej wersji, zwracany jest iterator na koniec drzewa.
	/// </summary>
	/// <param name="value">Wartosc do wyszukania.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns></returns>
	iterator find(Type value, int version = 0) const
	{
		if (version == 0)
			version = getCurrentVersion();
		bool found = false;
		Node<Type> * currentNode = _root.at(version);
		while (!found)
		{
			if (value < currentNode->getValue())
				currentNode = currentNode->getLeftChild(version);
			else if (value > currentNode->getValue())
				currentNode = currentNode->getRightChild(version);
			else
				found = true;
			if (currentNode == nullptr)
				found = true;
		}
		return iterator(currentNode, version);
	}
	
	/// <summary>
	/// Zwraca numer najnowszej wersji drzewa.
	/// </summary>
	/// <returns></returns>
	int getCurrentVersion() const
	{
		return _version;
	}

	/// <summary>
	/// Umieszcza nowy element w drzewie poszukiwan. Skutkuje utworzeniem nowej wersji drzewa.
	/// </summary>
	/// <param name="value">Wartosc do umieszczenia.</param>
	void insert(Type value)
	{
		if (_version == 0)
		{
			++_version;
			Node<Type> * node = new Node<Type>(value);
			_root[_version] = node;
		}
		else
		{
			// utworzenie nowego dziecka
			Node<Type> * newChild = new Node<Type>(value);
			// uwzglednienie zmian w rodzicach. Jezeli wezel rodzicielski
			// posiada juz zajety wskaznik zmiany, zostaje on skopiowany
			// wowczas kolejni rodzice tak samo, jezeli ich pole zmian jest zajete
			// w najgorszym wypadku zostaje utworzony nowy korzen
			bool stop = false;
			Node<Type> * currentChild = newChild;
			int currentChildValue = value;
			do
			{
				Node<Type> * currentParent = getParentNode(currentChildValue, _version);
				// brak rodzica -> dziecko jest nowym korzeniem
				if (currentParent == nullptr)
				{
					++_version;
					_root[_version] = currentChild;
					stop = true;
				}
				else
				{
					// jezeli w rodzicu nie ma zmiany, to ja wprowadzamy
					if (currentParent->_changeType == None)
					{
						if (currentChildValue < currentParent->getValue())
							currentParent->_changeType = LeftChild;
						else
							currentParent->_changeType = RightChild;
						++_version;
						currentParent->_changeTime = _version;
						currentParent->_changeChild = currentChild;
						_root[_version] = _root[_version - 1];
						stop = true;
					}
					// jezeli w rodzicu jest zmiana, to go kopiujemy
					// a zmiane wprowadzamy w jego rodzicu
					// powoduje wykonanie kolejnej iteracji
					else
					{
						Node<Type> * newParent = new Node<Type>(currentParent->getValue());
						if (currentParent->_changeType == LeftChild)
							newParent->setLeftChild(currentParent->_changeChild);
						else
							newParent->setRightChild(currentParent->_changeChild);
						if (currentChildValue < currentParent->getValue())
							newParent->setLeftChild(currentChild);
						else
							newParent->setRightChild(currentChild);
						currentChild = newParent;
						currentChildValue = currentChild->getValue();
					}
				}
			} while (!stop);
		}
	}

	/// <summary>
	/// Drukuje drzewo na konsoli
	/// </summary>
	void print(int version = 0) const
	{
		// nie mozna wydrukowac pustego drzewa
		if (_root.empty())
			return;
		// w przypadku braku parametru lub rownym zero, drukowana jest najswiezsza wersja
		if (version == 0)
			version = getCurrentVersion();
		int i = 1;
		Node<Type> * root = _root.at(version);
		Node<Type> * right = root->getRightChild(version);
		Node<Type> * left = root->getLeftChild(version);
		printNode(right, version, i);
		std::cout << root->getValue() << std::endl;
		printNode(left, version, i);
	}

private:
	/// <summary>
	/// Zwraca wskaznik na rodzica wezla o podanej wartosci zgodnie z podana wersja.
	/// </summary>
	/// <param name="value">Wartosc dziecka.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns>Jezeli rodzic istnieje, to wskaznik na niego, jezeli nie, to nullptr</returns>
	Node<Type> * getParentNode(Type value, int version) const
	{
		Node<Type> * currentNode = _root.at(version);
		if (currentNode->getValue() == value)
			return nullptr;
		bool found = false;
		while (!found)
		{
			Node<Type> * left, *right;
			left = currentNode->getLeftChild(version);
			right = currentNode->getRightChild(version);
			if (value < currentNode->getValue())
			{
				if (left == nullptr || left->getValue() == value)
					found = true;
				else
					currentNode = left;
			}
			else
			{
				if (right == nullptr || right->getValue() == value)
					found = true;
				else
					currentNode = right;
			}
		}
		return currentNode;
	}

	/// <summary>
	/// Drukuje pojedynczy wezel drzewa wraz z jego dziecmi
	/// </summary>
	/// <param name="node">Wezel.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <param name="level">Poziom w drzwie.</param>
	void printNode(Node<Type> * node, int version, int level) const
	{
		if (node == nullptr)
			return;
		Node<Type> * right = node->getRightChild(version);
		Node<Type> * left = node->getLeftChild(version);
		if (right != nullptr)
			printNode(right, version, level + 1);
		for (int i = 0; i < level; i++)
		{
			std::cout << '\t';
		}
		std::cout << node->getValue() << std::endl;
		if (left != nullptr)
			printNode(left, version, level + 1);
	}
};