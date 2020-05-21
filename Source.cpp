#include <SFML/Graphics.hpp>
#include <list>
#include <time.h>
#include <Windows.h>
#include <iostream>
using namespace sf;
Texture GameBackTex, BeamsTex, PlayerTex, EnemyTex;
Texture bTex1, bTex2, bTex3, bTex4, bTex5, bTex6, bTex7, bTex8, bTex9, bTex10, bTex11;
bool EnemyDraw = false, MenuClose = false; 
RenderWindow* pMenuWindow;
int what;

class point {
public:
	int x;
	int y;
};

class beam {
public:
	point lBeamPos;
	point rBeamPos;
	Sprite lBeamSpr;
	Sprite rBeamSpr;
	int h;
	int w;
	int damage;
};

class player {
public:
	point PlayerPos;
	beam BigBeam;
	beam LitBeam;
	Sprite PlayerSpr;
};
player Player;

class enemy {
public:
	int health;
	point EnemyPos;
	Sprite EnemySpr;
};
std::list<enemy> EnemyList;

class explosion {
public:
	Sprite e[11];
};
explosion Explosion;

struct ParamsForShot {
	player Player;
	beam Beam;
	int speed;
	RenderWindow* window;
};

void Boom(enemy Enemy, RenderWindow* window) {
	EnemyDraw = false;
	for (int i = 0; i < 11; i++) {
		std::cout << "Status: explosion animation\n";
		point EnemyCenter;
		EnemyCenter.x = (Enemy.EnemyPos.x + Enemy.EnemyPos.x + 120) / 2;
		EnemyCenter.y = (Enemy.EnemyPos.y + Enemy.EnemyPos.y + 185) / 2;
		point BoomPos;
		BoomPos.x = EnemyCenter.x - 125;
		BoomPos.y = EnemyCenter.y - 125;
		Explosion.e[i].setPosition(BoomPos.x, BoomPos.y);
		window->clear();
		window->draw(Sprite(GameBackTex));
		window->draw(Player.PlayerSpr);
		window->draw(Explosion.e[i]);
		window->display();
		Sleep(100);
	}
	window->display();
}

void ShotingByPlayer(ParamsForShot params) {
	Clock timer;
	Time tickRate;
	for (;;) {
		tickRate = timer.restart();
		std::cout << "Status: beam is going\n";
		params.Beam.lBeamSpr.move(0, params.speed*(float)tickRate.asMilliseconds()/1000);
		params.Beam.rBeamSpr.move(0, params.speed*(float)tickRate.asMilliseconds()/1000);
		if (params.window->mapCoordsToPixel(params.Beam.lBeamSpr.getPosition()).y <= 0) break;
		point CenterBeamL;
		point CenterBeamR;
		CenterBeamL.x = params.window->mapCoordsToPixel(params.Beam.lBeamSpr.getPosition()).x + params.Beam.w / 2;
		CenterBeamL.y = params.window->mapCoordsToPixel(params.Beam.lBeamSpr.getPosition()).y;
		CenterBeamR.x = params.window->mapCoordsToPixel(params.Beam.rBeamSpr.getPosition()).x + params.Beam.w / 2;
		CenterBeamR.y = params.window->mapCoordsToPixel(params.Beam.rBeamSpr.getPosition()).y;
		if (!EnemyList.empty()) {
			if (CenterBeamL.x<EnemyList.back().EnemyPos.x + 120 && CenterBeamL.x>EnemyList.back().EnemyPos.x) {
				if (CenterBeamL.y <= EnemyList.back().EnemyPos.y) {
					if (CenterBeamR.x<EnemyList.back().EnemyPos.x + 120 && CenterBeamR.x>EnemyList.back().EnemyPos.x) EnemyList.back().health -= params.Beam.damage;
					EnemyList.back().health -= params.Beam.damage;
					break;
				}
			}
			if (CenterBeamR.x<EnemyList.back().EnemyPos.x + 120 && CenterBeamR.x>EnemyList.back().EnemyPos.x) {
				if (CenterBeamR.y <= EnemyList.back().EnemyPos.y) {
					if (CenterBeamL.x<EnemyList.back().EnemyPos.x + 120 && CenterBeamL.x>EnemyList.back().EnemyPos.x) EnemyList.back().health -= params.Beam.damage;
					EnemyList.back().health -= params.Beam.damage;
					break;
				}
			}
		}
		params.window->clear();
		params.window->draw(Sprite(GameBackTex));
		params.window->draw(params.Player.PlayerSpr);
		params.window->draw(params.Beam.lBeamSpr);
		params.window->draw(params.Beam.rBeamSpr);
		if (EnemyDraw) params.window->draw(EnemyList.back().EnemySpr);
		params.window->display();
	}
}

