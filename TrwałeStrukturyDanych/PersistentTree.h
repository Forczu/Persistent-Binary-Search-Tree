#pragma once
#include "PersistentTreeIterator.h"
#include "Node.h"
#include <map>
#include <vector>

/// <summary>
/// Klasa reprezentujaca trwale drzewo poszukiwan binarnych.
/// Zastosowany algorytm to metoda Sleatora, Tarjana i innych
/// Parametr szablonowy Type okresla typ danych, jaki przechowywany w drzewie
/// </summary>
template<class Type>
class PersistentTree
{	
	typedef std::shared_ptr<Node<Type>> NodePtr;

	static const int FIRST_VERSION = 0;
	static const int CURRENT_VERSION = -1;

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
	PersistentTree() : _version(FIRST_VERSION)
	{
		_root[_version] == nullptr;
	}

	/// <summary>
	/// Tworzy nowe drzewo z wartosciami wstawianymi wg przekazanego wektora
	/// Otrzymane drzewo jest wersja zerowa.
	/// </summary>
	/// <param name="values">Wartosci poczatkowe.</param>
	PersistentTree(std::vector<Type> const & values) : _version(FIRST_VERSION)
	{
		if (values.empty())
			return;
		NodePtr root(new Node<Type>(values.front()));
		std::vector<Type>::const_iterator it = values.begin();
		++it;
		for ( ; it != values.end(); ++it)
		{
			NodePtr node(new Node<Type>(*it));
			NodePtr currentParent = root;
			bool isSet = false;
			while (!isSet)
			{
				int parentValue = currentParent->getValue(CURRENT_VERSION);
				int currentValue = node->getValue(CURRENT_VERSION);
				if (currentValue < parentValue)
				{
					NodePtr parentLeftChild = currentParent->getLeftChild(CURRENT_VERSION);
					if (parentLeftChild == nullptr)
					{
						currentParent->setLeftChild(node);
						isSet = true;
					}
					else
					{
						currentParent = parentLeftChild;
					}
				}
				else
				{
					NodePtr parentRightChild = currentParent->getRightChild(CURRENT_VERSION);
					if (parentRightChild == nullptr)
					{
						currentParent->setRightChild(node);
						isSet = true;
					}
					else
					{
						currentParent = parentRightChild;
					}
				}
			}
		}
		_root[_version] = root;
	}

	/// <summary>
	/// Zwraca iterator na poczatek aktualnego drzewa
	/// </summary>
	/// <param name="version">Wersja drzewa, po ktorej nalezy iterowac. Zero oznacza wersje aktualna</param>
	/// <returns></returns>
	iterator begin(int version = CURRENT_VERSION) const
	{
		if (!getCorrectVersion(version))
			return end();
		iterator it(_root.at(version), version);
		return it;
	}

	/// <summary>
	/// Usuwa calosc drzewa i zapisuje ten stan jako nowa wersje
	/// </summary>
	void clear()
	{
		auto currentRoot = _root[_version];
		// jak drzewo juz jest puste to nie ma zmiany
		if (currentRoot == nullptr)
			return;
		confirmChange();
		_root[_version] = nullptr;
	}
	
	/// <summary>
	/// Zlicza liczba elementow we wskazanej wersji drzewa.
	/// </summary>
	/// <param name="version">Wersja drzewa, po ktorej nalezy iterowac. Zero oznacza wersje aktualna</param>
	/// <returns></returns>
	int count(int version = CURRENT_VERSION) const
	{
		int count = 0;
		if (!getCorrectVersion(version))
			return count;
		for (iterator it = begin(version); it != end(); ++it, ++count) { }
		return count;
	}

	/// <summary>
	/// Zwraca koniec aktualnego drzewa
	/// </summary>
	/// <returns></returns>
	iterator end() const
	{
		iterator it;
		return it;
	}
	
