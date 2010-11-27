/*
Copyright (C) 2009-2010 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "../headers.h"

#include "../graphics/animation.h"
#include "../system/properties.h"
#include "../entity.h"
#include "../collisions.h"
#include "../system/random.h"
#include "../audio/audio.h"
#include "../audio/music.h"
#include "../graphics/graphics.h"
#include "../custom_actions.h"
#include "../graphics/decoration.h"
#include "../game.h"
#include "../hud.h"
#include "../map.h"
#include "../item/key_items.h"
#include "../player.h"
#include "../enemy/enemies.h"
#include "../graphics/gib.h"
#include "../system/error.h"
#include "../world/target.h"

extern Entity *self, player;

static void initialise(void);
static void doIntro(void);
static void die(void);
static void attackFinished(void);
static void takeDamage(Entity *, int);
static void touch(Entity *);
static void introPause(void);
static void freezeBody(void);
static void immolateBody(void);
static void throwWallWalkerInit(void);
static void throwWallWalker(void);
static void chargePlayerInit(void);
static void rechargeInit(void);
static void fallout(void);
static void falloutPause(void);
static void jumpOut(void);
static void jumpOutWait(void);
static void noTouch(Entity *);
static void moveToRechargeTarget(void);
static void recharge(void);
static void incinerateInit(void);
static void incinerate(void);
static void incinerateWait(void);
static void fireWait(void);

Entity *addCaveBoss(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add the Cave Boss");
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->action = &initialise;

	e->draw = &drawLoopingAnimationToMap;

	e->die = &die;

	e->fallout = &fallout;

	e->type = ENEMY;

	e->active = FALSE;

	setEntityAnimation(e, STAND);

	return e;
}

static void initialise()
{
	if (self->active == TRUE)
	{
		self->flags &= ~NO_DRAW;

		if (cameraAtMinimum())
		{
			centerMapOnEntity(NULL);

			self->action = &doIntro;

			self->thinkTime = 60;

			self->touch = &entityTouch;

			self->takeDamage = &takeDamage;

			self->touch = &touch;

			setContinuePoint(FALSE, self->name, NULL);
		}
	}

	checkToMap(self);
}

static void doIntro()
{
	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		checkToMap(self);

		self->action = &introPause;

		setEntityAnimation(self, STAND);
	}
}

static void introPause()
{
	playDefaultBossMusic();

	initBossHealthBar();

	self->action = &attackFinished;

	checkToMap(self);

	self->startX = 0;

	self->endY = 25;

	self->endX = 0;
}

static void entityWait()
{
	int action;

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		switch ((int)self->startX)
		{
			case -1:
				if (self->startY > 0)
				{
					action = prand() % 2;

					switch (action)
					{
						case 0:
							self->action = &throwWallWalkerInit;
						break;

						default:
							self->action = &chargePlayerInit;
						break;
					}
				}

				else
				{
					self->action = &rechargeInit;
				}
			break;

			case 0:
				switch ((int)self->endX)
				{
					self->action = prand() % 2 == 0 ? &immolateBody : &freezeBody;
				}
			break;

			case 1: /* Fire */
				action = prand() % 3;

				switch (action)
				{
					case 0:
						self->action = &throwWallWalkerInit;
					break;

					default:
						self->action = &chargePlayerInit;
					break;
				}
			break;

			default: /* Ice */
				action = prand() % 3;

				switch (action)
				{
					case 0:
						self->action = &throwWallWalkerInit;
					break;

					default:
						self->action = &chargePlayerInit;
					break;
				}
			break;
		}
		
		self->action = &incinerateInit;
	}

	checkToMap(self);
}

static void rechargeInit()
{
	Target *t;

	t = getTargetByName(prand() % 2 == 0 ? "CAVE_BOSS_TARGET_LEFT" : "CAVE_BOSS_TARGET_RIGHT");

	if (t == NULL)
	{
		showErrorAndExit("Cave Boss cannot find target");
	}

	self->targetX = t->x;

	self->targetY = self->y;

	self->action = &moveToRechargeTarget;

	self->touch = &noTouch;

	self->face = self->targetX < self->x ? LEFT : RIGHT;

	self->dirX = self->face == LEFT ? -self->speed : self->speed;

	checkToMap(self);
}

