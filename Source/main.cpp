#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#define ENTITY_POOL_SIZE 256
#define SPECIES_AMOUNT 3
// Shrimp, Algae, bacteria

typedef enum species{
	SHRIMP = 0,
	ALGAE = 1,
	BACTERIA = 2
}Species;

typedef struct speciesProperties{
//if > 0: Oxygen comsumer/ if < 0: carbondioxide comsumer
	double oxygenCarbonConsumption;
	double speed;
	double size;
	unsigned maxAge;
	unsigned offpsringAmount;
	unsigned reproductiveAgeFrom;
	unsigned reproductiveAgeTo;
	bool isConsumer;
	bool isDetritivore;
	bool isPrey[SPECIES_AMOUNT];
	int timeBetweenReproductions;
}SpeciesProperties;

typedef struct world
{
	double sizex, sizey;
	double oxygenCarbonAmount;
	double oxygenCarbonRatio;
}World;

typedef struct entity{
	double x, y;
	double size;
	double speed;
	double velx, vely;
	double rot;
	bool isActive;
	
	unsigned age;
	Species species;
}Entity;

void initializeWorld(World &world, double oxygenCarbonRatio, double sizex, double sizey);
void initializeCommunity(World &world, SpeciesProperties *sProps, Entity *entityPool, int shrimpAmount, int algaeAmount, int bacteriaAmount);
void initializeSpeciesProperties(SpeciesProperties *sProps);
void initializeEntity(Entity &entity, double x, double y, double size, double speed, bool isActive, unsigned age, Species species);
void simulationLoop(World &world, SpeciesProperties *sProps, Entity *entityPool, sf::Time simulationTime, sf::Time updateInterval, sf::Time renderInterval);
void updateEntity(World &world, Entity &entity, SpeciesProperties &sp, SpeciesProperties *sProps, Entity *entityPool, sf::Time elapsed);
void update(World &world, SpeciesProperties *sProps, Entity *entityPool, sf::Time elapsed);
void render(sf::RenderWindow *window, Entity *entityPool);

inline double distanceSquared(Entity &e1, Entity &e2)
{
	return (e2.x - e1.x)*(e2.x - e1.x) + (e2.y - e1.y)*(e2.y - e1.y);
}

inline double randInt(int from, int to)
{
	if(from == to) return to;
	return rand() % (to - from) + from;
}

inline double randDouble(double from, double to)
{
	if(from == to) return to;
	return (rand()/RAND_MAX) * (to - from) + from;
}

inline void goTo(Entity &e1, Entity &e2, double speed)
{
	e1.velx = e2.x - e1.x;
	e1.vely = e2.y - e1.y;
	
	double magnitude = sqrt(e1.velx * e1.velx + e1.vely * e1.vely);
	
	e1.velx = speed * (e1.velx/magnitude) * randDouble(0.8, 1);
	e1.vely = speed * (e1.vely/magnitude) * randDouble(0.8, 1);
	
	e1.x += e1.velx;
	e1.y += e1.vely;
}

inline void draw(sf::RenderWindow *window, Entity &entity)
{
	sf::CircleShape circle;
	if(entity.species == SHRIMP)
		circle.setFillColor(sf::Color::Red);
	else
	if(entity.species == ALGAE)
		circle.setFillColor(sf::Color::Green);
	else
	if(entity.species == BACTERIA)
		circle.setFillColor(sf::Color::White);
	
	circle.setRadius(entity.size);
	circle.setPosition(entity.x - entity.size, entity.y - entity.size);
	window->draw(circle);
}

int main()
{
	World world;
	Entity entityPool[ENTITY_POOL_SIZE];
	SpeciesProperties speciesProperties[SPECIES_AMOUNT];
	
	srand(time(NULL));
	
	initializeWorld(world, 0.5, 600, 600);
	initializeSpeciesProperties(speciesProperties);
	
	initializeCommunity(world, speciesProperties, entityPool, 5, 5, 10);
	simulationLoop(world, speciesProperties, entityPool, sf::seconds(10), sf::seconds(1/60), sf::seconds(1/60));
	
	
	return 0;
}

