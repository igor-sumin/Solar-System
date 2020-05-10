#define _USE_MATH_DEFINES
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define NOMINMAX

#include <limits>
#include <cmath>
#include <algorithm>

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <cstdint>
#include <windows.h>

#include <vector>
#include <string>
#include <tuple>
#include <utility>

// Рейтресинг
#include "geometry.h"
#include "stb_image_write.h"
#include "stb_image.h"

// GUI
#include <SFML/Graphics.hpp>
#include "../PNG/GUI/GUI.h"

#define PI  3.14159265358979323846

int envmap_width, envmap_height;
std::vector<Vec3f> envmap;

struct Date {
	unsigned int day;
	unsigned int month;
	unsigned int year;
};

// Точечные источников освещения
struct Light {
	Light(const Vec3f &p, const float &i) : position(p), intensity(i) {}
	Vec3f position;
	float intensity;
};

// Одноцветные сферы
struct Material {
	float refractive_index;
	Vec2f albedo;
	Vec3f diffuse_color;
	float specular_exponent;

	Material() : refractive_index(1), albedo(1, 0), diffuse_color(), specular_exponent() {}

	Material(const float &r, const Vec2f &a, const Vec3f &color, const float &spec) : refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec) {}
};

// Двуцветные сферы
struct MaterialHard {
	float refractive_index;
	Vec2f albedo;
	std::pair<Vec3f, Vec3f> diffuse_color;
	float specular_exponent;

	MaterialHard() : refractive_index(1), albedo(1, 0), diffuse_color(), specular_exponent() {}

	MaterialHard(const float &r, const Vec2f &a, const std::pair<Vec3f, Vec3f> &color, const float &spec) : refractive_index(r), albedo(a), specular_exponent(spec) {
		diffuse_color.first = color.first;
		diffuse_color.second = color.second;
	}
};

struct Sphere {
	Vec3f center;
	float radius;
	Material material;

	Sphere(const Vec3f &c, const float &r, const Material& m) : center(c), radius(r), material(m) {}

	// работа с одной фигурой
	bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const {
		Vec3f L = center - orig;
		float tca = L * dir;

		float d2 = L * L - tca * tca;
		if (d2 > radius*radius)
			return false;

		float thc = sqrtf(radius*radius - d2);
		t0 = tca - thc;
		float t1 = tca + thc;

		if (t0 < 0)
			t0 = t1;

		if (t0 < 0)
			return false;

		return true;
	}
};

struct SphereHard {
	Vec3f center;
	float radius;
	MaterialHard materialHard;

	SphereHard(const Vec3f &c, const float &r, const MaterialHard& m) : center(c), radius(r) {
		materialHard.diffuse_color.first = m.diffuse_color.first;
		materialHard.diffuse_color.second = m.diffuse_color.second;
	}

	// работа с одной фигурой
	bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const {
		Vec3f L = center - orig;
		float tca = L * dir;

		float d2 = L * L - tca * tca;
		if (d2 > radius*radius)
			return false;

		float thc = sqrtf(radius*radius - d2);
		t0 = tca - thc;
		float t1 = tca + thc;

		if (t0 < 0)
			t0 = t1;

		if (t0 < 0)
			return false;

		return true;
	}
};

Vec3f reflect(const Vec3f &I, const Vec3f &N) {
	return I - N * 2.f*(I*N);
}

Vec3f refract(const Vec3f &I, const Vec3f &N, const float &refractive_index) { // Snell's law
	float cosi = -std::max(-1.f, std::min(1.f, I*N));
	float etai = 1, etat = refractive_index;
	Vec3f n = N;
	// если луч находится внутри объекта, меняем местами индексы и инвертируем нормаль, чтобы получить правильный результат
	if (cosi < 0) {
		cosi = -cosi;
		std::swap(etai, etat); n = -N;
	}
	float eta = etai / etat;
	float k = 1 - eta * eta*(1 - cosi * cosi);

	return k < 0 ? Vec3f(0, 0, 0) : I * eta + n * (eta * cosi - sqrtf(k));
};

