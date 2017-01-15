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

using namespace std;
using namespace std::chrono;

void findValue(PersistentTree<int> const & tree, int value, int version)
{
	std::cout << (tree.find(value, version) != tree.end() ? "true" : "false") << std::endl;
}

void getCount(PersistentTree<int> const & tree, int version)
{
	std::cout << "Drzewo w wersji " << version << " zawiera " << tree.size(version) << " elementow." << std::endl;
}

// ===== Testy na stringach ===== //
void stringTests()
{
	set<string> polishSet;

	std::ifstream polish("polish.txt");
	std::istream_iterator<std::string> my_it(polish);
	std::istream_iterator<std::string> eof;

	// Kopiowanie
	vector<string> vec;
	copy(my_it, eof, back_inserter(vec));

	// ----- std::set
	// Wstawianie
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	high_resolution_clock::time_point clk1 = high_resolution_clock::now();
	copy(vec.begin(), vec.end(), inserter(polishSet, polishSet.begin()));
	high_resolution_clock::time_point clk2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wstawianie do seta polskiego slownika: " << time_span.count() << " sekund" << endl;

	// Pamiec
	auto size = sizeof(std::set<string>) + (sizeof(string) * polishSet.size());
	cout << "Seta polskiego slownika zajmuje: " << size << " bajtow" << endl;

	// Wyszukiwanie
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
	cout << "Wyszukiwanie 100k polskich w secie: " << time_span.count() << " sekund" << endl;

	// Usuwanie
	vector<string> vecDelete;
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec) {
		polishSet.erase(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Usuwanie z seta: " << time_span.count() << " sekund" << endl << endl;

	// ----- PersistentTree
	// Wstawianie
	PersistentTree<string> tree;
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec) {
		tree.insert(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wstawianie do drzewa stringow: " << time_span.count() << " sekund" << endl;

	// Pamiec
	size = sizeof(PersistentTree<string>) + (sizeof(string) * tree.size_of_history());
	cout << "Drzewo zajmuje: " << size << " bajtow" << endl;

	// Wyszukiwanie
	vec100k.clear();
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
	cout << "Wyszukiwanie 100k w drzewie stringow: " << time_span.count() << " sekund" << endl;

	// Usuwanie
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec) {
		tree.erase(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Usuwanie z drzewa stringow: " << time_span.count() << " sekund" << endl << endl;
}

// ===== Testy na intach ===== //
void intTests()
{
	set<int> intSet;

	// Kopiowanie
	int million = 1000000;
	vector<int> vec(million);
	for (int i = 0; i < million; i++)
	{
		vec[i] = i;
	}

	// ----- std::set
	// Wstawianie
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	high_resolution_clock::time_point clk1 = high_resolution_clock::now();
	copy(vec.begin(), vec.end(), inserter(intSet, intSet.begin()));
	high_resolution_clock::time_point clk2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wstawianie do seta intow: " << time_span.count() << " sekund" << endl;

	// Pamiec
	auto size = sizeof(std::set<int>) + (sizeof(int) * intSet.size());
	cout << "Seta polskiego slownika zajmuje: " << size << " bajtow" << endl;

	// Wyszukiwanie
	vector<int> vec100k;
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	vec100k.insert(vec100k.begin(), vec.begin(), vec.begin() + 100000);
	shuffle(vec100k.begin(), vec100k.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec100k) {
		intSet.find(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wyszukiwanie 100k intow w secie: " << time_span.count() << " sekund" << endl;

	// Usuwanie
	vector<string> vecDelete;
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec) {
		intSet.erase(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Usuwanie intow z seta: " << time_span.count() << " sekund" << endl << endl;

	// ----- PersistentTree
	// Wstawianie
	PersistentTree<int> tree;
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec) {
		tree.insert(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Wstawianie do drzewa intow: " << time_span.count() << " sekund" << endl;

	// Pamiec
	size = sizeof(PersistentTree<int>) + (sizeof(int) * tree.size_of_history());
	cout << "Drzewo zajmuje: " << size << " bajtow" << endl;

	// Wyszukiwanie
	vec100k.clear();
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
	cout << "Wyszukiwanie 100k w drzewie intow: " << time_span.count() << " sekund" << endl;

	// Usuwanie
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
	clk1 = high_resolution_clock::now();
	for (auto x : vec) {
		tree.erase(x);
	}
	clk2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(clk2 - clk1);
	cout << "Usuwanie z drzewa intow: " << time_span.count() << " sekund" << endl << endl;
}

int main()
{
	stringTests();
	intTests();
	getchar();
	return 0;
}