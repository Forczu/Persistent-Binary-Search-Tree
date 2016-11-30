#pragma once
#include <iterator>

/// <summary>
/// Iterator typu forward, s�u��cy do przechodzenia przez ca�e drzewo poszukiwa� binarnych we wskazanej wersji.
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
	/// Operator r�wno�ci
	/// </summary>
	/// <param name="rhs">Iterator do por�wnania.</param>
	/// <returns></returns>
	template<class OtherType>
	bool operator == (PersistentTreeIterator<OtherType> const & rhs) const
	{

	}

	/// <summary>
	/// Operator nier�wno�ci
	/// </summary>
	/// <param name="rhs">Iterator do por�wnania.</param>
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
	/// Operator dost�pu do przechowywanej warto�ci
	/// </summary>
	/// <returns></returns>
	Type & operator -> () const
	{

	}

};