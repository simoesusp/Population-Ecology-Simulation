#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <map>


#define ENTITY_POOL_SIZE 256
#define SPECIES_AMOUNT 3
#define MIN_REPRODUCTION_ENERGY 1500
#define MIN_SOIL_NUTRITION 100
// Shrimp, Algae, bacteria

enum args{
	NAME,
	SIMTIME,
	SHRIMP_AMOUNT,
	ALGAE_AMOUNT,
	BACTERIA_AMOUNT
};

typedef enum species{
	SHRIMP,
	ALGAE,
	BACTERIA
}Species;

typedef int Type;
#define AUTOTROPH 1
#define HETEROTROPH 2
#define DETRITIVORE 4



typedef struct speciesProperties{
//if > 0: Oxygen consumer/ if < 0: carbondioxide consumer
	double oxygenCarbonConsumption;
	double speed;
	double size;
	unsigned maxAge;
	
	unsigned minEnergyToReproduct;
	unsigned offspringAmount;
	Type type;
	bool isPrey[SPECIES_AMOUNT];
	int timeBetweenReproductions;
}SpeciesProperties;

typedef struct world
{
	double sizex, sizey;
	double oxygenCarbonAmount;
	double oxygenCarbonRatio;
	double soilNutrition;
}World;

typedef struct entity{
	double x, y;
	double size;
	double velx, vely;
	double rot;
	bool isActive;
	
	double energy;
	double speed;
	unsigned age;
	Species species;
}Entity;

void initializeWorld(World &world, double oxygenCarbonRatio, double oxygenCarbonAmount,
 									   double soilNutrition, double sizex, double sizey);

void initializeCommunity(World &world, SpeciesProperties *sProps, Entity *entityPool,
							      int shrimpAmount, int algaeAmount, int bacteriaAmount);

void initializeSpeciesProperties(SpeciesProperties *sProps);

void initializeEntity(Entity &entity, double x, double y, double size, double energy,
						  double speed, bool isActive, unsigned age, Species species);

void simulationLoop(World &world, SpeciesProperties *sProps, Entity *entityPool, 
			  sf::Time simulationTime, sf::Time updateInterval, sf::Time renderInterval);

int findClosestResource(World &world, Entity &entity, SpeciesProperties &sp, 
					                     SpeciesProperties *sProps, Entity *entityPool);

void updateEntity(World &world, Entity &entity, SpeciesProperties &sp,
		   			    SpeciesProperties *sProps, Entity *entityPool, sf::Time elapsed);

void update(World &world, SpeciesProperties *sProps, Entity *entityPool,
																	   sf::Time elapsed);

void render(sf::RenderWindow *window, SpeciesProperties *sProps, Entity *entityPool);

void draw(sf::RenderWindow *window, Entity &entity, SpeciesProperties &sp);

void logData(float secs, Entity *entityPool, SpeciesProperties *sProps);

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
	
	e1.velx = speed * (e1.velx/magnitude);
	e1.vely = speed * (e1.vely/magnitude);
	
	e1.x += e1.velx;
	e1.y += e1.vely;
	
	e1.energy -= 0.1;
}



