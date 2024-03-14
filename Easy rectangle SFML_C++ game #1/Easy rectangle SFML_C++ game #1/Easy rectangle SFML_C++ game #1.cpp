#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
using namespace std;

#include <string>
#include<Windows.h>

const int wk = 1.5;
const int H = 12;
const int W = 12;
const int CELLSIZE = 32;

String Tilemap[H] = {
	"WWWWWWWWWWWW",
	"W          W",
	"W          W",
	"W  WWWWWW  W",
	"W  W       W",
	"W  W    WW W",
	"W  W       W",
	"W  W  W    W",
	"W  W       W",
	"W  W    W  W",
	"W          W",
	"WWWWWWWWWWWW",
};

struct object
{
	vector<Vector2i>arr_pos = { {5,10 },{8,8}, {9,4}, {4, 2} };
	FloatRect obj;
	RectangleShape rectshape;
	int pos_index=0; // indicating what position is now
	int point_count = 0;

	object(int x=5, int y=10, int width=16, int height=16)
	{
		obj.width = width;
		obj.height = height;
		obj.left = x * CELLSIZE + obj.width / 2;
		obj.top = y * CELLSIZE + obj.height / 2;

		rectshape.setSize({ obj.width * wk, obj.height * wk });
		rectshape.setFillColor(Color::Red);
		rectshape.setPosition(obj.left * wk, obj.top * wk);
	}

	void update()
	{
		new_position();

		rectshape.setPosition(obj.left * wk, obj.top * wk);
	}

	void new_position()
	{
		point_count++;
		pos_index++;
		if (pos_index >= arr_pos.size())
			pos_index = 0;
		obj.left = arr_pos[pos_index].x * CELLSIZE + obj.width / 2;
		obj.top = arr_pos[pos_index].y * CELLSIZE + obj.height / 2;
	}

	int getcount()
	{
		return point_count;
	}
};

struct player
{
	FloatRect rect;
	int speed;
	float gravity;
	float dx, dy; // dir
	float velx, vely;
	float jump_height;
	float friction = 0.60;
	bool onGround;
	RectangleShape rectshape;

	int ground;

	player()
	{
		dx = dy = 0;
		rect.left = rect.top = 64;
		velx = vely = 0;
		rect.height = rect.width = 24;
		speed = 6.5;
		gravity = 3;
		onGround = false;
		jump_height = 19;
		ground = 350;
	}

	void Collision(bool dir)
	{
		for (int i = rect.top / CELLSIZE; i < (rect.top + rect.height) / CELLSIZE; i++)
			for (int j = rect.left / CELLSIZE; j < (rect.left + rect.width) / CELLSIZE; j++)
			{
				if (Tilemap[i][j] == 'W')
				{
					if (velx > 0 && dir == 1) { rect.left = j * CELLSIZE - rect.width; }
					if (velx < 0 && dir == 1) { rect.left = j * CELLSIZE + CELLSIZE; }
					if (vely > 0 && dir == 0) { rect.top = i * CELLSIZE - rect.height; vely = 0; onGround = true; }
					if (vely < 0 && dir == 0) { rect.top = i * CELLSIZE + CELLSIZE; vely = 0;}
				}
			}
	}

	void update(object& obj)
	{

		if (dx == 0) velx *= friction;
		else
		{
			if (abs(velx) <= speed)
				velx += dx * speed * 0.35;
			else if (abs(velx) > speed)
				velx = dx * speed;
			else
				velx = dx * speed;
		}

		dy = 1;
		
		if (rect.intersects(obj.obj))
			obj.update();

		if (Keyboard::isKeyPressed(Keyboard::W) || 
			Keyboard::isKeyPressed(Keyboard::Space) || 
			Keyboard::isKeyPressed(Keyboard::Z) || 
			Keyboard::isKeyPressed(Keyboard::Up)
			)
			if (onGround) 
			{ 
				dy = -1;
				vely = dy * jump_height; 
			}

		//cout << rect.left << ' ' << rect.top << ' ' << dx << ' ' << dy << ' ' << onGround << endl;
		
		

		vely += dy * gravity;

		onGround = false;

		rect.left += velx;
		Collision(1);
		rect.top += vely;
		Collision(0);


		dx = 0;
		rectshape.setPosition(rect.left * wk, rect.top * wk);
	}


};
int main()
{
	HWND hConsole = GetConsoleWindow();//Если компилятор старый заменить на GetForegroundWindow()
	ShowWindow(hConsole, SW_HIDE);//собственно прячем оконо консоли
	RenderWindow window(VideoMode(384 * wk, 384 * wk), "Easy rectangle SFML_C++ game #1"); // 384 - 12 x 32
	window.setFramerateLimit(30);

	RectangleShape rect_tile({ (CELLSIZE - 1) * wk,(CELLSIZE - 1) * wk });
	
	object obj;
	player p;
	Font f;
	
	if (!f.loadFromFile("MonaspaceXenon-ExtraLight.otf"))
		cout << "ERROR IN FONT OPENNING" << endl;
	
	

	RectangleShape text_bg;
	text_bg.setPosition(8 * wk, 8 * wk);
	text_bg.setFillColor({ 200,200,200,43 });
	Text t;
	t.setFont(f);
	t.setCharacterSize(15 * wk);
	t.setPosition(8 * wk, 8 * wk);
	t.setFillColor({125,125,125,255});

	string str = "";

	p.rectshape.setFillColor(Color::Black);
	p.rectshape.setPosition(p.rect.left * wk, p.rect.top * wk);
	p.rectshape.setSize(Vector2f(p.rect.width * wk, p.rect.height * wk));

	while (window.isOpen())
	{

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) p.dx = -1;
		if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) p.dx = 1;
		
		int og = obj.getcount();

		string str = to_string(og);

		text_bg.setSize(Vector2f{ (75 + float(15 * (str.length() - 1))) * wk, 20 * wk });
		t.setString("Count: " + str);


		p.update(obj);
		window.clear(Color::White);
		for (int i = 0; i < H; i++)
			for (int j = 0; j < W; j++)
			{
				if (Tilemap[i][j] == 'W') rect_tile.setFillColor(Color::Black);
				if (Tilemap[i][j] == ' ') continue;

				rect_tile.setPosition(j * CELLSIZE * wk, i * CELLSIZE * wk);
				window.draw(rect_tile);
			}
		window.draw(obj.rectshape);
		window.draw(p.rectshape);
		window.draw(text_bg);
		window.draw(t);
		window.display();
	}
	return 0;
}