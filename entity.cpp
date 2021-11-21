#include "entity.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <math.h>


/** 
 *	Creates an entity, loads in initial values
 *	@param x - X-position.
 *	@param y - Y-position.
 *	@param level - link to level
 */
Entity::Entity(float x, float y, Map* level) {
	std::cout << "Setting entity..." << std::endl;
	
	this->map = level;
	std::pair<float, float>
		Pos = map->tileToCoords(x, y);
	posX = Pos.first;
	posY = Pos.second;
	
					// generating buffers for pacman
};
Entity::~Entity() {
	vao->Delete();
	vbo->Delete();
	ebo->Delete();
};

/**
 *	Creates a pacman, loads in initial values
 *	@param x - X-position.
 *	@param y - Y-position.
 *	@param level - link to level
 */
Pacman::Pacman(float x, float y, Map* level, float speed/*=0.02f*/) : Entity(x, y, level) {
	shader = new Shader("shaders/player.vert", "shaders/player.frag");
	color = 3;
	newSpeed = speed;
	velX = 0;
	velY = 0;
	// remember to add texture and shit.
	std::cout << "Generating buffers for Pacman ..." << std::endl;
	loadBuffers();
};

/**
 *	Memory clean up on close
 */
Pacman::~Pacman() {
	shader->Delete();
};
/**
 *	Creates a ghost, loads in initial values
 *	@param x - X-position.
 *	@param y - Y-position.
 *	@param level - link to level
 *	@param target - set entity target of who to eat
 *	@param colour - set the colour of the ghost
 *	@param speed - set the speed of the ghost
 */
Ghost::Ghost(float x, float y, Map* level, Pacman* target, float colour/*=0.f*/, float speed/*=0.02f*/) : Entity(x, y, level) {
	shader = new Shader("shaders/ghost.vert", "shaders/ghost.frag");
	Target = target;
	color = colour;
	newSpeed = speed;
	
	// sets facing of ghosts on startup
	double startVal = 0, endVal = 1, randnum = getRandNum(startVal, endVal);
	(randnum > startVal) ? facing = venstre : facing = høyre;
	if (checkFacingTile(facing) == 1) { (randnum > startVal) ?  facing = ned : facing = opp;}

	/*facing = venstre;
	if (checkFacingTile(facing) == 1) {
		facing = ned;
	}*/
	velX = 0;
	velY = 0;
	// remember to add texture and shit.
	std::cout << "Generating buffers for Ghost ..." << std::endl;
	loadBuffers();
};

/**
 *	Memory clean up on close
 */
Ghost::~Ghost() {
	shader->Delete();
};

/**
 *	Loads in pacMan verticies to make 1x1 tile and color
 *	Loads and links VAO, VBO and EBO
 */