void EnemyGeneration() {
	enemy Enemy;
	Enemy.EnemySpr = Sprite(EnemyTex);
	Enemy.health = 500;
	Enemy.EnemyPos.x = rand() % (1024 - 120 + 1);
	Enemy.EnemyPos.y = rand() % (Player.PlayerPos.y - 185 + 1);
	Enemy.EnemySpr.setPosition(Enemy.EnemyPos.x, Enemy.EnemyPos.y);
	EnemyList.push_back(Enemy);
}

void Menu() {
	RenderWindow MenuWindow(VideoMode(323, 151), "Space Battle");
	pMenuWindow = &MenuWindow;
	Texture MenuTex;
	MenuTex.loadFromFile("ButtonsCircle.PNG");
	Sprite MenuSpr;
	MenuSpr.setTexture(MenuTex);
	MenuSpr.setTextureRect(IntRect(0, 0, 323, 151));
	point pMenu;
	while (MenuWindow.isOpen())
	{
		MenuWindow.draw(MenuSpr);
		MenuWindow.display();
		Event event;
		while (MenuWindow.pollEvent(event)) {
			if (event.type == Event::Closed) MenuWindow.close();
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
				pMenu.x = Mouse::getPosition(MenuWindow).x;
				pMenu.y = Mouse::getPosition(MenuWindow).y;
				if ((pMenu.x - 74) * (pMenu.x - 74) + (pMenu.y - 75) * (pMenu.y - 75) <= 5184) {
					what = 1;
					MenuClose = true;
					break;
				}
				if ((pMenu.x - 247) * (pMenu.x - 247) + (pMenu.y - 75) * (pMenu.y - 75) <= 5184) {
					what = 2;
					MenuClose = true;
					break;
				}
			}
		}
		if (MenuClose) break;
	}
}