int main(int argc, char **argv)
{
	// if(argc != 5)
	// {
	// 	printf("Missing arguments!\n");
	// 	printf("Usage(arguments): simulation_durantion(s), predators_amount, producer_amount, detritivore_amount\n");
	// 	return 1;
	// }
	
	std::map <std::string, double> initial_values;
	initial_values["simulation_duration"] 	= 30;
	initial_values["arena_height"]			= 600;
	initial_values["arena_width"]			= 600;
	initial_values["oxygenCarbonRatio"]		= .5;
	initial_values["producer_amount"] 		= 40;
	initial_values["herbivore_amount"] 		= 10;
	initial_values["detritivore_amount"] 	= 10;
	
	
	// 	if(shrimp_amount + algae_amount + bacteria_amount > 255)
	// {
	// 	printf("Too many individuals! 255 maximum!\n");
	// }
	
	
	
	std::string line;
	std::string name, amount;
	std::ifstream readFile("sim_data.in");
	
	if(readFile.is_open())
	{
		std::cout << "File opened";
	}
	while(std::getline(readFile,line))
	{
	    std::stringstream iss(line);
	    std::getline(iss, name, ':');
	    std::getline(iss, amount);
	    initial_values[name] = std::stod(amount);
	    std::cout << name << " ";
	    std::cout << initial_values[name] << std::endl;
	}
	readFile.close();
	
	
	World world;
	Entity entityPool[ENTITY_POOL_SIZE];
	SpeciesProperties speciesProperties[SPECIES_AMOUNT];
	
	srand(time(NULL));
	
	
	initializeWorld(world, initial_values["oxygenCarbonRatio"], 10000, 100000, 
										initial_values["arena_height"], initial_values["arena_width"]);
	
	initializeSpeciesProperties(speciesProperties);
	
	initializeCommunity(world, speciesProperties, entityPool, initial_values["herbivore_amount"], 
								initial_values["producer_amount"], initial_values["detritivore_amount"]);
	
	simulationLoop(world, speciesProperties, entityPool, 
				sf::seconds(initial_values["simulation_duration"]), sf::seconds(1/60), sf::seconds(1/60));
	
	
	return 0;
}

void initializeWorld(World &world, double oxygenCarbonRatio, double oxygenCarbonAmount, double soilNutrition, double sizex, double sizey)
{
	world.oxygenCarbonRatio = oxygenCarbonRatio;
	world.oxygenCarbonAmount = oxygenCarbonAmount;
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
		initializeEntity(entityPool[pooli++], randInt(10, world.sizex - 10), randInt(10, world.sizey - 10), 
					sProps[SHRIMP].size, randInt(500, 1500), sProps[SHRIMP].speed, true, 0, SHRIMP);
	}
	
	
	for(i = 0; i < algaeAmount; i++)
	{
		initializeEntity(entityPool[pooli++], randInt(10, world.sizex - 10), randInt(10, world.sizey - 10), 
					sProps[ALGAE].size, randInt(1000, 2000), sProps[ALGAE].speed, true, 0, ALGAE);
	}
	
	for(i = 0; i < bacteriaAmount; i++)
	{
		initializeEntity(entityPool[pooli++], randInt(10, world.sizex - 10), randInt(10, world.sizey - 10), 
			  sProps[BACTERIA].size, randInt(1000, 2000), sProps[BACTERIA].speed, true, 0, BACTERIA);
	}
	
	while(pooli < ENTITY_POOL_SIZE)
	{
		initializeEntity(entityPool[pooli++], 0, 0, 0, 0, 0, false, 0, SHRIMP);
	}
}

void initializeSpeciesProperties(SpeciesProperties *sProps)
{
	sProps[SHRIMP].oxygenCarbonConsumption 		= -1;
	sProps[SHRIMP].speed 						= 0.07;
	sProps[SHRIMP].size 						= 8;
	sProps[SHRIMP].maxAge 						= 10000;
	sProps[SHRIMP].offspringAmount 				= 3;
	sProps[SHRIMP].timeBetweenReproductions 	= 2;
	sProps[SHRIMP].minEnergyToReproduct			= 1500;
	sProps[SHRIMP].type 						= HETEROTROPH;
	sProps[SHRIMP].isPrey[SHRIMP] 				= false;
	sProps[SHRIMP].isPrey[ALGAE] 				= true;
	sProps[SHRIMP].isPrey[BACTERIA] 			= false;
	
	sProps[ALGAE].oxygenCarbonConsumption 		= 1;
	sProps[ALGAE].speed 						= 0;
	sProps[ALGAE].size 							= 5;
	sProps[ALGAE].maxAge 						= 100000;
	sProps[ALGAE].offspringAmount 				= 2;
	sProps[ALGAE].timeBetweenReproductions 		= 5;
	sProps[ALGAE].type							= AUTOTROPH;
	sProps[ALGAE].minEnergyToReproduct			= 2500;
	sProps[ALGAE].isPrey[SHRIMP] 				= false;
	sProps[ALGAE].isPrey[ALGAE] 				= false;
	sProps[ALGAE].isPrey[BACTERIA] 				= false;
	
	sProps[BACTERIA].oxygenCarbonConsumption 	= -1;
	sProps[BACTERIA].speed 						= 0.4;
	sProps[BACTERIA].size						= 3;
	sProps[BACTERIA].maxAge 					= 10100;
	sProps[BACTERIA].offspringAmount 			= 5;
	sProps[BACTERIA].timeBetweenReproductions 	= 5;
	sProps[BACTERIA].type						= DETRITIVORE;
	sProps[BACTERIA].minEnergyToReproduct		= 100;
	sProps[BACTERIA].isPrey[SHRIMP] 			= false;
	sProps[BACTERIA].isPrey[ALGAE] 				= false;
	sProps[BACTERIA].isPrey[BACTERIA] 			= false;
}

