#include "map.h"

#include <fstream>
#include <iostream>
#include <random>


/**
 *	Loads level, tiles, pellets and buffers
 *	@see loadLevel(..)
 *	@see loadTiles(..)
 *	@see loadPellets(..)
 *  @see loadBuffers(..)
 */
Map::Map(std::string inn) {
	loadLevel(inn);
	loadTiles();
	loadPellets();
	loadBuffers();
}


/**
 * Cleans up memory when application closes
 */
Map::~Map() {
	t_VAO->Delete();
	t_VBO->Delete();
	t_EBO->Delete();

	p_VAO->Delete();
	p_VBO->Delete();
	p_EBO->Delete();
}


/**
 *	Loads the level's tile types
 *	@param string - level file
 */
void Map::loadLevel(std::string string) {
	std::cout << "Loading level ..." << std::endl;
	std::ifstream inn(string);
	int tileTyp;
	if (inn) {
		inn >> width; inn.ignore(1); inn >> height;

		for (int i = 0; i < height * width; i++) {
			inn >> tileTyp;
			t_types.push_back(tileTyp);
		}
		std::cout << "Done loading level." << std::endl;
	}
	else std::cout << "Could not load level from file." << std::endl;
};



/**
 *	Creates VAO, VBO and EBO
 *	Links verticies and indicies
 */
