#include "Entity.h"

#include "Shape.h"


Entity::Entity(Shape*thisShape, GLuint thisShaderProgram, float startingLocX, float startingLocY, float scaleX, float scaleY)
{
	entityShape = thisShape;
	shaderProgramIndex = thisShaderProgram;
	entityPosition.x = startingLocX; entityPosition.y = startingLocY;
	entityScale.x = scaleX; entityScale.y = scaleY;
}

// simply sets X and Y values equal to the given values
void Entity::moveInst(float thisX, float thisY)
{
	entityPosition.x = thisX; entityPosition.y = thisY;
}

// modifies X and Y values by the given velocity; matches scale to the given value
// by scaling properly, movement in three dimensions - using foreground and background as the third - may be simulated
void Entity::moveVelocity3D(float velocityX, float velocityY, float scaleFactorGiven)
{
	entityPosition.x += velocityX; entityPosition.y += velocityY;
	entityScale.x = scaleFactorGiven * .09f; entityScale.y = scaleFactorGiven * .09f;
}

// modifies X and Y values according to the movement of a spring
void Entity::moveVelocitySpring(int dt, Entity*targetEntity, glm::vec3 givenVelocity)
{	
	// determine distance between entities
	glm::vec3 targetEntityPosition = targetEntity -> entityPosition;
	glm::vec3 displacement = (entityPosition - targetEntityPosition);

	// Various acceleration variables
	float dtScaled = dt * 0.043f;
	float k = 5.0f;
	float damp = .88f;
	float mass = 2.0f;
	
	glm::vec3 springForce = -k * displacement;
	springForce -= (damp * givenVelocity);
	springForce /= mass;
	
	// adjust the position
	givenVelocity += (dtScaled * springForce); 
	entityPosition.x += (dtScaled * givenVelocity.x); entityPosition.y += (dtScaled * givenVelocity.y);
}

// calls the associated shape's Draw method, passing in the relevant info
void Entity::Draw(void)
{
	entityShape -> Draw(entityPosition.x, entityPosition.y, entityScale.x, entityScale.y);
}

void Entity::Update(void)
{
	// set up world matrix
	/* glm::mat4 globalMatrix;
	globalMatrix = (glm::translate(entityPosition) * glm::scale(entityScale));

	// send updated position value to the shader
	setShaderMatrix(shaderProgramIndex, "worldMatrix", globalMatrix); */
}

Entity::~Entity(void)
{
}