static void moveToRechargeTarget()
{
	if (atTarget())
	{
		self->thinkTime = 600;

		self->action = &recharge;
	}

	checkToMap(self);
}

static void recharge()
{
	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->action = &attackFinished;
	}

	checkToMap(self);
}

static void throwWallWalkerInit()
{
	self->mental = 1 + prand() % 5;

	self->thinkTime = 30;

	self->action = &throwWallWalker;

	checkToMap(self);
}

static void throwWallWalker()
{
	Entity *e;

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		e = addEnemy("enemy/wall_walker", self->x, self->y);

		e->face = self->face;

		e->dirY = ITEM_JUMP_HEIGHT;

		e->flags |= LIMIT_TO_SCREEN;

		e->dirX = e->face == LEFT ? -e->speed : e->speed;

		self->mental--;

		self->thinkTime = 60;

		if (self->mental <= 0)
		{
			self->action = &attackFinished;
		}
	}

	checkToMap(self);
}

static void chargePlayerInit()
{
	self->action = &attackFinished;
}

static void freezeBody()
{
	self->startX = 1;

	self->startY = 8;

	playSoundToMap("sound/common/freeze.ogg", BOSS_CHANNEL, self->x, self->y, 0);

	self->action = &attackFinished;
}

static void immolateBody()
{
	self->startX = 2;

	self->startY = 8;

	playSoundToMap("sound/enemy/fireball/fireball.ogg", BOSS_CHANNEL, self->x, self->y, 0);

	self->action = &attackFinished;
}

static void attackFinished()
{
	self->mental = 0;

	self->thinkTime = 120;

	self->action = &entityWait;
}

static void takeDamage(Entity *other, int damage)
{
	Entity *temp;

	if (self->flags & INVULNERABLE)
	{
		return;
	}

	if (self->startX == -1)
	{
		if (self->flags & INVULNERABLE)
		{
			return;
		}

		if (damage != 0)
		{
			self->startY--;

			if (other->type == PROJECTILE)
			{
				temp = self;

				self = other;

				self->die();

				self = temp;
			}

			setCustomAction(self, &flashWhite, 6, 0, 0);

			/* Don't make an enemy invulnerable from a projectile hit, allows multiple hits */

			if (other->type != PROJECTILE)
			{
				setCustomAction(self, &invulnerableNoFlash, HIT_INVULNERABLE_TIME, 0, 0);
			}

			if (self->pain != NULL)
			{
				self->pain();
			}

			if (self->startY <= 0)
			{

			}
		}
	}

	else
	{
		playSoundToMap("sound/common/dink.ogg", EDGAR_CHANNEL, self->x, self->y, 0);

		if (other->reactToBlock != NULL)
		{
			temp = self;

			self = other;

			self->reactToBlock();

			self = temp;
		}

		if (prand() % 10 == 0)
		{
			setInfoBoxMessage(60, 255, 255, 255, _("This weapon is not having any effect..."));
		}

		setCustomAction(self, &invulnerableNoFlash, HIT_INVULNERABLE_TIME, 0, 0);

		damage = 0;

		addDamageScore(damage, self);
	}
}

static void die()
{
	Entity *e;

	checkToMap(self);

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		clearContinuePoint();

		increaseKillCount();

		freeBossHealthBar();

		e = addKeyItem("item/heart_container", self->x + self->w / 2, self->y);

		e->dirY = ITEM_JUMP_HEIGHT;

		entityDieNoDrop();
	}
}

