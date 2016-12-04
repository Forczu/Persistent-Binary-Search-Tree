#pragma once
#include "PersistentTreeIterator.h"
#include <map>

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
	// pole drzewa
	Node<Type> * _rightChild;
	Type _value;
	Node<Type> * _leftChild;

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
};

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
	iterator begin(int version = 0)
	{

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
	int count(int version = 0)
	{
		
	}

	/// <summary>
	/// Zwraca koniec aktualnego drzewa
	/// </summary>
	/// <param name="version">Wersja drzewa, po ktorej nalezy iterowac. Zero oznacza wersje aktualna</param>
	/// <returns></returns>
	iterator end(int version = 0)
	{
		
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
	iterator find(Type value, int version = 0)
	{

	}
	
	/// <summary>
	/// Zwraca numer najnowszej wersji drzewa.
	/// </summary>
	/// <returns></returns>
	int getCurrentVersion()
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
						if (currentChildValue < currentParent->_value)
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
						Node<Type> * newParent = new Node<Type>(currentParent->_value);
						if (currentParent->_changeType == LeftChild)
							newParent->_leftChild = currentParent->_changeChild;
						else
							newParent->_rightChild = currentParent->_changeChild;
						if (currentChildValue < currentParent->_value)
							newParent->_leftChild = currentChild;
						else
							newParent->_rightChild = currentChild;
						currentChild = newParent;
						currentChildValue = currentChild->_value;
					}
				}
			} while (!stop);
		}
	}

	/// <summary>
	/// Drukuje drzewo na konsoli
	/// </summary>
	void print(int version = 0)
	{
		// w przypadku braku parametru lub rownym zero, drukowana jest najswiezsza wersja
		if (version == 0)
			version = getCurrentVersion();
		int i = 1;
		Node<Type> * root = _root[version];
		Node<Type> * right = root->_changeType == RightChild && version >= root->_changeTime ? root->_changeChild : root->_rightChild;
		Node<Type> * left = root->_changeType == LeftChild && version >= root->_changeTime ? root->_changeChild : root->_leftChild;
		printNode(right, version, i);
		std::cout << root->_value << std::endl;
		printNode(left, version, i);
	}

private:
	/// <summary>
	/// Zwraca wskaznik na rodzica wezla o podanej wartosci zgodnie z podana wersja.
	/// </summary>
	/// <param name="value">Wartosc dziecka.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns>Jezeli rodzic istnieje, to wskaznik na niego, jezeli nie, to nullptr</returns>
	Node<Type> * getParentNode(Type value, int version)
	{
		Node<Type> * currentNode = _root[version];
		if (currentNode->_value == value)
			return nullptr;
		bool found = false;
		while (!found)
		{
			Node<Type> * left, *right;
			if (currentNode->_changeType == LeftChild && currentNode->_changeTime >= version)
			{
				left = currentNode->_changeChild;
				right = currentNode->_rightChild;
			}
			else if (currentNode->_changeType == RightChild && currentNode->_changeTime >= version)
			{
				left = currentNode->_leftChild;
				right = currentNode->_changeChild;
			}
			else
			{
				left = currentNode->_leftChild;
				right = currentNode->_rightChild;
			}
			if (value < currentNode->_value)
			{
				if (currentNode->_leftChild == nullptr || currentNode->_leftChild->_value == value)
					found = true;
				else
					currentNode = currentNode->_leftChild;
			}
			else
			{
				if (currentNode->_rightChild == nullptr || currentNode->_rightChild->_value == value)
					found = true;
				else
					currentNode = currentNode->_rightChild;
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
	void printNode(Node<Type> const * node, int version, int level)
	{
		if (node == nullptr)
			return;
		Node<Type> * right = node->_changeType == RightChild && version >= node->_changeTime ? node->_changeChild : node->_rightChild;
		Node<Type> * left = node->_changeType == LeftChild && version >= node->_changeTime ? node->_changeChild : node->_leftChild;
		if (right != nullptr)
			printNode(right, version, level + 1);
		for (int i = 0; i < level; i++)
		{
			std::cout << '\t';
		}
		std::cout << node->_value << std::endl;
		if (left != nullptr)
			printNode(left, version, level + 1);
	}
};