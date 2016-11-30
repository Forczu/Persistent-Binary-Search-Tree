#pragma once
#include <iterator>

/// <summary>
/// Iterator typu forward, s³u¿±cy do przechodzenia przez ca³e drzewo poszukiwañ binarnych we wskazanej wersji.
/// </summary>
template<class Type, class UnqualifiedType = std::remove_cv_t<Type>>
class PersistentTreeIterator : public std::iterator<std::forward_iterator_tag, UnqualifiedType, std::ptrdiff_t, Type*, Type&>
{
public:

	/// <summary>
	/// Preinkrementacja iteratora
	/// </summary>
	/// <returns></returns>
	PersistentTreeIterator& operator ++ ()
	{

	}

	/// <summary>
	/// Postinkrementacja iteratora
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	PersistentTreeIterator operator ++ (int)
	{

	}

	/// <summary>
	/// Operator równo¶ci
	/// </summary>
	/// <param name="rhs">Iterator do porównania.</param>
	/// <returns></returns>
	template<class OtherType>
	bool operator == (PersistentTreeIterator<OtherType> const & rhs) const
	{

	}

	/// <summary>
	/// Operator nierówno¶ci
	/// </summary>
	/// <param name="rhs">Iterator do porównania.</param>
	/// <returns></returns>
	template<class OtherType>
	bool operator != (PersistentTreeIterator<OtherType> const & rhs) const
	{

	}

	/// <summary>
	/// Operator dereferencji
	/// </summary>
	/// <returns></returns>
	Type & operator * () const
	{

	}

	/// <summary>
	/// Operator dostêpu do przechowywanej warto¶ci
	/// </summary>
	/// <returns></returns>
	Type & operator -> () const
	{

	}

};