static void touch(Entity *other)
{
	if (other->type == KEY_ITEM && strcmpignorecase(other->name, "item_stalactite") == 0)
	{
		if (self->startX == -1)
		{
			self->takeDamage(other, 500);

			other->mental = -1;
		}

		else
		{
			other->action = &die;
		}
	}

	else if (self->startX == 1 && other->type == KEY_ITEM && strcmpignorecase(other->name, "item/ice_cube") == 0)
	{
		self->startY--;

		setCustomAction(self, &flashWhite, 6, 0, 0);
		setCustomAction(self, &invulnerableNoFlash, HIT_INVULNERABLE_TIME, 0, 0);

		enemyPain();

		other->inUse = FALSE;

		if (self->startY <= 0)
		{
			self->startY = 150;

			self->startX = -1;
		}
	}

	else if (self->startX == 2 && other->type == PROJECTILE && strcmpignorecase(other->name, "weapon/flaming_arrow") == 0)
	{
		self->startY--;

		setCustomAction(self, &flashWhite, 6, 0, 0);
		setCustomAction(self, &invulnerableNoFlash, HIT_INVULNERABLE_TIME, 0, 0);

		enemyPain();

		other->inUse = FALSE;

		if (self->startY <= 0)
		{
			self->startY = 150;

			self->startX = -1;
		}
	}

	else
	{
		entityTouch(other);
	}
}

static void fallout()
{
	int damage;

	if (self->startX == -1)
	{
		damage = 500;

		self->health -= damage;

		addDamageScore(damage, self);
	}

	self->thinkTime = 360;

	self->action = &falloutPause;

	checkToMap(self);
}

static void falloutPause()
{
	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->action = &jumpOut;

		self->touch = &noTouch;
	}

	checkToMap(self);
}

static void jumpOut()
{
	self->dirY = -8;

	self->flags |= FLY;

	self->damage = 200;

	self->thinkTime = 180;

	self->action = &jumpOutWait;

	checkToMap(self);
}

static void jumpOutWait()
{
	int screenMid;

	if (self->environment == AIR)
	{
		self->flags &= ~FLY;

		screenMid = getMapStartX() + SCREEN_WIDTH / 2;

		self->face = self->x > screenMid ? LEFT : RIGHT;

		self->dirX = self->face == LEFT ? -6 : 6;
	}

	checkToMap(self);

	if (self->flags & ON_GROUND)
	{
		self->dirX = 0;

		self->damage = 1;

		self->touch = &touch;

		self->action = &attackFinished;
	}
}

static void noTouch(Entity *other)
{

}

static void incinerateInit()
{
	self->thinkTime = 300;

	self->action = &incinerate;

	self->mental = 60;

	checkToMap(self);
}

static void incinerate()
{
	int x, startX;
	Entity *e;

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->mental = 0;

		startX = getMapStartX();

		x = 0;

		while (x < SCREEN_WIDTH)
		{
			e = getFreeEntity();

			if (e == NULL)
			{
				showErrorAndExit("No free slots to add the Fire");
			}

			loadProperties("boss/phoenix_die_fire", e);

			setEntityAnimation(e, STAND);

			e->x = startX + x;
			e->y = self->y + self->h - e->h;

			e->action = &fireWait;
			e->touch = &entityTouch;

			e->draw = &drawLoopingAnimationToMap;

			e->type = ENEMY;

			e->flags |= FLY;

			e->thinkTime = 30;

			e->damage = 1;

			e->health = 0;

			e->maxHealth = 5;

			e->mental = 1;

			e->head = self;

			x += e->w;

			self->mental++;
		}

		self->action = &incinerateWait;
	}

	checkToMap(self);
}

static void incinerateWait()
{
	if (self->mental <= 0)
	{
		self->action = &attackFinished;
	}

	checkToMap(self);
}

static void fireWait()
{
	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->health += self->mental;

		if (self->health == self->maxHealth)
		{
			self->maxHealth = 5;

			self->thinkTime = 180;

			self->mental *= -1;
		}

		else if (self->health < 0)
		{
			self->head->mental--;

			self->inUse = FALSE;

			self->health = 0;
		}

		else
		{
			self->thinkTime = 5;
		}

		setEntityAnimation(self, self->health);
	}

	checkToMap(self);
}
