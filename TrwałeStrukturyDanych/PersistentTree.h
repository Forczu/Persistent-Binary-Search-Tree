#pragma once
#include "PersistentTreeIterator.h"
#include <set>

/// <summary>
/// Klasa reprezentuj�ca trwa�e drzewo poszukiwa� binarnych.
/// Zastosowany algorytm to metoda Sleatora, Tarjana i innych
/// Parametr szablonowy Type okre�la typ danych, jaki przechowywany w drzewie
/// </summary>
template<class Type>
class PersistentTree
{	
	/// <summary>
	/// Aktualna wersja drzewa. Zaczyna si� od jedynki, ka�da nowa wersja skutkuje inkrementacj� tej warto�ci
	/// </summary>
	int _version;

public:
	typedef PersistentTreeIterator<Type> iterator;
	typedef PersistentTreeIterator<const Type> const_iterator;

	/// <summary>
	/// Tworzy nowe, puste drzewo bez historii.
	/// </summary>
	PersistentTree() : _version(1)
	{

	}

	/// <summary>
	/// Zwraca iterator na pocz�tek aktualnego drzewa
	/// </summary>
	/// <param name="version">Wersja drzewa, po kt�rej nale�y iterowa�. Zero oznacza wersj� aktualn�</param>
	/// <returns></returns>
	iterator begin(int version = 0)
	{

	}
	
	/// <summary>
	/// Usuwa zawarto�� drzewa wraz z histori�
	/// </summary>
	void clear()
	{

	}
	
	/// <summary>
	/// Zlicza liczb� element�w we wskazanej wersji drzewa.
	/// </summary>
	/// <param name="version">Wersja drzewa, po kt�rej nale�y iterowa�. Zero oznacza wersj� aktualn�</param>
	/// <returns></returns>
	int count(int version = 0)
	{
		
	}

	/// <summary>
	/// Zwraca koniec akrualnego drzewa
	/// </summary>
	/// <param name="version">Wersja drzewa, po kt�rej nale�y iterowa�. Zero oznacza wersj� aktualn�</param>
	/// <returns></returns>
	iterator end(int version = 0)
	{
		
	}
	
	/// <summary>
	/// Usuwa element o podanej warto�ci z drzewa. Skutkuje utworzeniem nowej wersji drzewa.
	/// </summary>
	/// <param name="value">Warto�� do usuni�cia.</param>
	void erase(Type value)
	{

	}
	
	/// <summary>
	/// Wyszukuje podan� warto�� w drzewie o wskazanej wersji. Zero oznacza wersj� aktualn� drzewa.
	/// Je�eli warto�ci nie ma w drzewie o podanej wersji, zwracany jest iterator na koniec drzewa.
	/// </summary>
	/// <param name="value">Warto�� do wyszukania.</param>
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
	/// Umieszcza nowy element w drzewie poszukiwa�. Skutkuje utworzeniem nowej wersji drzewa.
	/// </summary>
	/// <param name="value">Warto�� do umieszczenia.</param>
	void insert(Type value)
	{

	}
};