/*
Copyright (C) 2009 Parallel Realities

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
#include "../hud.h"
#include "../collisions.h"
#include "../inventory.h"
#include "key_items.h"
#include "../audio/audio.h"

extern Entity *self;

static void wait(void);
static void shockTarget(void);
static void shockEnd(void);
static void activate(int);

Entity *addTeslaPack(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		printf("No free slots to add a Tesla Pack\n");

		exit(1);
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->type = KEY_ITEM;

	e->face = RIGHT;

	e->action = &wait;

	e->touch = NULL;

	e->activate = &activate;

	e->draw = &drawLoopingAnimationToMap;

	setEntityAnimation(e, STAND);

	return e;
}

static void wait()
{
	if (self->target != NULL)
	{
		self->x = self->target->x + self->target->w / 2;
		self->y = self->target->y + self->target->w / 2;

		self->x -= self->w / 2;
		self->y -= self->h / 2;

		self->thinkTime--;

		if (self->thinkTime <= 0)
		{
			shockTarget();
		}
	}

	else
	{
		checkToMap(self);
	}
}

static void shockTarget()
{
	self->target->health -= 100;

	self->target->maxHealth -= 100;

	self->thinkTime = 120;

	self->action = &shockEnd;
}

static void shockEnd()
{
	self->x = self->target->x;
	self->y = self->target->y;

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->target = NULL;

		self->touch = &keyItemTouch;

		self->action = &wait;
	}
}

static void activate(int val)
{
	if (self->target != NULL)
	{
		self->thinkTime = 60;

		addEntity(*self, self->target->x, self->target->y);

		self->inUse = FALSE;
	}
}