// работа с n фигурами
bool scene_intersect(const Vec3f &orig, const Vec3f &dir, const std::vector<Sphere> &spheres, Vec3f &hit, Vec3f &N, Material &material) {

	float spheres_dist = 3.40282e+38;
	// для каждой Одноцветной сферы 
	for (size_t i = 0; i < spheres.size(); i++) {
		float dist_i;
		// если мы в области и попали в область i сферы
		if (spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist) {
			// L
			spheres_dist = dist_i;
			// tca
			hit = orig + dir * dist_i;
			// dir
			N = (hit - spheres[i].center).normalize();
			// узнаем цвет 
			material = spheres[i].material;
		}
	}

	// в доступной области
	return spheres_dist < 1000;
}

bool scene_intersectHard(const Vec3f &orig, const Vec3f &dir, const std::vector<SphereHard> &spheresHard, Vec3f &hit, Vec3f &N, MaterialHard &materialHard) {
	float spheres_dist_d = 3.40282e+38;
	// для каждой Двуцветной сферы 
	for (size_t i = 0; i < spheresHard.size(); i++) {
		float dist_i;
		// если мы в области и попали в область i сферы
		if (spheresHard[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist_d) {
			// L
			spheres_dist_d = dist_i;
			// tca
			hit = orig + dir * dist_i;
			// dir
			N = (hit - spheresHard[i].center).normalize();

			// узнаем цвет (!) Рандом
			materialHard = spheresHard[i].materialHard;
		}
	}

	return spheres_dist_d < 1000;
}

// исходящий из orig в направлении dir
Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const std::vector<Sphere> &spheres, const std::vector<Light> &lights, size_t depth, const Vec3f &col = Vec3f(0.2, 0.7, 0.8)) {
	Vec3f point, N;
	Material material;

	// Узнаем color
	if (depth > 4 || !scene_intersect(orig, dir, spheres, point, N, material)) {
		return col;
	}

	// Узнаем light
	float diffuse_light_intensity = 0, specular_light_intensity = 0;
	for (size_t i = 0; i < lights.size(); i++) {
		Vec3f light_dir = (lights[i].position - point).normalize();

		// Добавление теней
		float light_distance = (lights[i].position - point).norm();

		Vec3f shadow_orig = light_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
		Vec3f shadow_pt, shadow_N;
		Material tmpmaterial;
		if (scene_intersect(shadow_orig, light_dir, spheres, shadow_pt, shadow_N, tmpmaterial) && (shadow_pt - shadow_orig).norm() < light_distance)
			continue;

		diffuse_light_intensity += lights[i].intensity * std::max(0.f, light_dir * N);
		specular_light_intensity += powf(std::max(0.f, -reflect(-light_dir, N)*dir), material.specular_exponent)*lights[i].intensity;
	}

	return material.diffuse_color * diffuse_light_intensity * material.albedo[0] + Vec3f(1., 1., 1.)*specular_light_intensity * material.albedo[1];
}

// исходящий из orig в направлении dir
Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const std::vector<SphereHard> &spheresHard, const std::vector<Light> &lights, size_t depth, const Vec3f &col = Vec3f(0.2, 0.7, 0.8), int pos = 0) {
	Vec3f point, N;
	MaterialHard material;

	// Узнаем color
	if (depth > 4 || !scene_intersectHard(orig, dir, spheresHard, point, N, material)) {
		return col;
	}

	// Узнаем light
	float diffuse_light_intensity = 0, specular_light_intensity = 0;
	for (size_t i = 0; i < lights.size(); i++) {
		Vec3f light_dir = (lights[i].position - point).normalize();

		// Добавление теней
		float light_distance = (lights[i].position - point).norm();

		// проверка, находится ли точка в тени источников света [i]
		Vec3f shadow_orig = light_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
		Vec3f shadow_pt, shadow_N;
		MaterialHard tmpmaterialHard;
		if (scene_intersectHard(shadow_orig, light_dir, spheresHard, shadow_pt, shadow_N, tmpmaterialHard) && (shadow_pt - shadow_orig).norm() < light_distance)
			continue;

		diffuse_light_intensity += lights[i].intensity * std::max(0.f, light_dir * N);
		specular_light_intensity += powf(std::max(0.f, -reflect(-light_dir, N)*dir), material.specular_exponent)*lights[i].intensity;
	}

	Vec3f p = material.diffuse_color.first;

	std::mt19937 gen;
	std::uniform_int_distribution<> unif(0, pos / 2);
	if (unif(gen) < pos / 4) {
		return material.diffuse_color.second * diffuse_light_intensity * material.albedo[0] + Vec3f(1., 1., 1.)*specular_light_intensity * material.albedo[1];
	}
	return material.diffuse_color.first * diffuse_light_intensity * material.albedo[0] + Vec3f(1., 1., 1.)*specular_light_intensity * material.albedo[1];
}

