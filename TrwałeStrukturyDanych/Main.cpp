#include <iostream>
#include <cstdio>
#include "PersistentTree.h"

void findValue(PersistentTree<int> const & tree, int value, int version)
{
	std::cout << (tree.find(value, version) != nullptr ? "true" : "false") << std::endl;
}

void getCount(PersistentTree<int> const & tree, int version)
{
	std::cout << "Drzewo w wersji " << version << " zawiera " << tree.count(version) << " elementow." << std::endl;
}

int main()
{
	PersistentTree<int> tree;
	tree.insert(4);
	tree.insert(7);
	tree.insert(2);
	tree.insert(5);
	tree.insert(3);
	tree.insert(1);
	tree.erase(7);
	tree.erase(2);

	// wydrukowanie drzew na konsoli
	for (int i = 1; i <= 15; ++i)
	{
		tree.print(i);
		std::cout << std::endl;
	}

	// iteracja po najswiezszej wersji
	std::cout << "Najnowsza wersja: ";
	for (PersistentTree<int>::iterator it = tree.begin(); it != tree.end(); ++it)
	{
		std::cout << *it << ' ';
	}
	std::cout << std::endl;

	// iteracji po wersji historycznej
	std::cout << "Wersja nr 5: ";
	for (PersistentTree<int>::iterator it = tree.begin(5); it != tree.end(); it++)
	{
		std::cout << *it << ' ';
	}
	std::cout << std::endl;

	// wyszukiwanie po historii
	findValue(tree, 3, 5); // oczekiwane true
	findValue(tree, 1, 6); // oczekiwane true
	findValue(tree, 2, 2); // oczekiwane false
	findValue(tree, 5, 3); // oczekiwane false

	// zliczanie elementow
	getCount(tree, 3);	// oczekiwane 3
	getCount(tree, 5);	// oczekiwane 5

	getchar();
	return 0;
}