#pragma once
#include "PersistentTreeIterator.h"
#include "NodeAllocator.h"
#include "Node.h"
#include <functional>
#include <queue>
#include <vector>
#include <unordered_set>

/// <summary>
/// Klasa reprezentujaca trwale drzewo poszukiwan binarnych.
/// Zastosowany algorytm to metoda Sleatora, Tarjana i innych
/// Parametr szablonowy Type okresla typ danych, jaki przechowywany w drzewie oraz funkcje porzadku
/// </summary>
template<class Type, class OrderFunctor = std::less<Type>>
class PersistentTree
{	
	typedef Node<Type>* NodePtr;
	typedef std::vector<std::pair<int, NodePtr>> RootVec;

	/// <summary>
	/// Identyfikator pierwszej wersji drzewa
	/// </summary>
	static const int FIRST_VERSION = 0;

	/// <summary>
	/// Identyfikator przekierowujacy do aktualnej wersji drzewa
	/// </summary>
	static const int CURRENT_VERSION = -1;

	/// <summary>
	/// Aktualna wersja drzewa. Zaczyna sie od jedynki, kazda nowa wersja skutkuje inkrementacja tej wartosci
	/// </summary>
	int _version;

	/// <summary>
	/// Punkty wejscia do drzewa. Klucz oznacza numer historii, wartoscia jest wskaznik na korzen
	/// </summary>
	RootVec _root;

	/// <summary>
	/// Obiekt funktora porzadku
	/// </summary>
	OrderFunctor orderFunctor;

	/// <summary>
	/// Alokator dla wezlow drzewa
	/// </summary>
	NodeAllocator<Type> _allocator;

	/// <summary>
	/// Pomocniczy alokator dla tworzenia zlozonych obiektow przy zmianie wartosci wezla
	/// </summary>
	std::allocator<Type> _typeAllocator;

public:
	typedef PersistentTreeIterator<Type> iterator;
	typedef PersistentTreeIterator<const Type> const_iterator;

	/// <summary>
	/// Tworzy nowe, puste drzewo bez historii.
	/// </summary>
	PersistentTree() : _version(FIRST_VERSION)
	{
	}

	/// <summary>
	/// Niszczy drzewo usuwajac z pamieci wszystkie wezly
	/// </summary>
	~PersistentTree()
	{
		deallocateNodes();
	}