void Game() {
	RenderWindow GameWindow(VideoMode(1024, 1024), "Space Battle");
	Sprite GameBackSpr;
	GameBackSpr = Sprite(GameBackTex);
	Player.PlayerSpr = Sprite(PlayerTex);
	Player.BigBeam.lBeamSpr.setTexture(BeamsTex);
	Player.BigBeam.rBeamSpr.setTexture(BeamsTex);
	Player.LitBeam.lBeamSpr.setTexture(BeamsTex);
	Player.LitBeam.rBeamSpr.setTexture(BeamsTex);
	Player.BigBeam.lBeamSpr.setTextureRect(IntRect(80, 311, 32, 65));
	Player.BigBeam.rBeamSpr.setTextureRect(IntRect(80, 311, 32, 65));
	Player.LitBeam.lBeamSpr.setTextureRect(IntRect(5, 152, 16, 23));
	Player.LitBeam.rBeamSpr.setTextureRect(IntRect(5, 152, 16, 23));
	Player.PlayerSpr.setPosition(427, 880);
	Player.BigBeam.lBeamSpr.setPosition(12 - 16 + 427, 35 - 64 + 880);
	Player.BigBeam.rBeamSpr.setPosition(157 - 16 + 427, 35 - 64 + 880);
	Player.LitBeam.lBeamSpr.setPosition(61 - 8 + 427, 5 - 22 + 880);
	Player.LitBeam.rBeamSpr.setPosition(108 - 8 + 427, 5 - 22 + 880);
	Explosion.e[0] = Sprite(bTex1);
	Explosion.e[1] = Sprite(bTex2);
	Explosion.e[2] = Sprite(bTex3);
	Explosion.e[3] = Sprite(bTex4);
	Explosion.e[4] = Sprite(bTex5);
	Explosion.e[5] = Sprite(bTex6);
	Explosion.e[6] = Sprite(bTex7);
	Explosion.e[7] = Sprite(bTex8);
	Explosion.e[8] = Sprite(bTex9);
	Explosion.e[9] = Sprite(bTex10);
	Explosion.e[10] = Sprite(bTex11);
	while (GameWindow.isOpen()) {
		GameWindow.draw(GameBackSpr);
		GameWindow.draw(Player.PlayerSpr);
		Event event;
		Player.PlayerPos.x = GameWindow.mapCoordsToPixel(Player.PlayerSpr.getPosition()).x;
		Player.PlayerPos.y = GameWindow.mapCoordsToPixel(Player.PlayerSpr.getPosition()).y;
		Player.BigBeam.lBeamPos.x = 12 - 16 + Player.PlayerPos.x;
		Player.BigBeam.rBeamPos.x = 157 - 16 + Player.PlayerPos.x;
		Player.BigBeam.lBeamPos.y = 35 - 64 + Player.PlayerPos.y;
		Player.BigBeam.rBeamPos.y = 35 - 64 + Player.PlayerPos.y;
		Player.LitBeam.lBeamPos.x = 61 - 8 + Player.PlayerPos.x;
		Player.LitBeam.rBeamPos.x = 108 - 8 + Player.PlayerPos.x;
		Player.LitBeam.lBeamPos.y = 5 - 22 + Player.PlayerPos.y;
		Player.LitBeam.rBeamPos.y = 5 - 22 + Player.PlayerPos.y;
		Player.BigBeam.lBeamSpr.setPosition(Player.BigBeam.lBeamPos.x, Player.BigBeam.lBeamPos.y);
		Player.BigBeam.rBeamSpr.setPosition(Player.BigBeam.rBeamPos.x, Player.BigBeam.rBeamPos.y);
		Player.LitBeam.lBeamSpr.setPosition(Player.LitBeam.lBeamPos.x, Player.LitBeam.lBeamPos.y);
		Player.LitBeam.rBeamSpr.setPosition(Player.LitBeam.rBeamPos.x, Player.LitBeam.rBeamPos.y);
		while (GameWindow.pollEvent(event)) {
			if (event.type == Event::Closed) GameWindow.close();
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Q) {
				EnemyGeneration();
				std::cout << "Status: new enemy spawn\n";
				EnemyDraw = true;
			}
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
				ParamsForShot params;
				params.Beam = Player.LitBeam;
				params.Player = Player;
				params.speed = -5000;
				params.Beam.h = 16;
				params.Beam.w = 23;
				params.Beam.damage = 50;
				params.window = &GameWindow;;
				ShotingByPlayer(params);
				if (!EnemyList.empty()) {
					if (EnemyList.back().health <= 0) {
						Boom(EnemyList.back(), &GameWindow);
						EnemyList.pop_back();
					}
				}
			}
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Right) {
				ParamsForShot params;
				params.Beam = Player.BigBeam;
				params.Player = Player;
				params.speed = -4000;
				params.Beam.h = 32;
				params.Beam.w = 65;
				params.Beam.damage = 250;
				params.window = &GameWindow;
				ShotingByPlayer(params);
				if (!EnemyList.empty()) {
					if (EnemyList.back().health <= 0) {
						Boom(EnemyList.back(), &GameWindow);
						EnemyList.pop_back();
					}
				}
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::W)) {
			if (Player.PlayerPos.y >= 0) Player.PlayerSpr.move(0, -0.5f);
		}
		if (Keyboard::isKeyPressed(Keyboard::S)) {
			if (Player.PlayerPos.y <= 922) Player.PlayerSpr.move(0, 0.5f);
		}
		if (Keyboard::isKeyPressed(Keyboard::A)) {
			if (Player.PlayerPos.x >= 0) Player.PlayerSpr.move(-0.5f, 0);
		}
		if (Keyboard::isKeyPressed(Keyboard::D)) {
			if (Player.PlayerPos.x <= 854) Player.PlayerSpr.move(0.5f, 0);
		}
		if (EnemyDraw) GameWindow.draw(EnemyList.back().EnemySpr);
		GameWindow.display();
	}
}

int main() {
	srand(time(NULL));
	GameBackTex.loadFromFile("SpaceBack.JPG");
	BeamsTex.loadFromFile("Beams.PNG");
	PlayerTex.loadFromFile("Player.PNG");
	EnemyTex.loadFromFile("Enemy1.PNG");
	bTex1.loadFromFile("boom01.PNG");
	bTex2.loadFromFile("boom02.PNG");
	bTex3.loadFromFile("boom03.PNG");
	bTex4.loadFromFile("boom04.PNG");
	bTex5.loadFromFile("boom05.PNG");
	bTex6.loadFromFile("boom06.PNG");
	bTex7.loadFromFile("boom07.PNG");
	bTex8.loadFromFile("boom08.PNG");
	bTex9.loadFromFile("boom09.PNG");
	bTex10.loadFromFile("boom10.PNG");
	bTex11.loadFromFile("boom11.PNG");
	Menu();
	switch (what) {
	case 1: 
		Game();
		break;
	case 2:
		pMenuWindow->close();
	}
	return 0;
}