void Entity::loadBuffers() {
	

	vertices.push_back(0.f);		// Left bottom corner
	vertices.push_back(0.f);
	vertices.push_back(0.01f);
	vertices.push_back(color);
	

	vertices.push_back(1.f);
	vertices.push_back(0.f);
	vertices.push_back(0.01f);
	vertices.push_back(color);

	vertices.push_back(1.f);
	vertices.push_back(1.f);
	vertices.push_back(0.01f);
	vertices.push_back(color);

	vertices.push_back(0.f);
	vertices.push_back(1.f);
	vertices.push_back(0.01f);
	vertices.push_back(color);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	/* ---buffers */
	// Generates Vertex Array Object and binds it
	vao = new VAO();
	vao->Bind();

	// Generates Vertex Buffer Object and links it to vertices
	vbo = new VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	ebo = new EBO(indices);

	// Links VBO attributes such as coordinates and colors to VAO
	vao->LinkAttrib(std::move(vbo), 0, 3, GL_FLOAT, 4 * sizeof(float), (void*)0);
	vao->LinkAttrib(std::move(vbo), 1, 1, GL_FLOAT, 4 * sizeof(float), (void*)(3 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	vao->Unbind();
	vbo->Unbind();
	ebo->Unbind();
}


/**
 *	Gets next tile and saves it
 */
void Entity::getNextTile() {
	nextTileX = map->coordsToTile(posX + velX * dt, posY + velY * dt).first;
	nextTileY = map->coordsToTile(posX + velX * dt, posY + velY * dt).second;
};

/**
 *	Gets tile mode and returns it
 *	@return tile mode (wall/tunnel/pacman)
 */
int Entity::getTileMode(int x, int y) {
	return map->getTileMode(x, y);
};

/**
 *	Checks for tile and pellet collision
 *	@param facing - direction
 *	@return The mode of the tile in front of us
 */
int Entity::checkFacingTile(direction facing) {
	// Gets and saves current tile
	std::pair<float, float> tile = map->tileToCoords(posX, posY),
		tempTile;
	// saves modes as temp values
	int tempMode1,
		tempMode2,
		wall = 1;
	// Pacman
	tile.first += 0.001; tile.second += 0.001;
	// Update it with facing dir
	switch (facing) {
	case venstre: {
		tile.first -= 0.005f;				// moves pacman along the x(.first) value

		tempTile.first = tile.first; 			// checks tile to the left
		tempTile.second = tile.second + 0.99f;

		tempMode1 = getTileMode(floor(tile.first), floor(tile.second));
		tempMode2 = getTileMode(floor(tempTile.first), floor(tempTile.second));

		if (tempMode1 == tempMode2) return tempMode1; // returns walkable tile if possible
		else return wall;

		break; }

	case høyre: {
		tile.first += 1.005f;					// moves pacman along the x(.first) value

		tempTile.first = tile.first;			// checks tile to the right
		tempTile.second = tile.second + 0.99f;
		tempMode1 = getTileMode(floor(tile.first), floor(tile.second));
		tempMode2 = getTileMode(floor(tempTile.first), floor(tempTile.second));

		if (tempMode1 == tempMode2) return tempMode1; // returns walkable tile if possible
		else return wall;

		break; }

	case opp: {
		tile.second -= 0.005f;					// moves pacman along the x(.first) value

		tempTile.first = tile.first + 0.99f;   // checks tile above
		tempTile.second = tile.second;

		tempMode1 = getTileMode(floor(tile.first), floor(tile.second));
		tempMode2 = getTileMode(floor(tempTile.first), floor(tempTile.second));
		if (tempMode1 == tempMode2) return tempMode1; // returns walkable tile if possible
		else return wall;

		break; }

	case ned: {
		tile.second += 1.005f;  // moves pacman along the x(.first) value

		tempTile.first = tile.first + 0.99f;    // checks tile under
		tempTile.second = tile.second;

		tempMode1 = getTileMode(floor(tile.first), floor(tile.second));
		tempMode2 = getTileMode(floor(tempTile.first), floor(tempTile.second));
		if (tempMode1 == tempMode2) return tempMode1; // returns walkable tile if possible
		else return wall;

		break; }

	}
};

/**
 *	Checks for pellet collision
 *	@see getPelletMode(..)
 *	@see getPeelletID(..)
 *	@see setPeleltMode(..)
 */
void Pacman::checkPellet(float x, float y) {
	if (map->getPelletMode(x, y)) {
		eatPellet(x,y);
	}
};

/**
 *	Moves pacman based on player input,
 *	and checks for collison.
 *	@param GLFW window
 */
void Pacman::move(GLFWwindow* window) {
	// Get relevant keys
	bool				k_left = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS,
						k_right = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS,
						k_up = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS,
						k_down = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;

	// Check which direction the player wants to turn
	direction newDir = (k_left ? venstre :
						k_right ? høyre :
						k_up ? opp :
						k_down ? ned :
						facing);

	// Check if we can actually turn towards newdir, 
	// if so change speed.
	if (checkFacingTile(newDir) == 0 || checkFacingTile(newDir) == 2) { facing = newDir; speed = newSpeed;  }
	else if (checkFacingTile(facing) == 1) {
		if (posX < 0.1f && posY == 18 && facing == venstre) {
			posX = map->getWidth()-0.1f;
		}
		if (posX > map->getWidth()-1.5f && posY == 18 && facing == høyre) {
			posX = -1;
		}
		
		speed = 0.f; //std::cout << checkFacingTile(facing) << " pos : x: " << posX << ", y:" << posY << std::endl;
		roundPos();
		
	}

	// Flips firstMove to true when player starts moving
	if (k_left + k_right + k_up + k_down == 1)	firstMove = true;
	
	// Go in that direction 
	
	switch (facing) {
	case venstre:	 posX -= speed; break;
	case høyre:		 posX += speed; break;
	case opp:		 posY += speed; break;
	case ned:		 posY -= speed; break;
	}
	// checks pellet from current pos, then eats the pellet
	checkPellet(map->coordsToTile(posX, posY).first, map->coordsToTile(posX, posY).second);
	// Updates pacman position
	Pacman::updatePos();
};

void Pacman::updatePos() {
	Entity::updatePos();
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(posX, posY, 0.0f));
	transform = glm::rotate(transform, 0.0f/*(GLfloat)glfwGetTime() * -5.0f*/, glm::vec3(0.0f, 0.0f, 1.0f));
	camera->Matrix(45.0f, 0.1f, 100.0f, *shader, "transformPac", transform);
}

double Ghost::getRandNum(int min, int max) {

	// x is in [0,1[
	double x = rand() / static_cast<double>(RAND_MAX + 1);

	// [0,1[ * (max - min) + min is in [min,max[
	double rand = min + static_cast<int>(x * (max - min));
	return rand;
}



/**
 * ...
 */
void Ghost::move() {
	
	// set relevant keys
	bool				k_left = false,
						k_right = false,
						k_up = false,
						k_down = false;
	// Check which direction the player wants to turn
	direction newDir = (
						k_left ? venstre :
						k_right ? høyre :
						k_up ? opp :
						k_down ? ned :
						facing
						);

	// Check if we can actually turn towards newdir, if so do so
	// ... 



	

	if (checkFacingTile(newDir) == 0 || checkFacingTile(newDir) == 2) {
		double randnum, startVal = 0, endVal = 2;
		facing = newDir; speed = newSpeed;

		switch (facing) {
		case 0: {
			randnum = getRandNum(startVal, endVal);
			if (checkFacingTile(ned) == 0 && checkFacingTile(opp) == 0) {
				roundPos();
				(randnum > startVal) ? facing = ned : facing = opp;
			}
			else if (checkFacingTile(ned)== 0) {
				roundPos();
				if (randnum > startVal) {
					facing = ned;
				}
			}
			else if (checkFacingTile(opp)== 0) {
				roundPos();
				if (randnum > startVal) {
					facing = opp;
				}
			}
			
			break;
		}
		case 1: {
			randnum = getRandNum(startVal, endVal);
			if (checkFacingTile(ned) == 0 && checkFacingTile(opp) == 0) {
				roundPos();
				(randnum > startVal) ? facing = ned : facing = opp;
			}
			else if (checkFacingTile(ned) == 0) {
				roundPos();
				if (randnum > startVal) {
					facing = ned;
				}
			}
			else if (checkFacingTile(opp) == 0) {
				roundPos();
				if (randnum > startVal) {
					facing = opp;
				}
			}
			break;
		}
		case 2: {
			randnum = getRandNum(startVal, endVal);
			if (checkFacingTile(høyre) == 0 && checkFacingTile(venstre) == 0) {
				roundPos();
				(randnum > startVal) ? facing = venstre : facing = høyre;
			}
			else if (checkFacingTile(høyre) == 0) {
				roundPos();
				if (randnum > startVal) {
					facing = høyre;
				}
			}
			else if (checkFacingTile(venstre) == 0) {
				roundPos();
				if (randnum > startVal) {
					facing = venstre;
				}
			}
			break;
		}
		case 3: {
			randnum = getRandNum(startVal, endVal);
			if (checkFacingTile(høyre) == 0 && checkFacingTile(venstre)== 0) {
				roundPos();
				(randnum > startVal) ? facing = venstre : facing = høyre;
			}
			else if (checkFacingTile(høyre) == 0) {
				roundPos();
				if (randnum > startVal) {
					facing = høyre;
				}
			}
			else if (checkFacingTile(venstre)== 0) {
				roundPos();
				if (randnum > startVal) {
					facing = venstre;
				}
			}
			break;
		}
		}
		
	}
	else if (checkFacingTile(facing) == 1) {
		roundPos();
		bounce();
	}
	
		
	// Go in that direction 
	switch (facing) {
	case venstre:	 posX -= speed; break;
	case høyre:		 posX += speed; break;
	case opp:		 posY += speed; break;
	case ned:		 posY -= speed; break;
	}
	checkPac();
	Ghost::updatePos();
};

void Ghost::bounce() {
	speed = 0.f;
	switch (facing) {
	case 0:  facing = høyre;	  break;
	case 1:  facing = venstre;	  break;
	case 2:  facing = ned;		  break;
	case 3:  facing = opp;		  break;
	}

}

void Ghost::checkPac() {
	std::pair<float, float> pacPos = Target->getPos();	
	std::pair<float,float> ghostPos = map->coordsToTile(posX,posY);
	if (round(pacPos.first) == round(ghostPos.first) && round(pacPos.second) == round(posY)) {
		eatPac();
	}
}
void Ghost::updatePos() {
	Entity::updatePos();
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(posX, posY, 0.0f));
	transform = glm::rotate(transform, 0.0f/*(GLfloat)glfwGetTime() * -5.0f*/, glm::vec3(0.0f, 0.0f, 1.0f));
	
	camera->Matrix(45.0f, 0.1f, 100.0f, *shader, "transformGhost", transform);
}



/**
 *	Updates Pacman posistion when called
 *	Transforms Pacman posistion
 */
void Entity::updatePos() {
	shader->Activate();
	float size = 1.f;
	if (checkFacingTile(facing) == 0 || checkFacingTile(facing) == 2) {
		posX += velX * dt;
		posY += velY * dt;
	}
	

};

/**
 *	Sets camera when called
 */
void Entity::setCamera(Camera* camera) {
	this->camera = camera;
};

/**
 *	Draws pacman on screen
 */
void Entity::draw() {
	vao->Bind();
	glDrawElements(GL_TRIANGLES, sizeof(std::vector<GLuint>) + sizeof(GLuint) * indices.size(), GL_UNSIGNED_INT, 0);
}
