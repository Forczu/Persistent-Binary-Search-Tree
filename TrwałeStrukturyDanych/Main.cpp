#include <iostream>
#include <cstdio>
#include "PersistentTree.h"

int main()
{
	PersistentTree<int> tree;
	tree.insert(4);
	tree.insert(7);
	tree.insert(2);
	tree.insert(5);
	tree.insert(3);
	tree.insert(1);

	for (int i = 1; i <= 6; ++i)
	{
		tree.print(i);
		std::cout << std::endl;
	}

	return 0;
}