void Map::loadBuffers() {
	std::cout << "Generating buffers for tiles and pellets ..." << std::endl;

	/* --- Tile buffers */
	// Generates Vertex Array Object and binds it
	t_VAO = new VAO();
	t_VAO->Bind();

	// Generates Vertex Buffer Object and links it to vertices
	t_VBO = new VBO(t_vertices);
	// Generates Element Buffer Object and links it to indices
	t_EBO = new EBO(t_indices);

	// Links VBO attributes such as coordinates and colors to VAO
	t_VAO->LinkAttrib(t_VBO, 0, 3, GL_FLOAT, 4 * sizeof(float), (void*)0);
	t_VAO->LinkAttrib(t_VBO, 1, 1, GL_FLOAT, 4 * sizeof(float), (void*)(3 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	t_VAO->Unbind();
	t_VBO->Unbind();
	t_EBO->Unbind();


	/* --- Pellet buffers */
	p_VAO = new VAO;
	p_VAO->Bind();

	// Generates Vertex Buffer Object and links it to vertices
	p_VBO = new VBO(p_vertices);
	// Generates Element Buffer Object and links it to indices
	p_EBO = new EBO(p_indices);

	// Links VBO attributes such as coordinates and colors to VAO
	p_VAO->LinkAttrib(p_VBO, 0, 3, GL_FLOAT, 4 * sizeof(float), (void*)0);
	p_VAO->LinkAttrib(p_VBO, 1, 1, GL_FLOAT, 4 * sizeof(float), (void*)(3 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	p_VAO->Unbind();
	p_VBO->Unbind();
	p_EBO->Unbind();

	std::cout << "Done generating buffers." << std::endl;
}


/**
 *	Loads tiles based on information from level
 *	@see getTileMode(..)
 *	@see pushPellet(..)
 */
void Map::loadTiles() {
	std::cout << "Setting tiles ..." << std::endl;

	int i = 0, j = 0; // Holder styr på hvor mange tiles vi har opprettet
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {

			// Load tile into VBO and EBO
			tileTyp  = getTileMode(x, y,false);
			tileTypN = getTileMode(x, y + 1, false);
			tileTypE = getTileMode(x - 1, y, false);
			tileTypS = getTileMode(x, y - 1, false);
			tileTypW = getTileMode(x + 1, y, false);
			pushTile(x, y, tileTyp, i, j, tileTyp, tileTypN, tileTypE, tileTypS, tileTypW);

			// Set player spawn
			if (tileTyp == 2) { spawnTileX = x; spawnTileY = y; }
		}
	std::cout << "Done setting tiles." << std::endl;
}


/**
 *	Loads pellets based on information from level
 *	@see getTileMode(..)
 *	@see pushPellet(..)
 */
void Map::loadPellets() {
	std::cout << "Setting pellets ..." << std::endl;

	// Initialize 2D Pellet coordinate array
	pelletCoords = new int*[height];
	for (int y = 0; y < height; y++) {
		pelletCoords[y] = new int[width];
		for (int x = 0; x < width; x++) {
			pelletCoords[y][x] = -1; // Set default value to -1 to indicate no pellet 
		}
	}

	// Push pellet data into vbo and ebo
	int j = 0; // Holder styr på hvor mange tiles vi har opprettet
	int k = 0; // Holder styr på hvilken ID pellet vi lager nå
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (getTileMode(x, y) == 0) {
				pushPellet(x + 0.5f, y - 0.25f, 0.25f, j);
				hasPellet.push_back(true);
				// Push back ID'n til den nyskapte pelleten i pelletCoords-arrayen
				pelletCoords[y][x] = k;
				k++;
				gPellets++;
			}
			else {
				hasPellet.push_back(false);
				pelletID.push_back(-1); // ? er du på discord nå?? prøv nå
			}
		}
	}
	std::cout << "Done setting pellets." << std::endl;
}


/**
 *	Gets the screenspace position of a given tile.
 * 
 *	@param x - X-position.
 *	@param y - Y-position.
 *	@return The screenspace coordinates of the given tile as a pair of X and Y
 */
std::pair<float, float> Map::tileToCoords(float x, float y) {
	//std::cout << "tilex " << x << " tiley " << y << std::endl;
	return std::pair<float, float> { x, height - y -1};
}


/**
 *	Gets which tile the given coordinates lays within.
 *	@param x - X-position.
 *	@param y - Y-position.
 *	@return x and y position of tile.
 */
std::pair<int, int>	Map::coordsToTile(float x, float y) {
	return std::pair<int, int> { round(x), height-1-round(y)};
}

/**
 *	Changes pellet mode when pacman eats it,
 *	and color changes to black.
 *
 *	@param x - X-position.
 *	@param y - Y-position.
 *	@param mode - wall/tunnel
 *	@param size - Default 1.f
 * 
 *	@see tileToCoords(x1, y1)
 */
void Map::pushTile(float x1, float y1, float mode, int& i, int& j, int tileTyp, int tileTypN, int tileTypE, int tileTypS, int tileTypW, float size/*=1.f*/) {

	std::pair<float, float> tilePos = tileToCoords(x1, y1);

	float	x = tilePos.first,
			y = tilePos.second;

	int temp;
	if (tileTyp == 1) temp = 1.f;
	else temp = 0.f;

	// Bottom left 1
	t_vertices.push_back(tilePos.first);
	t_vertices.push_back(tilePos.second);
	t_vertices.push_back(temp);
	t_vertices.push_back(mode);

	// Bottom right 1
	t_vertices.push_back(tilePos.first + size);
	t_vertices.push_back(tilePos.second);
	t_vertices.push_back(temp);
	t_vertices.push_back(mode);

	// Top right 1
	t_vertices.push_back(tilePos.first + size);
	t_vertices.push_back(tilePos.second + size);
	t_vertices.push_back(temp);
	t_vertices.push_back(mode);

	// Top left 1
	t_vertices.push_back(tilePos.first);
	t_vertices.push_back(tilePos.second + size);
	t_vertices.push_back(temp);
	t_vertices.push_back(mode);

	// Push back indices
	t_indices.push_back(i * 4);
	t_indices.push_back((i * 4) + 1);
	t_indices.push_back((i * 4) + 2);
	t_indices.push_back(i * 4);
	t_indices.push_back((i * 4) + 2);
	t_indices.push_back((i * 4) + 3);
	i++;

	if (tileTypE != 1 && tileTyp != 0) {

		// EAST
		t_vertices.push_back(tilePos.first);
		t_vertices.push_back(tilePos.second);
		t_vertices.push_back(0.0f);
		t_vertices.push_back(mode);

		// EAST
		t_vertices.push_back(tilePos.first);
		t_vertices.push_back(tilePos.second);
		t_vertices.push_back(1.0f);
		t_vertices.push_back(mode);

		// EAST
		t_vertices.push_back(tilePos.first);
		t_vertices.push_back(tilePos.second + size);
		t_vertices.push_back(1.0f);
		t_vertices.push_back(mode);

		// EAST
		t_vertices.push_back(tilePos.first);
		t_vertices.push_back(tilePos.second + size);
		t_vertices.push_back(0.0f);
		t_vertices.push_back(mode);

		// Push back indices
		t_indices.push_back(i * 4);
		t_indices.push_back((i * 4) + 1);
		t_indices.push_back((i * 4) + 2);
		t_indices.push_back(i * 4);
		t_indices.push_back((i * 4) + 2);
		t_indices.push_back((i * 4) + 3);
		i++;
	}
	if (tileTypW != 1 && tileTyp != 0) {

		// WEST left 3
		t_vertices.push_back(tilePos.first + size);
		t_vertices.push_back(tilePos.second);
		t_vertices.push_back(0.0f);
		t_vertices.push_back(mode);

		// WEST right 3
		t_vertices.push_back(tilePos.first + size);
		t_vertices.push_back(tilePos.second);
		t_vertices.push_back(1.0f);
		t_vertices.push_back(mode);

		// WEST right 3
		t_vertices.push_back(tilePos.first + size);
		t_vertices.push_back(tilePos.second + size);
		t_vertices.push_back(1.0f);
		t_vertices.push_back(mode);

		// WEST left 3
		t_vertices.push_back(tilePos.first + size);
		t_vertices.push_back(tilePos.second + size);
		t_vertices.push_back(0.0f);
		t_vertices.push_back(mode);

		// Push back indices
		t_indices.push_back(i * 4);
		t_indices.push_back((i * 4) + 1);
		t_indices.push_back((i * 4) + 2);
		t_indices.push_back(i * 4);
		t_indices.push_back((i * 4) + 2);
		t_indices.push_back((i * 4) + 3);
		i++;
	}
	if (tileTypN != 1 && tileTyp != 0) {
		// Bottom left 4
		t_vertices.push_back(tilePos.first);
		t_vertices.push_back(tilePos.second);
		t_vertices.push_back(0.0f);
		t_vertices.push_back(mode);

		// Bottom right 4
		t_vertices.push_back(tilePos.first + size);
		t_vertices.push_back(tilePos.second);
		t_vertices.push_back(0.0f);
		t_vertices.push_back(mode);

		// Top right 4
		t_vertices.push_back(tilePos.first + size);
		t_vertices.push_back(tilePos.second);
		t_vertices.push_back(1.0f);
		t_vertices.push_back(mode);

		// Top left 4
		t_vertices.push_back(tilePos.first);
		t_vertices.push_back(tilePos.second);
		t_vertices.push_back(1.0f);
		t_vertices.push_back(mode);

		// Push back indices
		t_indices.push_back(i * 4);
		t_indices.push_back((i * 4) + 1);
		t_indices.push_back((i * 4) + 2);
		t_indices.push_back(i * 4);
		t_indices.push_back((i * 4) + 2);
		t_indices.push_back((i * 4) + 3);
		i++;
	}
	if (tileTypS != 1 && tileTyp != 0) {
		// Bottom left 5
		t_vertices.push_back(tilePos.first);
		t_vertices.push_back(tilePos.second + size);
		t_vertices.push_back(0.0f);
		t_vertices.push_back(mode);

		// Bottom right 5
		t_vertices.push_back(tilePos.first + size);
		t_vertices.push_back(tilePos.second + size);
		t_vertices.push_back(0.0f);
		t_vertices.push_back(mode);

		// Top right 5
		t_vertices.push_back(tilePos.first + size);
		t_vertices.push_back(tilePos.second + size);
		t_vertices.push_back(1.0f);
		t_vertices.push_back(mode);

		// Top left 5
		t_vertices.push_back(tilePos.first);
		t_vertices.push_back(tilePos.second + size);
		t_vertices.push_back(1.0f);
		t_vertices.push_back(mode);

		// Push back indices
		t_indices.push_back(i * 4);
		t_indices.push_back((i * 4) + 1);
		t_indices.push_back((i * 4) + 2);
		t_indices.push_back(i * 4);
		t_indices.push_back((i * 4) + 2);
		t_indices.push_back((i * 4) + 3);
		i++;
	}
}


/**
 *	Changes pellet mode when pacman eats it,
 *	and color changes to black.
 *
 *	@param x - X-position.
 *	@param y - Y-position.
 *	@param size - size of the Pellet.
 *	@param i - Pellet ID
 */
void Map::pushPellet(float x, float y, float size, int& i) {
	x -= size / 2.f; y -= size / 2.f;
	int mode = 3;
	
	std::pair<float, float> 
			tilePos = tileToCoords(x, y);
	float	newX		= tilePos.first,
			newY		= tilePos.second;

	// Bottom left 1
	p_vertices.push_back(newX); 
	p_vertices.push_back(newY);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Bottom right 1
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Top right 1
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Top left 1
	p_vertices.push_back(newX);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Push back indices
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 1);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back((i * 4) + 3);
	i++;

	// Bottom left 2
	p_vertices.push_back(newX);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.375f);
	p_vertices.push_back(mode);

	// Bottom right 2
	p_vertices.push_back(newX);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Top right 2
	p_vertices.push_back(newX);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Top left 2
	p_vertices.push_back(newX);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.375f);
	p_vertices.push_back(mode);

	// Push back indices
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 1);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back((i * 4) + 3);
	i++;

	// Bottom left 3
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.375f);
	p_vertices.push_back(mode);

	// Bottom right 3
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Top right 3
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Top left 3
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.375f);
	p_vertices.push_back(mode);

	// Push back indices
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 1);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back((i * 4) + 3);
	i++;

	// Bottom left 4
	p_vertices.push_back(newX);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.375f);
	p_vertices.push_back(mode);

	// Bottom right 4
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.375f);
	p_vertices.push_back(mode);

	// Top right 4
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Top left 4
	p_vertices.push_back(newX);
	p_vertices.push_back(newY);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Push back indices
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 1);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back((i * 4) + 3);
	i++;

	// Bottom left 5
	p_vertices.push_back(newX);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.375f);
	p_vertices.push_back(mode);

	// Bottom right 5
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.375f);
	p_vertices.push_back(mode);

	// Top right 5
	p_vertices.push_back(newX + size);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Top left 5
	p_vertices.push_back(newX);
	p_vertices.push_back(newY + size);
	p_vertices.push_back(0.625f);
	p_vertices.push_back(mode);

	// Push back indices
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 1);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back(i * 4);
	p_indices.push_back((i * 4) + 2);
	p_indices.push_back((i * 4) + 3);
	i++;
}