	/// <summary>
	/// Tworzy nowe drzewo z wartosciami wstawianymi wg przekazanego wektora
	/// Otrzymane drzewo jest wersja zerowa.
	/// </summary>
	/// <param name="values">Wartosci poczatkowe.</param>
	template <class Iter>
	PersistentTree(Iter begin, Iter end) : _version(FIRST_VERSION)
	{
		NodePtr root = allocateNode(*begin);
		Iter it = begin;
		++it;
		for ( ; it != end; ++it)
		{
			NodePtr node = allocateNode(*it);
			NodePtr currentParent = root;
			bool isSet = false;
			while (!isSet)
			{
				Type parentValue = currentParent->getValue(CURRENT_VERSION);
				Type currentValue = node->getValue(CURRENT_VERSION);
				if (orderFunctor(currentValue, parentValue))
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
		_root.push_back(std::pair<int, NodePtr>(FIRST_VERSION, root));
	}

	/// <summary>
	/// Zwraca iterator na poczatek aktualnego drzewa
	/// </summary>
	/// <param name="version">Wersja drzewa, po ktorej nalezy iterowac. Brak parametru oznacza wersje aktualna</param>
	/// <returns></returns>
	iterator begin(int version = CURRENT_VERSION) const
	{
		NodePtr root = getRoot(version);
		if (root == nullptr)
			return end();
		iterator it(root, version);
		return it;
	}

	/// <summary>
	/// Usuwa calosc drzewa i zapisuje ten stan jako nowa wersje
	/// </summary>
	void clear()
	{
		auto currentRoot = getRoot(_version);
		// jak drzewo juz jest puste to nie ma zmiany
		if (currentRoot == nullptr)
			return;
		confirmChange();
		_root.push_back(std::pair<int, NodePtr>(_version, nullptr));
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
	bool erase(Type value)
	{
		iterator it = find(value, _version);
		// brak wartosci w drzwie
		if (it == end())
			return false;
		NodePtr node = it.getNode();
		//_root.push_back(_root[version]);
		NodePtr rightChild = node->getRightChild(_version);
		NodePtr leftChild  = node->getLeftChild(_version);
		NodePtr parent = getParentNode(*node->getValue(_version), _version);
		if (parent == nullptr)
		{
			// usuwamy korzen
			if (rightChild == nullptr && leftChild != nullptr)
			{
				setNewChildForMe(parent, leftChild);
			}
			else if (rightChild != nullptr && leftChild == nullptr)
			{
				setNewChildForMe(parent, rightChild);
			}
			else if (rightChild != nullptr && leftChild != nullptr)
			{
				setLargestValueInLeftSubtreeAsChild(node);
			}
			else
			{
				_root.push_back(std::pair<int, NodePtr>(_version + 1, nullptr));
			}
		}
		else
		{
			// usuwamy wezel na glebszym poziomie
			if (rightChild == nullptr && leftChild == nullptr)
			{
				// nie mam dzieci, daj rodzicowi nullptr
				if (parent->getLeftChild(_version) == node)
					setLeftChildAsNull(parent);
				else
					setRightChildAsNull(parent);
			}
			// lewe dziecko istnieje
			else if (rightChild == nullptr && leftChild != nullptr)
			{
				// rodzic otrzymuje zmiane na lewego potomka swego dziecka
				setNewChildForMe(parent, leftChild);
			}
			// prawe dziecko istnieje
			else if (rightChild != nullptr && leftChild == nullptr)
			{
				// rodzic otrzymuje zmiane na prawego potomka swego dziecka
				setNewChildForMe(parent, rightChild);
			}
			// dwoje dzieci istnieje
			else
			{
				setLargestValueInLeftSubtreeAsChild(node);
			}
		}
		confirmChange();
		return true;
	}
	
	/// <summary>
	/// Wyszukuje podana wartosc w drzewie o wskazanej wersji. Zero oznacza wersje aktualna drzewa.
	/// Jezeli wartosci nie ma w drzewie o podanej wersji, zwracany jest iterator na koniec drzewa.
	/// </summary>
	/// <param name="value">Wartosc do wyszukania.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns></returns>
	iterator find(Type & value, int version = CURRENT_VERSION) const
	{
		NodePtr root = getRoot(version);
		if (root == nullptr)
			return end();
		NodePtr currentNode = root;
		bool found = false;
		while (!found)
		{
			if (orderFunctor(value, *currentNode->getValue(version)))
				currentNode = currentNode->getLeftChild(version);
			else if (orderFunctor(*currentNode->getValue(version), value))
				currentNode = currentNode->getRightChild(version);
			else
				found = true;
			if (currentNode == nullptr)
				found = true;
		}
		iterator it(currentNode, root, orderFunctor, version);
		return it;
	}

	/// <summary>
	/// Zwraca kopie drzewa o wskazanej wersji.
	/// Kopia posiada jedynie te wersje, ktora jest jej pierwsza.
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	PersistentTree<Type> * getCopy(int version)
	{
		if (!getCorrectVersion(version))
			return nullptr;
		// wyciagnij wszystkie wartosci z drzewa o podanej wersji
		NodePtr root = getRoot(version);
		// jezeli korzen nie istnieje, zwroc nowe puste drzewo
		if (root == nullptr)
			return new PersistentTree();
		std::queue<NodePtr> queue;
		std::vector<Type> values;
		queue.push(root);
		while (!queue.empty())
		{
			NodePtr node(queue.front());
			queue.pop();
			Type value = node->getValue(version);
			values.push_back(value);
			NodePtr leftChild = node->getLeftChild(version);
			NodePtr rightChild = node->getRightChild(version);
			if (leftChild != nullptr)
				queue.push(leftChild);
			if (rightChild != nullptr)
				queue.push(rightChild);
		}
		return new PersistentTree<Type>(values.begin(), values.end());
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
	bool insert(Type & value)
	{
		if (find(value) != end())
			return false;
		NodePtr root = getRoot(_version);
		if (root == nullptr)
		{
			++_version;
			NodePtr node = allocateNode(value);
			_root.push_back(std::pair<int, NodePtr>(_version, node));
		}
		else
		{
			// utworzenie nowego dziecka
			NodePtr newChild = allocateNode(value);
			// uwzglednienie zmian w rodzicach. Jezeli wezel rodzicielski
			// posiada juz zajety wskaznik zmiany, zostaje on skopiowany
			// wowczas kolejni rodzice tak samo, jezeli ich pole zmian jest zajete
			// w najgorszym wypadku zostaje utworzony nowy korzen
			bool stop = false;
			NodePtr currentChild = newChild;
			Type * currentChildValue = &value;
			do
			{
				NodePtr currentParent = getParentNode(*currentChildValue, _version);
				// brak rodzica -> dziecko jest nowym korzeniem
				if (currentParent == nullptr)
				{
					++_version;
					_root.push_back(std::pair<int, NodePtr>(_version, currentChild));
					stop = true;
				}
				else
				{
					// jezeli w rodzicu nie ma zmiany, to ja wprowadzamy
					if (currentParent->getChangeType() == ChangeType::None)
					{
						ChangeType type = orderFunctor(*currentChildValue, *currentParent->getValue(_version)) ? ChangeType::LeftChild : ChangeType::RightChild;
						++_version;
						currentParent->setChange(type, currentChild, _version);
						stop = true;
					}
					// jezeli w rodzicu jest zmiana, to go kopiujemy
					// a zmiane wprowadzamy w jego rodzicu
					// powoduje wykonanie kolejnej iteracji
					else
					{
						// nowy rodzic z nowa wartoscia
						Type * parentValue = currentParent->getValue(_version);
						NodePtr leftChild = currentParent->getLeftChild(_version);
						NodePtr rightChild = currentParent->getRightChild(_version);

						NodePtr newParent = allocateNode(*parentValue);
						newParent->setLeftChild(leftChild);
						newParent->setRightChild(rightChild);

						if (orderFunctor(*currentChildValue, *parentValue))
							newParent->setLeftChild(currentChild);
						else
							newParent->setRightChild(currentChild);

						currentChild = newParent;
						currentChildValue = parentValue;
					}
				}
			} while (!stop);
		}
		return true;
	}

	/// <summary>
	/// Drukuje drzewo na konsoli
	/// </summary>
	void print(int version = CURRENT_VERSION) const
	{
		NodePtr root = getRoot(version);
		// nie mozna wydrukowac pustego drzewa
		if (root == nullptr)
			return;
		int i = 1;
		NodePtr right = root->getRightChild(version);
		NodePtr left = root->getLeftChild(version);
		printNode(right, version, i);
		std::cout << *root->getValue(version) << std::endl;
		printNode(left, version, i);
	}

	/// <summary>
	/// Usuwa zawartosc drzewa wraz z historia
	/// </summary>
	void purge()
	{
		deallocateNodes();
		_root.clear();
		_version = FIRST_VERSION;
		_root.push_back(nullptr);
	}

	/// <summary>
	/// Zlicza liczba elementow we wskazanej wersji drzewa.
	/// </summary>
	/// <param name="version">Wersja drzewa, po ktorej nalezy iterowac. Zero oznacza wersje aktualna</param>
	/// <returns></returns>
	int size(int version = CURRENT_VERSION) const
	{
		int count = 0;
		if (!getCorrectVersion(version))
			return count;
		for (iterator it = begin(version); it != end(); ++it, ++count) {}
		return count;
	}

	/// <summary>
	/// Zwraca liczbe wszystkich wezlow zaalokowanych w drzewie.
	/// </summary>
	/// <returns></returns>
	int size_of_history()
	{
		return _allocator.getNodeCount();
	}

private:
	/// <summary>
	/// Zwraca wskaznik na rodzica wezla o podanej wartosci zgodnie z podana wersja.
	/// </summary>
	/// <param name="value">Wartosc dziecka.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns>Jezeli rodzic istnieje, to wskaznik na niego, jezeli nie, to nullptr</returns>
	NodePtr getParentNode(Type & value, int version) const
	{
		NodePtr currentNode = getRoot(version);
		if (currentNode == nullptr || *currentNode->getValue(version) == value)
			return nullptr;
		bool found = false;
		while (!found)
		{
			NodePtr left, right;
			left = currentNode->getLeftChild(version);
			right = currentNode->getRightChild(version);
			if (orderFunctor(value, *currentNode->getValue(version)))
			{
				if (left == nullptr || *left->getValue(version) == value)
					found = true;
				else
					currentNode = left;
			}
			else
			{
				if (right == nullptr || *right->getValue(version) == value)
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
		std::cout << *node->getValue(version) << std::endl;
		if (left != nullptr)
			printNode(left, version, level + 1);
	}

	/// <summary>
	/// Zmienia potomka (na odpowiedniej pozycji) we wskazanym wezle i zapisuje zmiany
	/// </summary>
	/// <param name="value">Wartosc.</param>
	/// <param name="newChild">Nowe dziecko.</param>
	void setNewChildForMe(NodePtr parent, NodePtr newChild)
	{
		propagateChangesAfterInsert(parent, newChild);
	}

	/// <summary>
	/// Ustawia prawe dziecko wskazanego wezla jako nullptr wraz z propagacja zmian.
	/// </summary>
	/// <param name="parent">Wezel rodzica.</param>
	void setRightChildAsNull(NodePtr parent)
	{
		NodePtr currentChild = nullptr;
		NodePtr currentParent = parent;
		if (parent->getChangeType() == ChangeType::None)
		{
			parent->setChange(ChangeType::RightChild, nullptr, _version);
			return;
		}
		else
		{
			NodePtr newParent = makeCopy(parent, _version);
			newParent->setRightChild(nullptr);
			currentChild = newParent;
			currentParent = getParentNode(*currentChild->getValue(_version), _version);
		}
		propagateChangesAfterInsert(currentParent, currentChild);
	}

	/// <summary>
	/// Ustawia lewe dziecko wskazanego wezla jako nullptr wraz z propagacja zmian.
	/// </summary>
	/// <param name="parent">Wezel rodzica.</param>
	void setLeftChildAsNull(NodePtr parent)
	{
		NodePtr currentChild = nullptr;
		NodePtr currentParent = parent;
		if (parent->getChangeType() == ChangeType::None)
		{
			parent->setChange(ChangeType::LeftChild, nullptr, _version);
			return;
		}
		else
		{
			NodePtr newParent = makeCopy(parent, _version);
			newParent->setLeftChild(nullptr);
			currentChild = newParent;
			currentParent = getParentNode(*currentChild->getValue(_version), _version);
		}
		propagateChangesAfterInsert(currentParent, currentChild);
	}

	/// <summary>
	/// Zmienia wartosc w wezle i propaguje te zmiane
	/// </summary>
	/// <param name="node">Wezel.</param>
	/// <param name="value">Nowa wartosc.</param>
	/// <param name="version">Wersja drzewa.</param>
	void changeValue(NodePtr node, Type * value)
	{
		if (node->getChangeType() == ChangeType::None)
		{
			Type * newValue = _typeAllocator.allocate(1);
			_typeAllocator.construct(newValue, *value);
			node->setChange(ChangeType::Value, *newValue, _version + 1);
		}
		else
		{
			NodePtr currentNode = makeCopy(node, value, _version + 1);
			NodePtr currentParent = getParentNode(*node->getValue(_version), _version);
			propagateChangesAfterInsert(currentParent, currentNode);
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
		Type * value = node->getValue(version);
		NodePtr copy = allocateNode(*value);
		copy->setRightChild(node->getRightChild(version));
		copy->setLeftChild(node->getLeftChild(version));
		return copy;
	}

	/// <summary>
	/// worzy kopie wezla z uwzglednieniem pola zmiany i nowej wartosci
	/// </summary>
	/// <param name="node">Wezel.</param>
	/// <param name="value">Nowa wartosc.</param>
	/// <param name="version">Wersja drzewa.</param>
	/// <returns></returns>
	NodePtr makeCopy(NodePtr node, Type * value, int version)
	{
		NodePtr copy = allocateNode(*value);
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
		if (version == CURRENT_VERSION)
			version = currentVersion;
		return true;
	}

	/// <summary>
	/// Zwraca korzen do drzewa o wskazanej wersji
	/// </summary>
	/// <param name="version">Wersja.</param>
	/// <returns></returns>
	NodePtr getRoot(int & version) const
	{
		NodePtr result = nullptr;
		if (!getCorrectVersion(version))
			return result;
		for (RootVec::const_reverse_iterator it = _root.crbegin(); it != _root.crend(); ++it)
		{
			if (version >= it->first)
			{
				result = it->second;
				break;
			}
		}
		return result;
	}

	/// <summary>
	/// Ustawia najwieksza wartosc w lewym poddrzwie usuwanego wezla jako ten wezel.
	/// </summary>
	/// <param name="node">Wezel.</param>
	/// <param name="version">Wersja.</param>
	void setLargestValueInLeftSubtreeAsChild(NodePtr node)
	{
		NodePtr largestInLeftSubtree = node->getLeftChild(_version);
		bool found = false;
		while (!found)
		{
			NodePtr next = largestInLeftSubtree->getRightChild(_version);
			if (next != nullptr)
				largestInLeftSubtree = next;
			else
				found = true;
		}
		NodePtr largestInLeftSubtreeLeftChild = largestInLeftSubtree->getLeftChild(_version);
		Type * value = largestInLeftSubtree->getValue(_version);
		NodePtr largestInLeftSubtreeParent = getParentNode(*largestInLeftSubtree->getValue(_version), _version);

		if (largestInLeftSubtreeLeftChild != nullptr)
			setNewChildForMe(largestInLeftSubtreeParent, largestInLeftSubtreeLeftChild);
		else
		{
			if (largestInLeftSubtreeParent == node)
				setLeftChildAsNull(largestInLeftSubtreeParent);
			else
				setRightChildAsNull(largestInLeftSubtreeParent);
		}
		Type & val = *node->getValue(_version + 1);
		auto newNodeIt = find(val, _version + 1);
		if (newNodeIt != end())
		{
			node = newNodeIt.getNode();
			changeValue(node, value);
		}
	}

	/// <summary>
	/// Propaguje zmiany w drzewie po tym jak rodzic otrzyma nowe dziecko.
	/// </summary>
	/// <param name="firstParent">Rodzic w ktorym natepuje zmiana.</param>
	/// <param name="firstChild">Nowe dziecko rodzica.</param>
	void propagateChangesAfterInsert(NodePtr firstParent, NodePtr firstChild)
	{
		NodePtr currentParent = firstParent;
		NodePtr currentChild = firstChild;
		bool stop = false;
		do
		{
			if (currentParent == nullptr)
			{
				_root.push_back(std::pair<int, NodePtr>(_version + 1, currentChild));
				stop = true;
			}
			else
			{
				if (currentParent->getChangeType() == ChangeType::None)
				{
					Type * childValue = currentChild->getValue(_version);
					Type * parentValue = currentParent->getValue(_version);
					ChangeType type = orderFunctor(*childValue, *parentValue) ? ChangeType::LeftChild : ChangeType::RightChild;
					currentParent->setChange(type, currentChild, _version + 1);
					stop = true;
				}
				else
				{
					NodePtr newParent = makeCopy(currentParent, _version);
					if (orderFunctor(*currentChild->getValue(_version), *newParent->getValue(_version)))
						newParent->setLeftChild(currentChild);
					else
						newParent->setRightChild(currentChild);
					currentChild = newParent;
					currentParent = getParentNode(*currentChild->getValue(_version), _version);
				}
			}
		} while (!stop);
	}

	/// <summary>
	/// Alokuje pamiec na nowy wezel i zwraca go.
	/// </summary>
	/// <param name="value">Wartosc wezla.</param>
	/// <returns></returns>
	NodePtr allocateNode(Type & value)
	{
		NodePtr p = _allocator.allocate(1);
		Type * val = _typeAllocator.allocate(1);
		_typeAllocator.construct(val, value);
		_allocator.construct(p, *val);
		return p;
	}

	/// <summary>
	/// Dealokuje wezel z pamieci.
	/// </summary>
	/// <param name="p">Wskaznik do wezla.</param>
	void deallocateNode(Node<Type> * p)
	{
		if (p->getChangeType() == ChangeType::Value)
		{
			Type * changeVal = p->getChange().value;
			_typeAllocator.destroy(changeVal);
			_typeAllocator.deallocate(changeVal, 1);
		}
		Type * val = p->getValue(FIRST_VERSION);
		_typeAllocator.destroy(val);
		_typeAllocator.deallocate(val, 1);
		_allocator.destroy(p);
		_allocator.deallocate(p);
	}

	/// <summary>
	/// Dealokuje wszystkie wezly z pamieci.
	/// </summary>
	void deallocateNodes()
	{
		std::unordered_set<NodePtr> nodesToRemove;
		for (RootVec::iterator it = _root.begin(); it != _root.end(); ++it)
		{
			searchNodesToRemove(it->second, nodesToRemove);
		}
		int size = _allocator.getNodeCount();
		auto setSize = nodesToRemove.size();
		int ile = 1;
		for (auto it = nodesToRemove.begin(); it != nodesToRemove.end() ; ++it, ++ile)
		{
			deallocateNode(*it);
		}
	}

	/// <summary>
	/// Wyszukuje wszystkie wezly do usuniecia zaczynajac od przekazanego i dodaje je do zbioru wezlow do usuniecia.
	/// </summary>
	/// <param name="node">Wezel poczatkowy.</param>
	/// <param name="nodesToRemove">Zbior wezlow do usuniecia.</param>
	void searchNodesToRemove(NodePtr node, std::unordered_set<NodePtr> & nodesToRemove)
	{
		if (node == nullptr)
			return;
		auto rightChild = node->getRightChild(FIRST_VERSION);
		auto leftChild = node->getLeftChild(FIRST_VERSION);
		auto changeType = node->getChangeType();
		auto changedChild = changeType == ChangeType::LeftChild || changeType == ChangeType::RightChild ? node->getChange().child : nullptr;
		nodesToRemove.insert(node);
		if (rightChild != nullptr && nodesToRemove.find(rightChild) == nodesToRemove.end())
			searchNodesToRemove(rightChild, nodesToRemove);
		if (leftChild != nullptr && nodesToRemove.find(leftChild) == nodesToRemove.end())
			searchNodesToRemove(leftChild, nodesToRemove);
		if (changedChild != nullptr && nodesToRemove.find(changedChild) == nodesToRemove.end())
			searchNodesToRemove(changedChild, nodesToRemove);
	}
};