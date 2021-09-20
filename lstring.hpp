#pragma once
#ifndef LSTRING_HPP
#define LSTRING_HPP

#include <cstdlib>
#include <string>
#include <iostream>

template <
	class _Elem,
	class _Traits = std::char_traits<_Elem>,
	class _Allocator = std::allocator<_Elem>
>
class basic_lstring {

private:
	_Elem*      _Data;
	size_t      _Capacity;
	size_t      _Length;



	void ReAlloc(const _Elem* source, size_t bytes, size_t position = 0) {
		std::allocator<_Elem> _Alloc;
		const size_t newSize = position + bytes + 1;
		if (_Data == nullptr) {
			_Data = _Alloc.allocate(newSize);
			memset(_Data, 0, newSize); //can be optimized
			memcpy(_Data, source, bytes);
			_Capacity = newSize;
		}
		else {
			if (newSize <= _Capacity) {
				memcpy(_Data + position, source, bytes);
			}
			else {
				float tempCapacity = static_cast<float>(_Capacity);
				while (tempCapacity < newSize) {
					tempCapacity = ceil(tempCapacity * 1.5f);
				}

				const size_t newCapacity = static_cast<size_t>(tempCapacity);
				_Elem* oldData = _Data;
				_Data = _Alloc.allocate(newCapacity);
				memcpy(_Data, oldData, position);
				memcpy(_Data + position, source, bytes); //to check

				_Alloc.deallocate(oldData, _Capacity);
				_Capacity = newCapacity;
			}
		}
		_Length = position + _Traits::length(source); //long time
	}



public:
	basic_lstring() :
		_Data(nullptr),
		_Capacity(0),
		_Length(0) {}

	basic_lstring(const _Elem* _ptr) :
		_Data(nullptr),
		_Capacity(0),
		_Length(0) {
		size_t newSize = _Traits::length(_ptr);
		ReAlloc(_ptr, newSize);
	}

	basic_lstring(const basic_lstring& lstr) :
		_Data(nullptr),
		_Capacity(lstr._Capacity),
		_Length(lstr._Length) {
		_Allocator _Alloc;
		_Data = _Alloc.allocate(_Capacity);
		memcpy(_Data, lstr.c_str(), _Capacity);
	}

	basic_lstring(basic_lstring&& lstr) :
		_Data(lstr._Data),
		_Capacity(lstr._Capacity),
		_Length(lstr._Length) {
		lstr._Data = nullptr;
		lstr._Capacity = 0;
		lstr._Length = 0;
	}

	virtual ~basic_lstring() {
		_Allocator _Alloc;
		_Alloc.deallocate(_Data, _Capacity);
		_Data = nullptr;
		_Capacity = 0;
		_Length = 0;
	}



	void append(const _Elem* _ptr, size_t bytes) {
		ReAlloc(_ptr, bytes, _Length);
	}
	
	const _Elem* c_str() const {
		if (_Data != nullptr)
			return _Data;
		return "";
	}

	size_t capacity() const {
		return _Capacity;
	}

	void clear() {
		memset(_Data, 0, _Capacity); //OK???
		_Length = 0;
	}

	size_t length() const {
		return _Length;
	}

	void reserve(size_t newcap) {
		_Allocator _Alloc;
		if (newcap > _Length) {
			_Elem* oldData = _Data;
			
			_Data = _Alloc.allocate(newcap);
			memset(_Data, 0, newcap); //can be optimized
			if (oldData != nullptr) {
				memcpy(_Data, oldData, _Length);
				_Alloc.deallocate(oldData, _Capacity);
			}
			_Capacity = newcap;
		}
	}

	void shrink_to_fit() {
		_Allocator _Alloc;
		size_t newSize = _Length + 1;
		if (_Data != nullptr) {
			_Elem* oldData = _Data;
			_Data = _Alloc.allocate(newSize);
			memcpy(_Data, oldData, newSize);
			_Alloc.deallocate(oldData, _Capacity);
			_Capacity = newSize;
			
		}
		else {
			_Data = _Alloc.allocate(newSize); //to check
		}
	}



	basic_lstring& operator=(const _Elem* _ptr) {
		size_t newSize = _Traits::length(_ptr); //long time
		if(_Data != nullptr)
			memset(_Data + newSize, 0, _Capacity - newSize); //long time
		ReAlloc(_ptr, newSize);
		return *this;
	}

	basic_lstring& operator+=(const _Elem* _ptr) {
		size_t addSize = _Traits::length(_ptr); //long time
		append(_ptr, addSize);
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const basic_lstring& str) {
		os << str.c_str();
		return os;
	}
};



typedef    basic_lstring<char>       lstring;
typedef    basic_lstring<wchar_t>    lwstring;

#endif