void initializeEntity(Entity &entity, double x, double y, double size, double energy,
						  double speed, bool isActive, unsigned age, Species species)
{
	entity.x = x;
	entity.y = y;
	entity.size = size;
	entity.speed = speed;
	entity.velx = 0;
	entity.vely = 0;
	entity.isActive = isActive;
	
	entity.energy = energy;
	entity.age = age;
	entity.species = species;
}

void simulationLoop(World &world, SpeciesProperties *sProps, Entity *entityPool,
	  sf::Time simulationTime, sf::Time updateInterval, sf::Time renderInterval)
{
	bool enableUpdate 	= true;
	bool enableRender 	= true;
	bool enableLog 		= true;
	
	sf::Time updateTimeThreshold, renderTimeThreshold, logTimeThreshold;
	
	logTimeThreshold = updateTimeThreshold = renderTimeThreshold = sf::Time::Zero;
	
	sf::Time logInterval = sf::seconds(1);
	
	sf::RenderWindow mainWindow;
	mainWindow.create(sf::VideoMode(world.sizex, world.sizey), "Main Window");
	
	sf::Clock mainClock;
	
	sf::Time previous;
	sf::Time current = mainClock.getElapsedTime();
	
	//sf::Time toHandleInput 	= sf::Time::Zero;
	sf::Time toUpdate 	= sf::Time::Zero;
	sf::Time toRender 	= sf::Time::Zero;
	sf::Time toLog 		= sf::Time::Zero;
	
	sf::Clock elapsed;
	while(mainClock.getElapsedTime() < simulationTime)
	{
		previous = current;
		if(enableLog && toLog < logTimeThreshold) {
			toLog = logInterval;
			logData(mainClock.getElapsedTime().asSeconds(), entityPool, sProps);
		}
		current = mainClock.getElapsedTime();
		toUpdate -= current - previous;
		toRender -= current - previous;
		toLog -= current - previous;
		previous = current;
		if(enableUpdate && toUpdate < updateTimeThreshold) {
			toUpdate = updateInterval;
			update(world, sProps, entityPool, elapsed.restart());
		}
		current = mainClock.getElapsedTime();
		toUpdate -= current - previous;
		toRender -= current - previous;
		toLog -= current - previous;
		previous = current;
		if(enableRender && toRender < renderTimeThreshold) {
			toRender = renderInterval;
			render(&mainWindow, sProps, entityPool);
		}
		current = mainClock.getElapsedTime();
		toUpdate -= current - previous;
		toRender -= current - previous;
		toLog -= current - previous;
	}
}

