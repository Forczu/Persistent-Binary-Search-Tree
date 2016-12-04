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
	typedef std::shared_ptr<Node<Type>> NodePtr;

	/// <summary>
	/// Aktualna wersja drzewa. Zaczyna sie od jedynki, kazda nowa wersja skutkuje inkrementacja tej wartosci
	/// </summary>
	int _version;

	/// <summary>
	/// Punkty wejscia do drzewa. Klucz oznacza numer historii, wartoscia jest wskaznik na korzen
	/// </summary>
	std::map<int, NodePtr> _root;

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
		_root.clear();
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
	/// <param name="value">Wartosc do usuniecia.</param>
	void erase(Type value)
	{
		NodePtr node = find(value, getCurrentVersion());
		// brak wartosci w drzwie
		if (node == nullptr)
			return;
		// brak dzieci
		if (node->getRightChild(_version) == nullptr && node->getLeftChild(_version) == nullptr)
		{
			// rodzic otrzymuje zmiane na nullptr
			setNewChildForMyParent(node, nullptr);
		}
		// lewe dziecko istnieje
		else if (node->getRightChild(_version) == nullptr && node->getLeftChild(_version) != nullptr)
		{
			// rodzic otrzymuje zmiane na lewego potomka swego dziecka
			setNewChildForMyParent(node, node->getLeftChild(_version));
		}
		// prawe dziecko istnieje
		else if (node->getRightChild(_version) != nullptr && node->getLeftChild(_version) == nullptr)
		{
			// rodzic otrzymuje zmiane na prawego potomka swego dziecka
			setNewChildForMyParent(node, node->getRightChild(_version));
		}
		// dwoje dzieci istnieje
		else
		{
			NodePtr largestInLeftSubtree = node->getLeftChild(_version);;
			bool found = false;
			while (!found)
			{
				if (largestInLeftSubtree->getRightChild(_version) != nullptr)
					largestInLeftSubtree = largestInLeftSubtree->getRightChild(_version);
				else
					found = true;
			}
			NodePtr smallestInRightSubtree = largestInLeftSubtree->getRightChild(_version);
			if (smallestInRightSubtree != nullptr)
			{
				found = false;
				while (!found)
				{
					if (smallestInRightSubtree->getLeftChild(_version) != nullptr)
						smallestInRightSubtree = smallestInRightSubtree->getLeftChild(_version);
					else
						found = true;
				}
				setNewChildForMyParent(largestInLeftSubtree, smallestInRightSubtree);
			}
			NodePtr largestInLeftSubtreeLeftChild = largestInLeftSubtree->getLeftChild(_version);
			NodePtr largestInLeftSubtreeParent = getParentNode(largestInLeftSubtree->getValue(), _version);

			if (largestInLeftSubtreeLeftChild != nullptr)
				setNewChildForMe(largestInLeftSubtreeParent, largestInLeftSubtreeLeftChild);

			if (node->getLeftChild(_version) != nullptr && node->getLeftChild(_version) != largestInLeftSubtree)
				setNewChildForMe(largestInLeftSubtree, node->getLeftChild(_version));
			if (node->getRightChild(_version) != nullptr)
				setNewChildForMe(largestInLeftSubtree, node->getRightChild(_version));

			setNewChildForMyParent(node, largestInLeftSubtree);
		}
	}
	
	/// <summary>
	/// Wyszukuje podana wartosc w drzewie o wskazanej wersji. Zero oznacza wersje aktualna drzewa.
	/// Jezeli wartosci nie ma w drzewie o podanej wersji, zwracany jest iterator na koniec drzewa.
	/// </summary>
	/// <param name="value">Wartosc do wyszukania.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns></returns>
	NodePtr find(Type value, int version = 0) const
	{
		if (version == 0)
			version = getCurrentVersion();
		bool found = false;
		NodePtr currentNode = _root.at(version);
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
		return currentNode;
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
			NodePtr node(new Node<Type>(value));
			_root[_version] = node;
		}
		else
		{
			// utworzenie nowego dziecka
			NodePtr newChild(new Node<Type>(value));
			// uwzglednienie zmian w rodzicach. Jezeli wezel rodzicielski
			// posiada juz zajety wskaznik zmiany, zostaje on skopiowany
			// wowczas kolejni rodzice tak samo, jezeli ich pole zmian jest zajete
			// w najgorszym wypadku zostaje utworzony nowy korzen
			bool stop = false;
			NodePtr currentChild(newChild);
			int currentChildValue = value;
			do
			{
				NodePtr currentParent = getParentNode(currentChildValue, _version);
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
						NodePtr newParent (new Node<Type>(currentParent->getValue()));
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
		if (version > getCurrentVersion())
			return;
		int i = 1;
		NodePtr root = _root.at(version);
		NodePtr right = root->getRightChild(version);
		NodePtr left = root->getLeftChild(version);
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
	NodePtr getParentNode(Type value, int version) const
	{
		NodePtr currentNode = _root.at(version);
		if (currentNode->getValue() == value)
			return nullptr;
		bool found = false;
		while (!found)
		{
			NodePtr left, right;
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
	void printNode(NodePtr node, int version, int level) const
	{
		if (node == nullptr)
			return;
		NodePtr right = node->getRightChild(version);
		NodePtr left = node->getLeftChild(version);
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

	/// <summary>
	/// Zmienia potomka (na odpowiedniej pozycji) w rodzicu wskazanego wezla i zapisuje zmiany
	/// </summary>
	/// <param name="value">Wartosc.</param>
	/// <param name="newChild">Nowe dziecko.</param>
	void setNewChildForMyParent(NodePtr base, NodePtr newChild)
	{
		bool stop = false;
		NodePtr currentChild(newChild);
		NodePtr currentBase = base;
		do
		{
			NodePtr currentParent = getParentNode(currentBase->getValue(), _version);
			// brak rodzica -> wstawiamy tam dziecko
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
					if (currentBase->getValue() < currentParent->getValue())
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
					NodePtr newParent(new Node<Type>(currentParent->getValue()));
					if (currentParent->_changeType == LeftChild)
						newParent->setLeftChild(currentParent->_changeChild);
					else
						newParent->setRightChild(currentParent->_changeChild);
					if (currentBase->getValue() < currentParent->getValue())
						newParent->setLeftChild(currentChild);
					else
						newParent->setRightChild(currentChild);
					currentChild = currentBase = newParent;
				}
			}
		} while (!stop);
	}

	/// <summary>
	/// Zmienia potomka (na odpowiedniej pozycji) we wskazanym wezle i zapisuje zmiany
	/// </summary>
	/// <param name="value">Wartosc.</param>
	/// <param name="newChild">Nowe dziecko.</param>
	void setNewChildForMe(NodePtr base, NodePtr newChild)
	{
		bool stop = false;
		NodePtr currentChild(newChild);
		NodePtr currentBase = base;
		do
		{
			NodePtr currentParent = currentBase;
			// brak rodzica -> wstawiamy tam dziecko
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
					if (currentBase->getValue() < currentParent->getValue())
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
					NodePtr newParent(new Node<Type>(currentParent->getValue()));
					if (currentParent->_changeType == LeftChild)
						newParent->setLeftChild(currentParent->_changeChild);
					else
						newParent->setRightChild(currentParent->_changeChild);
					if (currentBase->getValue() < currentParent->getValue())
						newParent->setLeftChild(currentChild);
					else
						newParent->setRightChild(currentChild);
					currentChild = currentBase = newParent;
				}
			}
		} while (!stop);
	}
};