#include <iostream>
#include <cstdio>
#include "PersistentTree.h"
#include <chrono>
#include <algorithm>
#include <array>
#include <set>
#include <fstream>
#include <random>
#include <string>
#include <iostream>
#include <set>
#include <unordered_set>
#include <ratio>
#include <chrono>
#include <iterator>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <array>

void findValue(PersistentTree<int> const & tree, int value, int version)
{
	std::cout << (tree.find(value, version) != tree.end() ? "true" : "false") << std::endl;
}

void getCount(PersistentTree<int> const & tree, int version)
{
	std::cout << "Drzewo w wersji " << version << " zawiera " << tree.size(version) << " elementow." << std::endl;
}

using namespace std;
using namespace std::chrono;
int main()
{

	set<string> polishSet;

	std::ifstream polish("polish.txt");
	std::istream_iterator<std::string> my_it(polish);
	std::istream_iterator<std::string> eof;

	//Kopiowanie

	vector<string> vec;

	copy(my_it, eof, back_inserter(vec));
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	high_resolution_clock::time_point clk1 = high_resolution_clock::now();
	copy(vec.begin(), vec.end(), inserter(polishSet, polishSet.begin()));
	high_resolution_clock::time_point clk2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wstawianie do seta polskiego slownika: " << time_span.count() << "sekund" << endl;


	vector<string> vec100k;
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
    vec100k.insert(vec100k.begin(), vec.begin(), vec.begin() + 100000);
	shuffle(vec100k.begin(), vec100k.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec100k) {
		polishSet.find(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wyszukiwanie 100k polskich w secie: " << time_span.count() << "sekund" << endl;

	//Usuwanie
	vector<string> vecDelete;
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec) {
		polishSet.erase(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Usuwanie z seta: " << time_span.count() << "sekund" << endl;

	
	vector<int> vecInt;
	vecInt.push_back(3);
	vecInt.push_back(2);
	vecInt.push_back(4);
	vecInt.push_back(5);
	clk1 = high_resolution_clock::now();
	PersistentTree<int> tree(vecInt.begin(), vecInt.end());
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wstawianie do drzewa stringow: " << time_span.count() << "sekund" << endl;

	/*vec100k.clear();
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	vec100k.insert(vec100k.begin(), vec.begin(), vec.begin() + 100000);
	shuffle(vec100k.begin(), vec100k.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec100k) {
		tree.find(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wyszukiwanie 100k w drzewie stringow: " << time_span.count() << "sekund" << endl;*/

	/*PersistentTree<int> tree;
	tree.insert(4);
	tree.insert(7);
	tree.insert(2);
	tree.insert(5);
	tree.insert(3);
	tree.erase(2);
	tree.erase(3);
	tree.erase(7);

	// wydrukowanie drzew na konsoli
	for (int i = 1; i <= 15; ++i)
	{
		tree.print(i);
	}
	std::cout << std::endl;

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
	findValue(tree, 1, 6); // oczekiwane false
	findValue(tree, 2, 2); // oczekiwane false
	findValue(tree, 5, 3); // oczekiwane false

	// zliczanie elementow
	getCount(tree, 3);	// oczekiwane 3
	getCount(tree, 5);	// oczekiwane 5

	// drzewo zbudowane z wektora wartosci
	std::vector<int> input = { 4, 7, 2, 5, 3, 1 };
	std::cout << std::endl << "Wektor wejsciowy: ";
	for (int value : input)
		std::cout << value << ' ';
	std::cout << std::endl;
	PersistentTree<int> tree2(input.begin(), input.end());
	std::cout << "Drzewo na podstawie powyzszewgo wektora: " << std::endl;
	tree2.print();
	tree2.insert(8);
	tree2.insert(6);
	tree2.insert(0);
	std::cout << "Wersja nr 2: " << std::endl;
	tree2.print(2);
	std::cout << std::endl;

	// wyczyszczenie jako nowa zmiana
	std::cout << "Najnowsza wersja drzewa po czystce: " << std::endl;
	tree2.clear();
	tree2.print();
	std::cout << std::endl;
	std::cout << "Najnowsza wersja drzewa po czystce i kilku insertach: " << std::endl;
	tree2.insert(5);
	tree2.insert(2);
	tree2.insert(7);
	tree2.insert(6);
	tree2.insert(9);
	tree2.insert(1);
	tree2.print();
	std::cout << std::endl;
	std::cout << "Pierwsza wersja drzewa przed czystka: " << std::endl;
	tree2.print(3);
	std::cout << std::endl;

	// Kopia
	std::cout << "Kopia powyzszego drzewa: " << std::endl;
	auto copy = tree2.getCopy(3);
	copy->print(0);
	std::cout << std::endl;
	delete copy;

	// Drzewo o przeciwnym porzadku
	PersistentTree<int, std::greater<int>> treeInv;
	treeInv.insert(4);
	treeInv.insert(7);
	treeInv.insert(2);
	treeInv.insert(5);
	treeInv.insert(3);
	treeInv.insert(1);
	treeInv.insert(8);
	treeInv.erase(2);
	treeInv.erase(3);
	treeInv.erase(7);
	std::cout << "Historia drzewa o odwroconym porzadku: " << std::endl;
	// wydrukowanie drzew na konsoli
	for (int i = 1; i <= 11; ++i)
	{
		treeInv.print(i);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	// wyszukanie i iterowanie
	std::cout << "Iteracja po drzewie w wersji 7 od elementu '5'" << std::endl;
	for (auto it = treeInv.find(5, 7); it != treeInv.end(); ++it)
	{
		std::cout << *it << ' ';
	}
	std::cout << std::endl;
	*/
	getchar();
	return 0;
}