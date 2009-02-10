#include "headers.h"

#include "animation.h"
#include "properties.h"
#include "map.h"
#include "geometry.h"
#include "inventory.h"
#include "collisions.h"
#include "custom_actions.h"
#include "collisions.h"
#include "player.h"
#include "entity.h"

extern Entity player, playerShield, playerWeapon;
extern Entity *self;
extern Input input;

static void takeDamage(Entity *, int);
static void attackFinish(void);

void loadPlayer(int x, int y)
{
	loadProperties("edgar/edgar", &player);

	if (player.inUse != TRUE)
	{
		player.inUse = TRUE;
		player.x = x;
		player.y = y;
		player.dirX = player.dirY = 0;
		player.face = LEFT;
		player.flags = 0;
		player.type = PLAYER;
		player.frameSpeed = 1;

		player.thinkTime = 0;

		player.maxHealth = player.health = 5;

		setEntityAnimation(&player, STAND);

		player.draw = &drawLoopingAnimationToMap;

		player.takeDamage = &takeDamage;

		playerShield.parent = &player;
		playerWeapon.parent = &player;

		playerWeapon.face = playerShield.face = LEFT;

		centerMapOnEntity(&player);
	}

	else
	{
		setPlayerLocation(x, y);
	}
}

void setPlayerLocation(int x, int y)
{
	player.x = x;
	player.y = y;

	player.draw = &drawLoopingAnimationToMap;

	player.inUse = TRUE;
}

void doPlayer()
{
	int i;

	self = &player;

	if (self->action == NULL)
	{
		for (i=0;i<MAX_CUSTOM_ACTIONS;i++)
		{
			if (self->customThinkTime[i] > 0)
			{
				self->custom[i](&self->customThinkTime[i]);
			}
		}

		/* Gravity always pulls the player down */

		if (!(self->flags & FLY))
		{
			self->dirY += GRAVITY_SPEED;

			if (self->dirY >= MAX_FALL_SPEED)
			{
				self->dirY = MAX_FALL_SPEED;
			}

			else if (self->dirY > 0 && self->dirY < 1)
			{
				self->dirY = 1;
			}
		}

		else
		{
			self->dirY = 0;
		}

		if (!(self->flags & HELPLESS))
		{
			self->dirX = 0;

			if (self->standingOn != NULL)
			{
				self->dirX += self->standingOn->dirX;

				if (self->standingOn->dirY > 0)
				{
					self->dirY = self->standingOn->dirY;
				}
			}

			if (self->animationCallback == NULL)
			{
				if (input.left == 1)
				{
					self->dirX -= self->speed;

					/* Only pull the target */

					if ((self->flags & GRABBING) && self->target != NULL)
					{
						if (self->target->x > self->x)
						{
							self->target->dirX = -self->speed;
						}
					}

					else
					{
						playerWeapon.face = playerShield.face = self->face = LEFT;
					}

					setEntityAnimation(&player, WALK);
					setEntityAnimation(&playerShield, WALK);
					setEntityAnimation(&playerWeapon, WALK);
				}

				else if (input.right == 1)
				{
					self->dirX += self->speed;

					/* Only pull the target */

					if ((self->flags & GRABBING) && self->target != NULL)
					{
						if (self->target->x < self->x)
						{
							self->target->dirX = self->speed;
						}
					}

					else
					{
						playerWeapon.face = playerShield.face = self->face = RIGHT;
					}

					setEntityAnimation(&player, WALK);
					setEntityAnimation(&playerShield, WALK);
					setEntityAnimation(&playerWeapon, WALK);
				}

				else if (input.left == 0 && input.right == 0)
				{
					setEntityAnimation(&player, STAND);
					setEntityAnimation(&playerShield, STAND);
					setEntityAnimation(&playerWeapon, STAND);

					if ((self->flags & GRABBING) && self->target != NULL)
					{
						self->target->dirX = 0;
					}
				}

				if (input.up == 1)
				{
					if (self->flags & FLY)
					{
						self->dirY = -1;
					}

					else
					{
						if (self->standingOn != NULL)
						{
							if (self->standingOn->activate != NULL)
							{
								printf("Trying to activate %s\n", self->standingOn->objectiveName);

								self = self->standingOn;

								self->activate(1);

								self = &player;
							}

							input.up = 0;
						}
					}
				}

				if (input.down == 1)
				{
					if (self->flags & FLY)
					{
						self->dirY = 1;
					}

					else
					{
						if (self->standingOn != NULL)
						{
							if (self->standingOn->activate != NULL)
							{
								printf("Trying to activate %s\n", self->standingOn->objectiveName);

								self = self->standingOn;

								self->activate(-1);

								self = &player;

								self->dirY = self->standingOn->speed;
							}

							input.down = 0;
						}
					}
				}

				if (input.attack == 1)
				{
					if (playerWeapon.inUse == TRUE)
					{
						self->animationCallback = &attackFinish;
						playerShield.animationCallback = &attackFinish;
						playerWeapon.animationCallback = &attackFinish;

						playerWeapon.flags |= ATTACKING;

						setEntityAnimation(&player, ATTACK_1);
						setEntityAnimation(&playerShield, ATTACK_1);
						setEntityAnimation(&playerWeapon, ATTACK_1);
					}

					input.attack = 0;
				}

				if (input.drop == 1)
				{
					dropInventoryItem();

					input.drop = 0;
				}

				if (input.interact == 1)
				{
					interactWithEntity(self->x, self->y, self->w, self->h);

					input.interact = 0;
				}

				if (input.grabbing == 1)
				{
					self->flags |= GRABBING;
				}

				if (input.activate == 1)
				{
					useInventoryItem();

					input.activate = 0;
				}

				if (input.next == 1 || input.previous == 1)
				{
					selectNextInventoryItem(input.next == 1 ? 1 : -1);

					input.next = input.previous = 0;
				}

				if (input.jump == 1)
				{
					if (self->flags & ON_GROUND)
					{
						self->dirY = -JUMP_HEIGHT;
					}

					input.jump = 0;
				}

				if (input.fly == 1)
				{
					self->flags ^= FLY;

					input.fly = 0;
				}
			}
		}

		else
		{
			self->flags &= ~GRABBING;

			if (self->target != NULL)
			{
				self->target->flags &= ~HELPLESS;

				self->target = NULL;
			}
		}

		checkToMap(self);

		self->standingOn = NULL;
	}

	else
	{
		self->action();
	}
}