void initializeWorld(World &world, double oxygenCarbonRatio, double sizex, double sizey)
{
	world.oxygenCarbonRatio = oxygenCarbonRatio;
	world.sizex = sizex;
	world.sizey = sizey;
}

void initializeCommunity(World &world, SpeciesProperties *sProps, Entity *entityPool,
						       int shrimpAmount, int algaeAmount, int bacteriaAmount)
{
	if(shrimpAmount + algaeAmount + bacteriaAmount > ENTITY_POOL_SIZE)
	{
		std::cout << "Too many individuals" << std::endl;
		return;
	}
	
	int pooli = 0;
	int i;
	
	
	
	for(i = 0; i < shrimpAmount; i++)
	{
		initializeEntity(entityPool[pooli++], randInt(10, world.sizex), randInt(10, world.sizey), 
							 sProps[SHRIMP].size, sProps[SHRIMP].speed, true, 0, SHRIMP);
	}
	
	
	for(i = 0; i < algaeAmount; i++)
	{
		initializeEntity(entityPool[pooli++], randInt(10, world.sizex), randInt(10, world.sizey), 
									 sProps[ALGAE].size, sProps[ALGAE].speed, true, 0, ALGAE);
	}
	
	for(i = 0; i < bacteriaAmount; i++)
	{
		initializeEntity(entityPool[pooli++], randInt(10, world.sizex), randInt(10, world.sizey), 
							sProps[BACTERIA].size, sProps[BACTERIA].speed, true, 0, BACTERIA);
	}
	
	while(pooli < ENTITY_POOL_SIZE)
	{
		initializeEntity(entityPool[pooli++], 0, 0, 0, 0, false, 0, SHRIMP);
	}
}

void initializeSpeciesProperties(SpeciesProperties *sprops)
{
	sprops[SHRIMP].oxygenCarbonConsumption 		= -1;
	sprops[SHRIMP].speed 						= 0.07;
	sprops[SHRIMP].size 						= 8;
	sprops[SHRIMP].maxAge 						= 100;
	sprops[SHRIMP].offpsringAmount 				= 20;
	sprops[SHRIMP].timeBetweenReproductions 	= 5;
	sprops[SHRIMP].isConsumer 					= true;
	sprops[SHRIMP].isDetritivore 				= false;
	sprops[SHRIMP].isPrey[SHRIMP] 				= false;
	sprops[SHRIMP].isPrey[ALGAE] 				= true;
	sprops[SHRIMP].isPrey[BACTERIA] 			= false;
	sprops[SHRIMP].reproductiveAgeFrom 			= 30;
	sprops[SHRIMP].reproductiveAgeTo 			= 80;
		
	sprops[ALGAE].oxygenCarbonConsumption 		= 1;
	sprops[ALGAE].speed 						= 0;
	sprops[ALGAE].size 							= 5;
	sprops[ALGAE].maxAge 						= 100;
	sprops[ALGAE].offpsringAmount 				= 20;
	sprops[ALGAE].timeBetweenReproductions 		= 5;
	sprops[ALGAE].isConsumer 					= false;
	sprops[ALGAE].isDetritivore 				= false;
	sprops[ALGAE].isPrey[SHRIMP] 				= false;
	sprops[ALGAE].isPrey[ALGAE] 				= false;
	sprops[ALGAE].isPrey[BACTERIA] 				= false;
	sprops[ALGAE].reproductiveAgeFrom 			= 30;
	sprops[ALGAE].reproductiveAgeTo 			= 80;
	
	sprops[BACTERIA].oxygenCarbonConsumption 	= -1;
	sprops[BACTERIA].speed 						= 0.1;
	sprops[BACTERIA].size						= 3;
	sprops[BACTERIA].maxAge 					= 1000;
	sprops[BACTERIA].offpsringAmount 			= 20;
	sprops[BACTERIA].timeBetweenReproductions 	= 5;
	sprops[BACTERIA].isConsumer 				= false;
	sprops[BACTERIA].isDetritivore 				= true;
	sprops[BACTERIA].isPrey[SHRIMP] 			= false;
	sprops[BACTERIA].isPrey[ALGAE] 				= false;
	sprops[BACTERIA].isPrey[BACTERIA] 			= false;
	sprops[BACTERIA].reproductiveAgeFrom 		= 30;
	sprops[BACTERIA].reproductiveAgeTo 			= 80;
}