void render(const std::vector<Sphere> &spheres, const std::vector<Light> &lights, const std::vector<SphereHard> &spheresHard = {}) {
	const int   width = 1024;
	const int   height = 768;
	const double fov = M_PI / 3.;
	std::vector<Vec3f> buf(width*height);

	// Итерации делятся на блоки на chunk (размеры блока) итераций и статически разделяются между потоками
	// Если chunk - неопределен, то итерации делятся между потоками равномерно и непрерывно
	// Итерации делятся на chunk и выполнение программы идет по блокам. Можно менять размер блока
#pragma omp parallel for
	for (size_t j = 0; j < height; j++) { // actual rendering loop
		for (size_t i = 0; i < width; i++) {
			double dir_x = (i + 0.5) - width / 2.;
			// это одновременно переворачивает изображение 
			double dir_y = -(j + 0.5) + height / 2.;
			double dir_z = -height / (2.*tan(fov / 2.));
			buf[i + j * width] = cast_ray(Vec3f(0, 0, 0), Vec3f(dir_x, dir_y, dir_z).normalize(), spheresHard, lights, 0, envmap[i + j * envmap_width], i + j * envmap_width);

			// Если не закрашивали
			if (buf[i + j * width].x == envmap[i + j * width].x && buf[i + j * width].y == envmap[i + j * width].y && buf[i + j * width].z == envmap[i + j * width].z) {
				buf[i + j * width] = cast_ray(Vec3f(0, 0, 0), Vec3f(dir_x, dir_y, dir_z).normalize(), spheres, lights, 0, envmap[i + j * envmap_width]);
			}
		}
	}

	std::vector<unsigned char> pixmap(width*height * 3);
	for (size_t i = 0; i < height*width; ++i) {
		Vec3f &c = buf[i];
		float max = std::max(c[0], std::max(c[1], c[2]));
		if (max > 1) c = c * (1. / max);
		for (size_t j = 0; j < 3; j++) {
			pixmap[i * 3 + j] = (unsigned char)(255 * std::max(0.f, std::min(1.f, buf[i][j])));
		}
	}

	std::cout << "done." << std::endl;
	// stbi_write_jpg(s, width, height, 3, pixels, 100);
	stbi_write_jpg("../result.jpg", width, height, 3, pixmap.data(), 100);
}

void CreateBackround() {
	int nrChannel;
	unsigned char *pixmap = stbi_load("../envmap.jpg", &envmap_width, &envmap_height, &nrChannel, 0);
	if (!pixmap || 3 != nrChannel) {
		std::cerr << "Error: can not load the environment map" << std::endl;
		system("error");
		exit(1);
	}

	envmap = std::vector<Vec3f>(envmap_width*envmap_height);
	for (int j = envmap_height - 1; j >= 0; j--) {
		for (int i = 0; i < envmap_width; i++) {
			envmap[i + j * envmap_width] = Vec3f(pixmap[(i + j * envmap_width) * 3 + 0], pixmap[(i + j * envmap_width) * 3 + 1], pixmap[(i + j * envmap_width) * 3 + 2])*(1 / 255.);
		}
	}
	stbi_image_free(pixmap);
}