static void attackFinish()
{
	player.animationCallback = NULL;
	playerShield.animationCallback = NULL;
	playerWeapon.animationCallback = NULL;

	playerWeapon.flags &= ~(ATTACKING|ATTACK_SUCCESS);

	setEntityAnimation(&player, STAND);
	setEntityAnimation(&playerShield, STAND);
	setEntityAnimation(&playerWeapon, STAND);
}

void drawPlayer()
{
	self = &player;

	if (self->inUse == TRUE && (self->flags & NO_DRAW) == 0)
	{
		/* Draw the weapon */

		self = &playerWeapon;

		if (self->inUse == TRUE)
		{
			self->x = player.x;
			self->y = player.y;

			self->draw();
		}

		/* Draw the player */

		self = &player;

		self->draw();

		/* Draw the shield */

		self = &playerShield;

		if (self->inUse == TRUE)
		{
			self->x = player.x;
			self->y = player.y;

			self->draw();
		}
	}
}

void setPlayerShield(int val)
{
	playerShield = *self;

	playerShield.parent = &player;

	playerShield.face = player.face;
}

void setPlayerWeapon(int val)
{
	playerWeapon = *self;

	playerWeapon.parent = &player;

	playerWeapon.face = player.face;
}

void autoSetPlayerWeapon(Entity *newWeapon)
{
	if (playerWeapon.inUse == FALSE)
	{
		playerWeapon = *newWeapon;

		playerWeapon.parent = &player;

		playerWeapon.face = player.face;
	}
}

void autoSetPlayerShield(Entity *newWeapon)
{
	if (playerShield.inUse == FALSE)
	{
		playerShield = *newWeapon;

		playerShield.parent = &player;

		playerShield.face = player.face;
	}
}

static void takeDamage(Entity *other, int damage)
{
	if (!(player.flags & INVULNERABLE))
	{
		player.health -= damage;

		player.animationCallback = NULL;
		playerShield.animationCallback = NULL;
		playerWeapon.animationCallback = NULL;

		playerWeapon.flags &= ~(ATTACKING|ATTACK_SUCCESS);

		setEntityAnimation(&player, STAND);
		setEntityAnimation(&playerShield, STAND);
		setEntityAnimation(&playerWeapon, STAND);

		if (player.health > 0)
		{
			setCustomAction(&player, &helpless, 10);

			setCustomAction(&player, &invulnerable, 60);

			if (player.dirX == 0)
			{
				player.dirX = other->dirX < 0 ? -6 : 6;
			}

			else
			{
				player.dirX = player.dirX < 0 ? 6 : -6;
			}
		}
	}
}

int getDistanceFromPlayer(Entity *e)
{
	if (player.x < e->x)
	{
		return -getDistance(player.x + player.w, player.y, e->x, e->y);
	}

	return getDistance(player.x, player.y, e->x + e->w, e->y);
}

void writePlayerToFile(FILE *fp)
{
	self = &player;

	fprintf(fp, "{\n");
	fprintf(fp, "TYPE player_start\n");
	fprintf(fp, "NAME player_start\n");
	fprintf(fp, "START_X %d\n", (int)self->x);
	fprintf(fp, "START_Y %d\n", (int)self->y);
	fprintf(fp, "}\n\n");
}