	/// <summary>
	/// Usuwa element o podanej wartosci z drzewa. Skutkuje utworzeniem nowej wersji drzewa.
	/// </summary>
	/// <param name="value">Wartosc do usuniecia.</param>
	void erase(Type value)
	{
		int version = getCurrentVersion();
		NodePtr node = find(value, version);
		// brak wartosci w drzwie
		if (node == nullptr)
			return;
		_root[version + 1] = _root[version];
		auto rightChild = node->getRightChild(version);
		auto leftChild  = node->getLeftChild(version);
		auto parent = getParentNode(node->getValue(version) , version);
		bool isLeftChild = parent->getLeftChild(version) == node ? true : false;
		// brak dzieci
		if (rightChild == nullptr && leftChild == nullptr)
		{
			// rodzic otrzymuje zmiane na nullptr
			setNewChildForMe(parent, nullptr, isLeftChild, version + 1);
		}
		// lewe dziecko istnieje
		else if (rightChild == nullptr && leftChild != nullptr)
		{
			// rodzic otrzymuje zmiane na lewego potomka swego dziecka
			setNewChildForMe(parent, leftChild, isLeftChild, version + 1);
		}
		// prawe dziecko istnieje
		else if (rightChild != nullptr && leftChild == nullptr)
		{
			// rodzic otrzymuje zmiane na prawego potomka swego dziecka
			setNewChildForMe(parent, rightChild, isLeftChild, version + 1);
		}
		// dwoje dzieci istnieje
		else
		{
			// wezlem zostaje najwieksza wartosc w lewym poddrzwie usuwanego wezla
			NodePtr largestInLeftSubtree = node->getLeftChild(version);
			bool found = false;
			while (!found)
			{
				if (largestInLeftSubtree->getRightChild(version) != nullptr)
					largestInLeftSubtree = largestInLeftSubtree->getRightChild(version);
				else
					found = true;
			}
			NodePtr largestInLeftSubtreeLeftChild = largestInLeftSubtree->getLeftChild(version);
			int value = largestInLeftSubtree->getValue(version);
			NodePtr largestInLeftSubtreeParent = getParentNode(largestInLeftSubtree->getValue(version), version);
			bool isLeftChild = largestInLeftSubtreeParent->getLeftChild(version) == largestInLeftSubtree ? true : false;
			setNewChildForMe(largestInLeftSubtreeParent, largestInLeftSubtreeLeftChild, isLeftChild, version + 1);
			changeValue(node, value, version + 1);
		}
		confirmChange();
	}
	
