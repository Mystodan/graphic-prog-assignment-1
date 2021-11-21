#ifndef PACMAN_H
#define PACMAN_H

#include <iostream>
#include <vector>
#include <string>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shaderClass.h"
#include "map.h"
#include "camera.h"


enum direction {
	venstre, høyre, opp, ned
};

class Entity {
	private:
	public:
		double					dt = glfwGetTime();
		float					posX,
								posY,
								velX,
								velY,
								speed,
								newSpeed,
								color;
		direction				facing;
		int						nextTileX,
								nextTileY;
		Map*					map;
		Camera*					camera;
		VAO*					vao;
		VBO*					vbo;
		EBO*					ebo;
		std::vector<float>		vertices;
		std::vector<GLuint>		indices;
		Shader*					shader;
		
		Entity(float x, float y, Map* level);
		~Entity();
		
		std::pair<float, float> getPos() { return std::pair<float, float> { posX, posY }; }
		void roundPos() {posX = round(posX);posY = round(posY);};
		void setCamera(Camera* camera);
		int  checkFacingTile(direction facing);
		void updatePos();
		void draw();
		void loadBuffers();
		int  getTileMode(int x, int y);
		void getNextTile();
};



/**
 *
 */
class Pacman : public Entity {
private:
	bool				isAlive = true,
						firstMove = false;
	void checkPellet(float x, float y);
	void updatePos();
	void eatPellet(int x, int y) { map->setPelletMode(x, y);};
public:
	Pacman(float x, float y, Map* level, float speed = 0.02f);
	~Pacman();
	void move(GLFWwindow* window);
	bool getIsAlive() { return this->isAlive; }
	bool hasMoved() { return this->firstMove; }
	void setIsAlive(bool state = false) { this->isAlive = state; }
	

};

class Ghost : public Entity{

private:
	// player target
	Pacman*				Target;
	std::vector<float>  vertices;
	std::vector<GLuint> indices;
	void updatePos();
	void bounce();
	double  getRandNum(int min, int max);
	void checkPac();
	void eatPac() { map->setGameStatus(); };

public:
	void move();
	Ghost(float x, float y,  Map* level, Pacman* target, float colour = 0.f, float speed = 0.02f );
	~Ghost();
};




#endif