void logData(float secs, Entity *entityPool, SpeciesProperties *sProps)
{
	int totalAlive[SPECIES_AMOUNT];
	// int totalDead[SPECIES_AMOUNT];
	
	for(int i = 0; i < SPECIES_AMOUNT; i++)
	{
		totalAlive[i] = 0;
		// totalDead[i] = 0;
	}
	
	for(int i = 0; i < ENTITY_POOL_SIZE; i++)
	{
		if(entityPool[i].isActive)
		{
			if(entityPool[i].age < sProps[entityPool[i].species].maxAge)
				totalAlive[entityPool[i].species]++;
			// else
				// totalDead[entityPool[i].species]++;
		}
	}
	
	std::cout << secs;
	for(int i = 0; i < SPECIES_AMOUNT; i++)
		std::cout << ", " << totalAlive[i];
	std::cout << std::endl;
}

int findClosestResource(World &world, Entity &entity, SpeciesProperties &sp, 
					                     SpeciesProperties *sProps, Entity *entityPool)
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
			   ((sp.type & DETRITIVORE)  && entityPool[j].age > 
			   	sProps[entityPool[j].species].maxAge)))
			{
					
				if(distanceSquared(entity, entityPool[j]) < entity.size*entity.size)
				{
					entity.energy += 0.9 * entityPool[j].energy;
					entityPool[j].age += 1000000;
					if(sp.type & DETRITIVORE){
						entity.energy += 0.1 * entityPool[j].energy;
						world.soilNutrition+= 10 * entityPool[j].size;
						entityPool[j].isActive = false;
					}
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
	
	return closestResource;
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
	if(entity.energy <= 0 || randInt(0, RAND_MAX) <= 0)
	{
		entity.age += 10000000;
		return;
	}
	
	entity.age++;

	world.oxygenCarbonRatio += sp.oxygenCarbonConsumption * (1/world.oxygenCarbonAmount);
	
	if(entity.energy > sp.minEnergyToReproduct && (world.oxygenCarbonRatio * sp.oxygenCarbonConsumption) < 0)
	{
		if(entity.species == ALGAE && world.soilNutrition < MIN_SOIL_NUTRITION)
		{
			world.soilNutrition -= 0.01;
			entity.energy += 5;
			return;
		}
			
		
		int j = sp.offspringAmount;
		entity.energy /= sp.offspringAmount + 1;
		for(int i = 0; i < ENTITY_POOL_SIZE && j > 0; i++)
		{
			if(!entityPool[i].isActive)
			{
				initializeEntity(entityPool[i], randInt(10, world.sizex - 10), randInt(10, world.sizey - 10),
								 sp.size, entity.energy/(sp.offspringAmount + 1), sp.speed,
								 true, 0, entity.species);
				j--;
			}
		}
	}
	else
	if(sp.type & (DETRITIVORE + HETEROTROPH) )
	{
		int closestResource = findClosestResource(world, entity, sp, sProps, entityPool);
		if(closestResource >= 0)
			goTo(entity, entityPool[closestResource], entity.speed);
	}
	else
	{
		world.soilNutrition -= 0.01;
		entity.energy += 5;
	}
}

void render(sf::RenderWindow *window, SpeciesProperties *sProps, Entity *entityPool)
{
	window->clear(sf::Color::Black);
	for(int i = 0; i < ENTITY_POOL_SIZE; i++)
	{
		if(entityPool[i].isActive)
			draw(window, entityPool[i], sProps[entityPool[i].species]);
	}
	window->display();
}

void draw(sf::RenderWindow *window, Entity &entity, SpeciesProperties &sp)
{
	sf::CircleShape circle;
	if(entity.age > sp.maxAge)
		circle.setFillColor(sf::Color::White);
	else
	if(entity.species == SHRIMP)
		circle.setFillColor(sf::Color::Red);
	else
	if(entity.species == ALGAE)
		circle.setFillColor(sf::Color::Green);
	else
	if(entity.species == BACTERIA)
		circle.setFillColor(sf::Color::Yellow);
	
	circle.setRadius(entity.size);
	circle.setPosition(entity.x - entity.size, entity.y - entity.size);
	window->draw(circle);
}