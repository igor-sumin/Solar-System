#pragma once

#include "Textbox.h"
#include "Button.h"



class GUI {
public:
	inline void delay(clock_t sec)
	{
		clock_t start_time = clock();
		clock_t end_time = sec + start_time;
		while (clock() != end_time);
	}

	void Create() {
		// Подключение юнит-тестов
		sf::RenderWindow window(sf::VideoMode(1024, 768), "Space", sf::Style::None);
		bool isFUllScreen = true;
		sf::Vector2i centerWindow((sf::VideoMode::getDesktopMode().width / 2) - 445, (sf::VideoMode::getDesktopMode().height / 2) - 480);
		window.setKeyRepeatEnabled(true);

		sf::Font font;
		font.loadFromFile("../Montserrat-Medium.ttf");
		Textbox text1(30, sf::Color::White, true);
		text1.setPosition({ 80, 180 });
		text1.setLimit(true, 50);
		text1.setFont(font);

		Button btn1("Enter", { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
		btn1.setFont(font);
		btn1.setPosition({ 200, 600 });

		Button btn2("Exit", { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
		btn2.setFont(font);
		btn2.setPosition({ 400, 600 });

		Button btn3("Retry", { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
		btn3.setFont(font);
		btn3.setPosition({ 600, 600 });

		sf::Text text;
		text.setString("Enter Date of moving planet");
		text.setCharacterSize(30);
		text.setFillColor(sf::Color::White);
		text.setStyle(sf::Text::Regular);

		Button btn4("Enter Date of moving planet", { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
		btn4.setFont(font);
		btn4.setPosition({ 193, 35 });

		Button btn6(" ", { 800, 50 }, 30, sf::Color::Black, sf::Color::Transparent);
		btn6.setFont(font);
		btn6.setPosition({ 70, 175 });

		Button btn7("Example: 10 January 2000", { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
		btn7.setFont(font);
		btn7.setPosition({ 170, 80 });



		Button btn_1(" ", { 800, 50 }, 30, sf::Color::Black, sf::Color::Transparent);
		btn_1.setFont(font);
		btn_1.setPosition({ 70, 300 });

		Button btn_2("Please waiting...", { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
		btn_2.setFont(font);
		btn_2.setPosition({ 90, 278 });

		sf::Texture texture;
		texture.loadFromFile("../background.jpg");
		sf::Sprite sprite;
		sf::Vector2u size = texture.getSize();
		sprite.setTexture(texture);
		sprite.setPosition(sf::Vector2f(0, 0));

		sf::Texture texture1;
		texture1.loadFromFile("../background.jpg");
		sf::Sprite sprite1;
		sf::Vector2u size1 = texture1.getSize();
		sprite1.setTexture(texture1);
		sprite1.setPosition(sf::Vector2f(0, 0));

		stringstream ss;

		int time = 0;
		bool check_ex = true;
		bool check_ex1 = false;
		bool flag = false;
		bool fla = true;
		while (window.isOpen()) {

			sf::Event Event;

			if (fla) {
				fla = false;
				window.clear();
				window.draw(sprite1);
				window.display();
			}



			bool k = false;
			int p = 0;


			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
				text1.setSelected(true);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				text1.setSelected(false);
			}
			do {
				flag = false;
				//Event Loop:
				while (window.pollEvent(Event)) {
					switch (Event.type) {
					case sf::Event::Closed:
						window.close();
						clear_file("../input.txt");
						exit(1);
					case sf::Event::TextEntered: {
						text1.typedOn(Event);
					}

					case sf::Event::MouseMoved:
						if (btn1.isMouseOver(window)) {
							btn1.setTextColor(sf::Color::Green);
						}
						else {
							btn1.setTextColor(sf::Color::White);
						}
						if (btn2.isMouseOver(window)) {
							btn2.setTextColor(sf::Color::Red);
						}
						else {
							btn2.setTextColor(sf::Color::White);
						}
						if (btn3.isMouseOver(window)) {
							btn3.setTextColor(sf::Color::Blue);
						}
						else {
							btn3.setTextColor(sf::Color::White);
						}
						break;
					case sf::Event::MouseButtonPressed:
						if (btn1.isMouseOver(window)) {
							try {
								check_ex = true;
								ParseNer(text1.getText());
								std::cout << "Image = " << text1.getText() << std::endl;
								check_ex1 = true;
								break;
							}
							catch (const std::exception& ex) {
								p++;
								check_ex = false;

								ss.str(std::string());
								ss << ex.what();
								std::cerr << "caused by exception: " << ex.what() << std::endl;

								clear_file("../input.txt");
								text1.DeleteAll(Event);
								if (remove("../input.txt")) {
									std::cout << "delete input" << std::endl;
								}
								//exit(1);
							}

							check_ex1 = true;
						}
						if (btn2.isMouseOver(window)) {
							window.close();
							clear_file("../input.txt");
							exit(1);
						}
						if (btn3.isMouseOver(window)) {
							flag = true;
							k = true;
							clear_file("../input.txt");
							text1.DeleteAll(Event);
							break;

						}
						else k = false;

					case sf::Event::KeyReleased:
						switch (Event.key.code) {

						case sf::Keyboard::Escape:
							if (true == isFUllScreen) {
								window.create(sf::VideoMode(640, 480), "Image", sf::Style::Default);
								isFUllScreen = false;

							}
							else {
								window.create(sf::VideoMode(1024, 768), "Image", sf::Style::Fullscreen);
								isFUllScreen = true;

							}
							break;

						case sf::Keyboard::Enter:
							try {
								check_ex = true;
								ParseNer(text1.getText());
								std::cout << "Image = " << text1.getText() << std::endl;
								check_ex1 = true;
								break;
							}
							catch (const std::exception& ex) {
								p++;
								check_ex = false;

								ss.str(std::string());
								ss << ex.what();
								std::cerr << "caused by exception: " << ex.what() << std::endl;

								clear_file("../input.txt");
								text1.DeleteAll(Event);
								if (remove("../input.txt")) {
									std::cout << "delete input" << std::endl;
								}
								//exit(1);
							}

							check_ex1 = true;
							break;

						case sf::Keyboard::F5:
							flag = true;
							k = true;
							// if (p == 0) {
							clear_file("../input.txt");
							text1.DeleteAll(Event);
							// }
							break;
						}
						break;
					}

					window.clear();
					window.draw(sprite);
					btn6.drawTo(window);
					btn7.drawTo(window);
					text1.drawTo(window);
					btn1.drawTo(window);
					btn2.drawTo(window);
					btn3.drawTo(window);
					btn4.drawTo(window);

					if ((!k) || (check_ex)) {
						if (check_ex1) {
							//если нет искдючение то одна запись 
							if (check_ex) {
								time = 1;

								btn_1.drawTo(window);
								btn_2.drawTo(window);
							}
							//если есть искдючение то дргуая запись 
							if (!check_ex) {
								//time = 2;
								btn_1.drawTo(window);

								Button btn_3(ss.str(), { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
								btn_3.setFont(font);

								btn_3.setPosition({ 135, 270 });

								btn_3.drawTo(window);
							}
						}
					}
					window.display();
				}
			} while (!check_ex);
			//конец вайла		

			if (time == 1) {
				delay(1000);//задержка экрана 
			}

			if (check_ex1) {
				break;
			}

		}

	}

	~GUI() {
		clear_file("../input.txt");
	}

	void clear_file(const std::string& file_name) {
		std::cerr << "caused by clear_file" << std::endl;
		std::fstream(file_name, std::fstream::out);
	}

	void ParseNer(const std::string& data) {
		if (data.empty()) {
			throw std::logic_error("empty data");
		}

		std::stringstream ss(data);

		bool ok = true;
		int day, year;
		std::string month;

		if (ss)
		{
			ok = ok && (ss >> day);
			if (day < 1 || day > 31) {
				ok = false;
				throw std::logic_error("Day value is invalid");
			}

			ok = ok && (ss >> month);
			if (month != "January" && month != "February" && month != "March" && month != "April" && month != "May" && month != "June" && month != "July" && month != "August" && month != "September" && month != "Octomber" && month != "November" && month != "December") {
				ok = false;
				throw std::logic_error("Month value is invalid");// +month);
			}

			if ((day > 29 || day < 1) && (month == "February")) {
				ok = false;
				throw std::logic_error("Day value is invalid");// +month);
			}

			if ((day > 30 || day < 1) && (month == "April" || month == "June" || month == "September" || month == "November")) {
				ok = false;
				throw std::logic_error("Day value is invalid");// +month);
			}

			ok = ok && (ss >> year);
			if (year < 1128) {

				ok = false;

				std::string error = "Year value is invalid"; //+ std::to_string(year);
				throw std::logic_error(error);
			}

			if (year == 1128) {
				if ((day < 11) && (month == "January" || month == "February" || month == "March" || month == "April" || month == "May")) {
					std::string error = "Wrong data";
					throw std::logic_error(error);
				}
				if (month == "June" && day < 11) {
					std::string error = "Wrong data";
					throw std::logic_error(error);
				}

			}

			ok = ok && ss.eof();

			if (!ok) {
				throw std::logic_error("Wrong date format");// +data);
			}

			std::cout << "image_date = " << day << month << year << std::endl;
		}
		else {
			throw std::logic_error("unknown: " + ss.str());
		}
	}

	void openIs(const std::string& path) {

		sf::RenderWindow window(sf::VideoMode(1024, 768), "Result");
		sf::Vector2i centerWindow((sf::VideoMode::getDesktopMode().width / 2) - 445, (sf::VideoMode::getDesktopMode().height / 2) - 480);
		window.setKeyRepeatEnabled(true);

		sf::Texture herotexture;
		//загружаем картинку
		herotexture.loadFromFile(path);

		//создаем объект Sprite(спрайт)
		sf::Sprite herosprite;
		//передаём в него объект Texture (текстуры)
		herosprite.setTexture(herotexture);
		//задаем начальные координаты появления спрайта
		herosprite.setPosition(0, 0);

		sf::Font font;
		font.loadFromFile("../Montserrat-Medium.ttf");
		Button btn5("Exit", { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
		btn5.setFont(font);
		btn5.setPosition({ 300, 600 });

		Button btn6("Retry", { 200, 100 }, 30, sf::Color::Transparent, sf::Color::White);
		btn6.setFont(font);
		btn6.setPosition({ 500, 600 });

		while (window.isOpen()) {
			sf::Event Event;
			while (window.pollEvent(Event)) {

				switch (Event.type) {
				case sf::Event::Closed:
					window.close();
					if (remove("../input.txt")) {
						std::cout << "delete input" << std::endl;
					}
					exit(1);

				case sf::Event::MouseMoved:

					if (btn5.isMouseOver(window)) {
						btn5.setTextColor(sf::Color::Red);
					}
					else {
						btn5.setTextColor(sf::Color::White);
					}
					if (btn6.isMouseOver(window)) {
						btn6.setTextColor(sf::Color::Blue);
					}
					else {
						btn6.setTextColor(sf::Color::White);
					}
					break;

				case sf::Event::MouseButtonPressed:
					if (btn5.isMouseOver(window)) {
						window.close();
						clear_file("../input.txt");
						exit(1);
					}
					if (btn6.isMouseOver(window)) {
						window.close();
					}

				case sf::Event::KeyReleased:
					switch (Event.key.code) {
					case sf::Keyboard::Escape:
						clear_file("../input.txt");
						window.close();
						exit(1);

					case sf::Keyboard::F5:
						clear_file("../input.txt");
						window.close();
						break;
					}
					break;
				}
				break;
			}

			window.clear();
			//выводим спрайт на экран
			window.draw(herosprite);
			btn5.drawTo(window);
			btn6.drawTo(window);
			window.display();
		}
	}
};

// C:\Users\Asus\Desktop\CodeC\C++\Visual Studio\PNG\x64\Release