void initializeEntity(Entity &entity, double x, double y, double size, double speed,
									   bool isActive, unsigned age, Species species)
{
	entity.x = x;
	entity.y = y;
	entity.size = size;
	entity.speed = speed;
	entity.velx = 0;
	entity.vely = 0;
	entity.isActive = isActive;
	
	entity.age = age;
	entity.species = species;
}

void simulationLoop(World &world, SpeciesProperties *sProps, Entity *entityPool,
	  sf::Time simulationTime, sf::Time updateInterval, sf::Time renderInterval)
{
	bool enableUpdate = true;
	bool enableRender = true;
	
	sf::Time updateTimeThreshold, renderTimeThreshold;
	updateTimeThreshold = renderTimeThreshold = sf::Time::Zero;
	
	
	sf::RenderWindow mainWindow;
	mainWindow.create(sf::VideoMode(world.sizex, world.sizey), "Main Window");
	
	sf::Clock mainClock;
	
	sf::Time previous;
	sf::Time current = mainClock.getElapsedTime();
	
	//sf::Time toHandleInput 	= sf::Time::Zero;
	sf::Time toUpdate 		= sf::Time::Zero;
	sf::Time toRender 		= sf::Time::Zero;
	
	sf::Clock elapsed;
	while(mainClock.getElapsedTime() < simulationTime)
	{
		previous = current;
		if(enableUpdate && toUpdate < updateTimeThreshold) {
			toUpdate = updateInterval;
			update(world, sProps, entityPool, elapsed.restart());
		}
		current = mainClock.getElapsedTime();
		toUpdate -= current - previous;
		toRender -= current - previous;
		previous = current;
		if(enableRender && toRender < renderTimeThreshold) {
			toRender = renderInterval;
			render(&mainWindow, entityPool);
		}
		current = mainClock.getElapsedTime();
		toUpdate -= current - previous;
		toRender -= current - previous;
	}
}


void update(World &world, SpeciesProperties *sProps, Entity *entityPool, sf::Time elapsed)
{
	for(int i = 0; i < ENTITY_POOL_SIZE; i++)
	{
		if(entityPool[i].isActive)
		{	
			updateEntity(world, entityPool[i], sProps[entityPool[i].species],
						 								   sProps, entityPool, elapsed);
		}
	}
}

void updateEntity(World &world, Entity &entity, SpeciesProperties &sp, 
                  SpeciesProperties *sProps, Entity *entityPool, sf::Time elapsed)
{
	if(entity.age > sp.maxAge)
		return;
	
	if( randDouble(0, 1) <= 0.5)
	{
		entity.age++;
	}
	
	world.oxygenCarbonRatio += sp.oxygenCarbonConsumption * (1/world.oxygenCarbonAmount);
	if(sp.isConsumer || sp.isDetritivore)
	{
		int closestResource;
	
		closestResource = -1;
		
		for(int j = 0; j < ENTITY_POOL_SIZE; j++)
		{
			if(entityPool[j].isActive)
			{
				if((&entityPool[j] != &entity) && 
					((sp.isPrey[entityPool[j].species] && 
					entityPool[j].age < sProps[entityPool[j].species].maxAge) || 
				   (sp.isDetritivore && entityPool[j].age > 
				   	sProps[entityPool[j].species].maxAge)))
				{
						
					if(distanceSquared(entity, entityPool[j]) < entity.size*entity.size)
					{
						entityPool[j].isActive = false;
					}
					else	
					if(closestResource == -1){
						closestResource = j;
					}
					else
					if(distanceSquared(entity, entityPool[j]) < 
					   distanceSquared(entity, entityPool[closestResource]))
					{
						closestResource = j;
					}
				}
			}
		}
		
		if(closestResource >= 0)
			goTo(entity, entityPool[closestResource], entity.speed);
	}
}

void render(sf::RenderWindow *window, Entity *entityPool)
{
	window->clear(sf::Color::Black);
	for(int i = 0; i < ENTITY_POOL_SIZE; i++)
	{
		if(entityPool[i].isActive)
			draw(window, entityPool[i]);
	}
	window->display();
}
