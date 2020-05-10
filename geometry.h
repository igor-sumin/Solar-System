#pragma once
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

template <size_t DIM, typename Tname > struct vec  // Конструктор. Создали шаблонную структуру вектор и заполнили его память местом 
{												   //  заданного типа(Tname) и заданной длины(DIM - размерность), но ничем не заполняли
	vec()
	{
		for (size_t i = DIM; i--; data_[i] = Tname());
	}
	Tname& operator[](const size_t i)   // Перегрузка оператора индексирования
	{
		assert(i < DIM);
		return data_[i];
	}
	const Tname& operator[](const size_t i) const
	{
		assert(i < DIM);
		return data_[i];
	}
private:
	Tname data_[DIM];   // Выделили одно поле - массив значений( т.к. вектор) типа Tname
};

typedef vec<2, float> Vec2f;  // Переименовали шаблонные вектора в понятные обозначения
typedef vec<3, float> Vec3f;

template <typename Tname> struct vec<2, Tname>  // Конструктор и оператор индексирования для 2-го вектора
{
	vec() : x(Tname()), y(Tname()) {}
	vec(Tname X, Tname Y) : x(X), y(Y) {}
	template <class U> vec<2, Tname>(const vec<2, U> &v);
	Tname& operator[](const size_t i)
	{
		assert(i < 2);
		return i <= 0 ? x : y;
	}
	const Tname& operator[](const size_t i) const
	{
		assert(i < 2);
		return i <= 0 ? x : y;
	}
	Tname x, y;  // Поля вектора
};

template <typename Tname> struct vec<3, Tname> // Конструктор и оператор индексирования для 3-го вектора
{
	vec() : x(Tname()), y(Tname()), z(Tname()) {}
	vec(Tname X, Tname Y, Tname Z) : x(X), y(Y), z(Z) {}
	Tname& operator[](const size_t i)
	{
		assert(i < 3);
		return i <= 0 ? x : (1 == i ? y : z);
	}
	const Tname& operator[](const size_t i) const
	{
		assert(i < 3);
		return i <= 0 ? x : (1 == i ? y : z);
	}
	float norm()
	{
		return std::sqrt(x*x + y * y + z * z);
	}
	vec<3, Tname> & normalize(Tname l = 1)
	{
		*this = (*this)*(l / norm()); return *this;
	}
	Tname x, y, z; // Поля вектора
};

template<size_t DIM, typename Tname> Tname operator*(const vec<DIM, Tname>& lhs, const vec<DIM, Tname>& rhs) // Скалярное произведение
{
	Tname ret = Tname();
	for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);
	return ret;
}

template<size_t DIM, typename Tname>vec<DIM, Tname> operator+(vec<DIM, Tname> lhs, const vec<DIM, Tname>& rhs)  // Сложение
{
	for (size_t i = DIM; i--; lhs[i] += rhs[i]);
	return lhs;
}

template<size_t DIM, typename Tname>vec<DIM, Tname> operator-(vec<DIM, Tname> lhs, const vec<DIM, Tname>& rhs) // Вычитание
{
	for (size_t i = DIM; i--; lhs[i] -= rhs[i]);
	return lhs;
}

template<size_t DIM, typename Tname, typename U> vec<DIM, Tname> operator*(const vec<DIM, Tname> &lhs, const U& rhs) // Векторное-произведеие
{
	vec<DIM, Tname> ret;
	for (size_t i = DIM; i--; ret[i] = lhs[i] * rhs);
	return ret;
}

template<size_t DIM, typename Tname> vec<DIM, Tname> operator-(const vec<DIM, Tname> &lhs)  // Вычитание
{
	return lhs * Tname(-1);
}

template <typename Tname> vec<3, Tname> cross(vec<3, Tname> v1, vec<3, Tname> v2)  // 3-х мерного вектора
{
	return vec<3, Tname>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

template <size_t DIM, typename Tname> std::ostream& operator<<(std::ostream& out, const vec<DIM, Tname>& v) // Вывод
{
	for (unsigned int i = 0; i < DIM; i++) {
		out << v[i] << " ";
	}
	return out;
}
#endif //__GEOMETRY_H__