	/// <summary>
	/// Wyszukuje podana wartosc w drzewie o wskazanej wersji. Zero oznacza wersje aktualna drzewa.
	/// Jezeli wartosci nie ma w drzewie o podanej wersji, zwracany jest iterator na koniec drzewa.
	/// </summary>
	/// <param name="value">Wartosc do wyszukania.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns></returns>
	NodePtr find(Type value, int version = CURRENT_VERSION) const
	{
		if (!getCorrectVersion(version))
			return nullptr;
		bool found = false;
		NodePtr currentNode = _root.at(version);
		while (!found)
		{
			if (value < currentNode->getValue(version))
				currentNode = currentNode->getLeftChild(version);
			else if (value > currentNode->getValue(version))
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
		if (_root.at(_version) == nullptr)
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
						ChangeType type = currentChildValue < currentParent->getValue(_version) ? LeftChild : RightChild;
						++_version;
						currentParent->setChange(type, currentChild, _version);
						_root[_version] = _root[_version - 1];
						stop = true;
					}
					// jezeli w rodzicu jest zmiana, to go kopiujemy
					// a zmiane wprowadzamy w jego rodzicu
					// powoduje wykonanie kolejnej iteracji
					else
					{
						// nowy rodzic z nowa wartoscia
						int parentValue = currentParent->getValue(_version);
						NodePtr newParent(new Node<Type>(parentValue));
						// ustaw dzieci
						if (currentParent->_changeType == LeftChild)
							newParent->setLeftChild(currentParent->_change.child);
						else if (currentParent->_changeType == RightChild)
							newParent->setRightChild(currentParent->_change.child);
						if (currentChildValue < parentValue)
							newParent->setLeftChild(currentChild);
						else
							newParent->setRightChild(currentChild);
						currentChild = newParent;
						currentChildValue = currentChild->getValue(_version);
					}
				}
			} while (!stop);
		}
	}

	/// <summary>
	/// Drukuje drzewo na konsoli
	/// </summary>
	void print(int version = CURRENT_VERSION) const
	{
		// nie mozna wydrukowac pustego drzewa
		if (!getCorrectVersion(version) || _root.at(version) == nullptr)
			return;
		int i = 1;
		NodePtr root = _root.at(version);
		NodePtr right = root->getRightChild(version);
		NodePtr left = root->getLeftChild(version);
		printNode(right, version, i);
		std::cout << root->getValue(version) << std::endl;
		printNode(left, version, i);
	}

	/// <summary>
	/// Usuwa zawartosc drzewa wraz z historia
	/// </summary>
	void purge()
	{
		_root.clear();
		_version = FIRST_VERSION;
		_root[_version] = nullptr;
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
		if (currentNode->getValue(version) == value)
			return nullptr;
		bool found = false;
		while (!found)
		{
			NodePtr left, right;
			left = currentNode->getLeftChild(version);
			right = currentNode->getRightChild(version);
			if (value < currentNode->getValue(version))
			{
				if (left == nullptr || left->getValue(version) == value)
					found = true;
				else
					currentNode = left;
			}
			else
			{
				if (right == nullptr || right->getValue(version) == value)
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
		std::cout << node->getValue(version) << std::endl;
		if (left != nullptr)
			printNode(left, version, level + 1);
	}

	/// <summary>
	/// Zmienia potomka (na odpowiedniej pozycji) we wskazanym wezle i zapisuje zmiany
	/// </summary>
	/// <param name="value">Wartosc.</param>
	/// <param name="newChild">Nowe dziecko.</param>
	void setNewChildForMe(NodePtr parent, NodePtr newChild, bool isLeftChild, int version)
	{
		bool stop = false;
		NodePtr currentChild = newChild;
		NodePtr currentParent = parent;
		do
		{
			if (currentParent == nullptr)
			{
				_root[version] = currentChild;
				stop = true;
			}
			else
			{
				if (currentParent->_changeType == None)
				{
					ChangeType type;
					if (currentChild == nullptr)
					{
						type = isLeftChild ? LeftChild : RightChild;
					}
					else
					{
						int childValue = currentChild->getValue(version);
						int parentValue = currentParent->getValue(version);
						type = childValue < parentValue ? LeftChild : RightChild;
					}
					currentParent->setChange(type, currentChild, version);
					stop = true;
				}
				else
				{
					NodePtr newParent = makeCopy(currentParent, version);
					if (currentChild != nullptr)
					{
						if (currentChild->getValue(version) < newParent->getValue(version))
							newParent->setLeftChild(currentChild);
						else
							newParent->setRightChild(currentChild);
					}
					else
					{
						if (isLeftChild)
							newParent->setLeftChild(currentChild);
						else
							newParent->setRightChild(currentChild);
					}
					currentChild = newParent;
					currentParent = getParentNode(currentChild->getValue(version), version);
				}
			}
		} while (!stop);
	}

	/// <summary>
	/// Zmienia wartosc w wezle i propaguje te zmiane
	/// </summary>
	/// <param name="node">Wezel.</param>
	/// <param name="value">Nowa wartosc.</param>
	/// <param name="version">Wersja drzewa.</param>
	void changeValue(NodePtr node, int value, int version)
	{
		bool stop = false;
		ChangeType type = Value;
		// jezeli brak zmiany, to tylko ja wprowadzamy
		if (node->_changeType == None)
		{
			node->setChange(type, value, version);
		}
		else
		{
			bool stop = false;
			NodePtr currentNode = makeCopy(node, version);
			NodePtr currentParent = getParentNode(currentNode->getValue(version), version);
			currentNode->setValue(value);
			do
			{
				if (currentParent == nullptr)
				{
					_root[version] = currentNode;
					stop = true;
				}
				else if (currentParent->_changeType == None)
				{
					ChangeType type = currentNode->getValue(version) < currentParent->getValue(version) ? LeftChild : RightChild;
					currentParent->setChange(type, currentNode, version);
					stop = true;
				}
				else
				{
					NodePtr newParent = makeCopy(currentParent, version);
					int nodeValue = currentNode->getValue(version);
					int parentValue = newParent->getValue(version);
					if (nodeValue < parentValue)
						newParent->setLeftChild(currentNode);
					else
						newParent->setRightChild(currentNode);
					currentNode = newParent;
					currentParent = getParentNode(currentNode->getValue(version), version);
				}
			} while (!stop);
		}
	}

	/// <summary>
	/// Tworzy kopie wezla z uwzglednieniem pola zmiany.
	/// </summary>
	/// <param name="node">Wezel.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns></returns>
	NodePtr makeCopy(NodePtr node, int version)
	{
		NodePtr copy(new Node<Type>(node->getValue(version)));
		copy->setRightChild(node->getRightChild(version));
		copy->setLeftChild(node->getLeftChild(version));
		return copy;
	}

	/// <summary>
	/// Potwierdzenie zmiany w historii.
	/// </summary>
	void confirmChange()
	{
		++_version;
	}

	/// <summary>
	/// Zapisuje do referencji poprawna wersja drzewa na podstawie przekazanego argumentu.
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns>True, jezeli poprawna wersja istnieje, false, jezeli nie.</returns>
	bool getCorrectVersion(int & version) const
	{
		int currentVersion = getCurrentVersion();
		if (version > currentVersion)
			return false;
		if (version == CURRENT_VERSION)
			version = currentVersion;
		return true;
	}
};