float ConvertUnit(Date& date) {
	// 11 Июня 1128 - все в ряд

	// 11.06.1128 отправная точка
	int days_point = 11 + 6 * 30 + 1128 * 365;

	int month = 0;
	switch (date.month) {
		case 1:case 3:case 5:case 7:case 8:case 10:case 12:
			month = 31;
		case 2:
			month = 28;
		case 4:case 6:case 9:case 11:
			month = 30;
		default:
			break;
	}

	int days_date = date.day + date.month * month + date.year * 365;

	return (days_date - days_point) / 10;
}

int getMonth(Date& date, std::string month) {
	switch (month[0]) {
	case 'J':
		if (month[1] == 'a') {
			return 1;
		}
		else if (month[2] == 'n') {
			return 6;
		}
		else {
			return 7;
		}
	case 'F':
		return 2;
	case 'M':
		if (month[2] == 'r') {
			return 3;
		}
		else {
			return 5;
		}
	case 'A':
		if (month[1] == 'p') {
			return 4;
		}
		else {
			return 8;
		}
	case 'S':
		return 9;
	case 'O':
		return 10;
	case 'N':
		return 11;
	case 'D':
		return 12;
	default:
		std::string s = "unknown month: " + month;
		// throw std::runtime_error(s);
	}
}

Date ParseDate() {
	// Кладем из файла output.txt данные в год, месяц, число
	const std::string path = "../input.txt";
	std::ifstream input(path);

	Date date;
	std::string smonth;

	if (input) {
		input >> date.day;
		input.ignore(1);

		input >> smonth;
		date.month = getMonth(date, smonth.substr(0, 3));

		input.ignore(1);
		input >> date.year;
		// (?) input.ignore(1);

	}
	else {
		std::string s = "not found path: " + path;
		throw std::runtime_error(s);
	}

	std::cout << "date_main = " << date.day << " " << date.month << " " << date.year << std::endl;
	return date;
}