/**
 *	Updates a Pellets VAO and VBO
 */
void Map::updatePellet() {
	p_VAO->Bind();

	// Generates Vertex Buffer Object and links it to vertices
	p_VBO = new VBO(p_vertices);

	// Links VBO attributes such as coordinates and colors to VAO
	p_VAO->LinkAttrib(p_VBO, 0, 3, GL_FLOAT, 4 * sizeof(float), (void*)0);
	p_VAO->LinkAttrib(p_VBO, 1, 1, GL_FLOAT, 4 * sizeof(float), (void*)(3 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	p_VAO->Unbind();
	p_VBO->Unbind();
	p_VBO->Delete();
}

/**
 *	Returns a tile mode, when called.
 *	return -1 means out of bounds.
 * 
 *  @param x - X-position.
 *	@param y - Y-position.
 */
int Map::getTileMode(int x, int y, bool entityUse) {
	if (!entityUse) {
		if (x >= width || x < 0 || y >= height || y < 0) {
			return -1;
		}
	}
	//std::cout << getType()[y * width + x] << std::endl;
	return getType()[y * width + x];
}
double Map::getRandNum(int min, int max) {

	// x is in [0,1[
	double x = rand() / static_cast<double>(RAND_MAX + 1);

	// [0,1[ * (max - min) + min is in [min,max[
	double rand = min + static_cast<int>(x * (max - min));
	return rand;
}
/**
 *	Returns a pellet mode, when called.
 *
 *  @param x - X-position.
 *	@param y - Y-position.
 */
std::pair<int, int>	Map::getSpawnGhost(int x, int y) {
	double valX = 0, valY = 0;
	while (getTileMode(valX, valY) == 1)
	{
		valX = getRandNum(0, x - 1), valY = getRandNum(0, y - 1);
	}

	return std::pair<int, int> {valX, valY};
}

/**
 *	Returns a pellet mode, when called.
 *
 *  @param x - X-position.
 *	@param y - Y-position.
 */
bool Map::getPelletMode(int x, int y) {
	return checkPellet()[y * width + x];
}

/**
 *	Returns a pellet ID, when called.
 * 
 *  @param x - X-position.
 *	@param y - Y-position.
 *	@return The Pellet ID
 */
int Map::getPelletID(int x, int y) {
	return pelletCoords[y][x];

	/*
	if (getPelletMode(x,y)) {
		std::cout << " y * width + x " << y * width + x << std::endl;
		return pelletID[y * width + x];
	}
	else {
		return NULL;
	}*/
}
void Map::printAllPellets() {
	for (int i = 0; i < hasPellet.size(); i++) {
		if (i % 28 == 0) {
			std::cout << std::endl;
		}
		std::cout << hasPellet[i];
	}
}

/**
 *	Changes pellet mode when pacman eats it,
 *	and color changes to black.
 * 
 *	@param x - X-position.
 *	@param y - Y-position.
 *	@param ID - Pellet ID
 *	@param mode - default false
 */
void Map::setPelletMode(int x, int y, bool mode) {
	// Get the ID of the pellet
	int ID = getPelletID(x, y);

	// ...and return if its invalid
	if (ID == -1) return; 

	// Otherwise, set the pellet mode and subtract one pellet
	hasPellet[y * width + x] = mode;
	mode ? gPellets++ : gPellets--;

	// ...and make the pellet appear black
	for (int i = 0; i < 80; i+=4)
		p_vertices[ID*80 + i] = 0;
}

/**
 *	Draws tiles on screen.
 */
void Map::tileDraw() {
	t_VAO->Bind();
	glDrawElements(GL_TRIANGLES, sizeof(std::vector<GLuint>) + sizeof(GLuint) * t_indices.size(), GL_UNSIGNED_INT, 0);
}
/**
 *	Draws pellets on screen.
 */
void Map::pelletDraw() {
	p_VAO->Bind();
	glDrawElements(GL_TRIANGLES, sizeof(std::vector<GLuint>) + sizeof(GLuint) * p_indices.size(), GL_UNSIGNED_INT, 0);
}
/**
 *	Draws map items, tiles, pellets.
 *	Quits game on escape, or 0 pellets.
 *	@param window - GLFW window
 */
void Map::draw(GLFWwindow* window) {
	tileDraw();
	updatePellet();
	pelletDraw();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		setGameStatus();
	else if (gPellets == 0)
		setGameStatus();
}
