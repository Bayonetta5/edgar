#include "defs.h"

typedef struct Map
{
	char *filename;
	int startX, startY;
	int maxX, maxY;
	int tile[MAX_MAP_Y][MAX_MAP_X];
	SDL_Surface *background;
} Map;

typedef struct Animation
{
	int frameCount;
	int offsetX, offsetY;
	int *frameTimer, *frameID;
} Animation;

typedef struct Input
{
	int left, right, up, down, add, remove;
	int previous, next, load, save, jump;
	int mouseX, mouseY, toggle;
} Input;

typedef struct Entity
{
	int active, w, h;
	int thinkTime, face;
	int currentFrame, frameTimer;
	int type, animation[MAX_ANIMATION_TYPES];
	int currentAnim, health, customThinkTime[MAX_CUSTOM_ACTIONS];
	long flags;
	float x, y, dirX, dirY;
	float startX, startY, endX, endY;
	float targetX, targetY;
	struct Entity *standingOn, *parent;
	void (*action)(void);
	void (*activate)(struct Entity *);
	void (*draw)(void);
	void (*touch)(struct Entity *);
	void (*custom[MAX_CUSTOM_ACTIONS])(int *);
} Entity;

typedef struct Sound
{
	Mix_Chunk *effect;
} Sound;

typedef struct Game
{
	char sword[30], shield[30];
	int thinkTime, weatherType, weatherThinkTime;
	void (*action)(void);
	void (*weatherAction)(void);
	void (*weatherDraw)(void);
	SDL_Surface *screen;
	TTF_Font *font;
} Game;

typedef struct Cursor
{
	int x, y, tileID, entityType;
	int type;
	Entity entity;
} Cursor;

typedef struct Message
{
	char text[MAX_MESSAGE_LENGTH];
	int counter;
} Message;

typedef struct Inventory
{
	int selectedIndex;
	Entity item[MAX_INVENTORY_ITEMS];
} Inventory;

typedef struct Droplet
{
	int active;
	float x, y, dirX, dirY;
} Droplet;

typedef struct Properties
{
	char name[30], key[MAX_PROPS_ENTRIES][30], value[MAX_PROPS_ENTRIES][30];
} Properties;