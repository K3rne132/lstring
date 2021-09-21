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
		_Allocator _Alloc;
		const size_t newSize = position + bytes + 1;
		if (_Data != nullptr) {
			if (newSize <= _Capacity) {
				memcpy(_Data + position, source, bytes);
				_Data[position + bytes] = 0;
			}
			else {
				const size_t newCapacity = static_cast<const size_t>(
					ceil(static_cast<float>(newSize) * 1.5f));

				_Elem* oldData = _Data;
				_Data = _Alloc.allocate(newCapacity);
				memcpy(_Data, oldData, position);
				memcpy(_Data + position, source, bytes);
				_Data[position + bytes] = 0;

				_Alloc.deallocate(oldData, _Capacity);
				_Capacity = newCapacity;
			}
		}
		else {
			_Data = _Alloc.allocate(newSize);
			memcpy(_Data, source, bytes);
			_Data[bytes] = 0;
			_Capacity = newSize;
		}
		_Length = position + bytes;
	}



public:
	basic_lstring() : //OK
		_Data(nullptr),
		_Capacity(0),
		_Length(0) {}

	basic_lstring(const _Elem* _ptr) : //900% slower
		_Data(nullptr),
		_Capacity(0),
		_Length(0) {
		_Allocator _Alloc;
		size_t newSize = _Traits::length(_ptr) + 1;
		_Data = _Alloc.allocate(newSize);
		memcpy(_Data, _ptr, newSize);
		_Capacity = newSize;
		_Length = newSize - 1;
	}

	basic_lstring(const basic_lstring& lstr) ://2700% slower
		_Data(nullptr),
		_Capacity(lstr._Capacity),
		_Length(lstr._Length) {
		_Allocator _Alloc;
		_Data = _Alloc.allocate(_Capacity);
		memcpy(_Data, lstr.c_str(), _Capacity);
	}

	basic_lstring(basic_lstring&& lstr) : //OK
		_Data(lstr._Data),
		_Capacity(lstr._Capacity),
		_Length(lstr._Length) {
		lstr._Data = nullptr;
		lstr._Capacity = 0;
		lstr._Length = 0;
	}

	virtual ~basic_lstring() { //OK
		if (_Data != nullptr) {
			_Allocator _Alloc;
			_Alloc.deallocate(_Data, _Capacity);
			_Data = nullptr;
			_Capacity = 0;
			_Length = 0;
		}
	}



	void append(const _Elem* _ptr, size_t bytes) { //200% slower
		size_t ptrLen = 1;
		if (ptrLen > bytes)
			ReAlloc(_ptr, bytes, _Length);
		else
			ReAlloc(_ptr, ptrLen, _Length);
	}
	
	const _Elem* c_str() const { //OK
		if (_Data != nullptr)
			return _Data;
		return "";
	}

	size_t capacity() const { //OK
		return _Capacity;
	}

	void clear() { //OK
		if (_Data != nullptr)
			_Data[0] = 0;
		_Length = 0;
	}

	size_t length() const { //OK
		return _Length;
	}

	void reserve(size_t newcap) { //OK
		if (newcap > _Capacity) {
			_Allocator _Alloc;
			_Elem* oldData = _Data;
			
			_Data = _Alloc.allocate(newcap);
			_Data[_Length] = 0;
			if (oldData != nullptr) {
				memcpy(_Data, oldData, _Length);
				_Alloc.deallocate(oldData, _Capacity);
			}
			_Capacity = newcap;
		}
	}

	void shrink_to_fit() { //OK
		size_t newSize = _Length + 1;
		if (newSize < _Capacity) {
			_Allocator _Alloc;
			if (_Data != nullptr) {
				_Elem* oldData = _Data;
				_Data = _Alloc.allocate(newSize);
				memcpy(_Data, oldData, newSize);
				_Alloc.deallocate(oldData, _Capacity);
				_Capacity = newSize;

			}
			else {
				_Data = _Alloc.allocate(newSize);
				_Data[0] = 0;
			}
		}
	}



	basic_lstring& operator=(const _Elem* _ptr) { //10% slower
		size_t newSize = _Traits::length(_ptr);
		ReAlloc(_ptr, newSize);
		return *this;
	}

	basic_lstring& operator+=(const _Elem* _ptr) { //300% slower
		size_t addSize = _Traits::length(_ptr);
		ReAlloc(_ptr, addSize, _Length);
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const basic_lstring& str) { //OK
		os << str.c_str();
		return os;
	}
};



typedef    basic_lstring<char>       lstring;
typedef    basic_lstring<wchar_t>    lwstring;

#endif
