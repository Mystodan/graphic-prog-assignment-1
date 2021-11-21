#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "ebo.h"
#include "vao.h"
#include "vbo.h"
#include "shaderClass.h"



/**
 *	A class for loading and displaying the map
 */
class Map {
private:
	unsigned int		width, 
						height,
						gPellets = 0;

	bool				gameOn = true;
	int					tempwidth, 
						tempheight, 
						resolution,
						spawnTileX,
						spawnTileY,
						tileTyp,
						tileTypN,
						tileTypE,
						tileTypW,
						tileTypS;
	int**				pelletCoords;

	// map tiles
	VAO*				t_VAO;
	VBO*				t_VBO;
	EBO*				t_EBO;
	
	std::vector<float>  t_vertices;
	std::vector<GLuint> t_indices;
	std::vector<GLuint>	t_types;

	// map tiles
	VAO* f_VAO;
	VBO* f_VBO;
	EBO* f_EBO;

	std::vector<float>  f_vertices;
	std::vector<GLuint> f_indices;
	std::vector<GLuint>	f_types;

	// pellets
	VAO*				p_VAO;
	VBO*				p_VBO;
	EBO*				p_EBO;

	std::vector<int>	pelletID;
	std::vector<bool>   hasPellet;
	std::vector<float>	p_vertices;
	std::vector<GLuint> p_indices;

	


public:
	std::vector<GLuint> getType() { return t_types; }
	std::vector<bool> checkPellet() { return hasPellet; }
	int  getWidth()		{ return width; }
	int  getGameStatus(){ return gameOn; }
	int  getHeight()	{ return height; }
	int  getSpawnX()	{ return spawnTileX; }
	int  getSpawnY()	{ return spawnTileY; }
	int  getPelletAmount() { return gPellets; }
	int  getPelletIndices() { return p_indices.size(); }
	int  getTileMode(int x, int y, bool entityUse = true);
	bool getPelletMode(int x, int y);
	int	 getPelletID(int x, int y);
  double getRandNum(int min, int max);

	std::pair<int, int> getSpawnGhost(int x, int y);

	void setPelletMode(int x, int y, bool mode = false);
	void setGameStatus(bool mode = false) { this->gameOn = mode; }
	std::pair<float, float> tileToCoords(float x, float y);
	std::pair<int, int>		coordsToTile(float x, float y);
	
	void printAllPellets();
	void pushTile(float x1, float y1, float mode, int& i, float size = 1.f);
	void pushPellet(float x1, float y1, float size, int& i);

	void loadLevel(std::string inn);
	void loadTiles();
	void loadPellets();
	void loadBuffers();

	void updatePellet();

	void pelletDraw();
	void tileDraw();
	void draw(GLFWwindow* window);

	Map(std::string inn);
	~Map();
};



#endif