int main() {
	while (true) {
		float y = 0;

		// Вызываем Графический интерфейс
		GUI gui;
		// gui.main1();
		gui.Create();

		const std::string path = "../input.txt";
		std::ifstream input(path);
		std::string line;
		while (getline(input, line)) {
			std::cout << "line in main: " << line << std::endl;
		}

		// Обрабатываем корректную запись пользователя
		Date date = ParseDate();

		// Загружаем фон
		CreateBackround();

		// x0, y0, z0
		std::vector<double> sun_param = { 0, 0, -65 };

		// rad, omega
		std::vector<double> mer_param = { 0.29, 4.15 };
		std::vector<double> ven_param = { 0.605, 1.54 };
		std::vector<double> earth_param = { 0.64, 1. };

		std::vector<double> mars_param = { 0.34, 0.53 };
		std::vector<double> jupiter_param = { 3.56, 0.86 };
		std::vector<double> saturn_param = { 3., 0.034 };
		std::vector<double> uran_param = { 1.225, 0.012 };
		std::vector<double> neptune_param = { 1.255, 0.006 };
		std::vector<double> pluto_param = { 0.14, 0.004 };

		// Характеристики планет
		std::vector<std::vector<double>> origin_param = { mer_param, ven_param, earth_param, mars_param, jupiter_param, saturn_param, uran_param, neptune_param, pluto_param };

		Material    sun(2.0, Vec2f(0.9, 0.4), Vec3f(0.8, 0.5, 0.), 5.);
		Material mercury(1.0, Vec2f(0.9, 0.1), Vec3f(109, 110, 133) * (1 / 255.), 20);
		Material venus(1.0, Vec2f(0.9, 0.1), Vec3f(250, 234, 193) * (1 / 255.), 20);
		MaterialHard earth(1.0, Vec2f(0.0, 10.0), std::make_pair(Vec3f(0.3, 0.8, 0.13), Vec3f(0.13, 0.24, 0.8)), 20);
		MaterialHard mars(1.5, Vec2f(1.0, 10.0), std::make_pair(Vec3f(1., 0.27, 0.), Vec3f(0.502, 0., 0.)), 60);
		MaterialHard jupiter(1.5, Vec2f(1.0, 10.0), std::make_pair(Vec3f(0.54, 0.27, 0.07), Vec3f(0.95, 0.64, 0.37)), 60);
		MaterialHard saturn(1.5, Vec2f(1.0, 10.0), std::make_pair(Vec3f(1, 0.87, 0.67), Vec3f(0.96, 0.87, 0.7)), 60);
		MaterialHard uran(1.5, Vec2f(1.0, 10.0), std::make_pair(Vec3f(0, 0.74, 1.), Vec3f(0.11, 0.56, 1.)), 30);
		Material neptune(1.5, Vec2f(1.0, 10.0), Vec3f(0., 0., 1.), 80);
		Material pluto(1.5, Vec2f(1.0, 10.0), Vec3f(1., 0.89, 0.88), 30);

		std::vector<Material> materials = { mercury, venus, neptune, pluto };
		std::vector<MaterialHard> materialsHard = { earth, mars, jupiter, saturn, uran };

		std::vector<Light>  lights;
		std::vector<Sphere> spheres;
		std::vector<SphereHard> spheresHard;

		double unit = ConvertUnit(date);
		std::cout << "unit = " << unit << std::endl;

		std::vector<double> rad_orb = { 7.29, 9.185, 11.43, 13.41, 18.31, 26.77, 33.095, 36.575, 38.97 };

		// верхний
		lights.push_back(Light(Vec3f(sun_param[0], y + 10, sun_param[2] + 13), 1.5));
		// левый
		lights.push_back(Light(Vec3f(sun_param[0] - 10, y, sun_param[2]), 1.5));
		// правый
		lights.push_back(Light(Vec3f(sun_param[0] + 10, y, sun_param[2]), 1.5));
		// задний
		lights.push_back(Light(Vec3f(sun_param[0], y, sun_param[2] - 10 - .1), 1.5));
		// нижний
		lights.push_back(Light(Vec3f(sun_param[0], y - 10, sun_param[2] + 13), 1.5));

		// Солнце
		spheres.push_back(Sphere(Vec3f(sun_param[0], sun_param[1], sun_param[2]), 5, sun));

		// Меркурий, Венера
		for (int i = 0; i < 2; i++) {
			double x1 = sun_param[0] + rad_orb[i] * cos(origin_param[i][1] * unit / 57.3);
			double z1 = sun_param[2] + rad_orb[i] * sin(origin_param[i][1] * unit / 57.3);

			spheres.push_back(Sphere(Vec3f(x1, 0., z1), origin_param[i][0], materials[i]));
			std::cout << "num planet: " << i << std::endl;
		}

		// Земля, Марс, Юпитер, Сатурн, Уран
		for (int i = 2; i < 7; i++) {
			double x1 = sun_param[0] + rad_orb[i] * cos(origin_param[i][1] * unit / 57.3);
			double z1 = sun_param[2] + rad_orb[i] * sin(origin_param[i][1] * unit / 57.3);

			spheresHard.push_back(SphereHard(Vec3f(x1, 0., z1), origin_param[i][0], materialsHard[i - 2]));
			std::cout << "num planet: " << i << std::endl;
		}

		// Нептун, Плутон
		for (int i = 7; i < 9; i++) {
			double x1 = sun_param[0] + rad_orb[i] * cos(origin_param[i][1] * unit / 57.3);
			double z1 = sun_param[2] + rad_orb[i] * sin(origin_param[i][1] * unit / 57.3);

			spheres.push_back(Sphere(Vec3f(x1, 0., z1), origin_param[i][0], materials[i - 5]));
			std::cout << "num planet: " << i << std::endl;
		}

		// Вывод
		render(spheres, lights, spheresHard);

		for (int i = 0; i < 2; i++) {
			spheres.pop_back();
		}
		for (int i = 2; i < 7; i++) {
			spheresHard.pop_back();
		}
		for (int i = 7; i < 9; i++) {
			spheres.pop_back();
		}

		gui.openIs("../result.jpg");
	}

	system("pause");
	return 0;
}

// C:\Users\Asus\Desktop\CodeC\C++\Visual Studio\PNG\x64\Release