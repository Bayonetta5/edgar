/*
Copyright (C) 2009-2012 Parallel Realities

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
Foundation, 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.
*/

#include "../headers.h"

#include "../audio/audio.h"
#include "../draw.h"
#include "../graphics/graphics.h"
#include "../init.h"
#include "../system/error.h"
#include "about_menu.h"
#include "main_menu.h"
#include "widget.h"

extern Input input, menuInput;
extern Game game;
extern Control control;

static Menu menu;

static void loadMenuLayout(void);
static void doMenu(void);
static void showMainMenu(void);

void drawAboutMenu()
{
	int i;

	drawImage(menu.background, menu.x, menu.y, FALSE, 196);

	for (i=0;i<menu.widgetCount;i++)
	{
		drawWidget(menu.widgets[i], &menu, menu.index == i);
	}
}

static void doMenu()
{
	if (input.attack == TRUE || menuInput.attack == TRUE)
	{
		menuInput.attack = FALSE;
		input.attack = FALSE;

		playSound("sound/common/click.ogg");

		showMainMenu();
	}
}

static void loadMenuLayout()
{
	char versionText[MAX_VALUE_LENGTH];

	menu.widgetCount = 3;

	menu.widgets = malloc(sizeof(Widget *) * menu.widgetCount);

	snprintf(versionText, MAX_VALUE_LENGTH, _("The Legend of Edgar v%0.2f"), VERSION);

	if (menu.widgets == NULL)
	{
		showErrorAndExit("Ran out of memory when creating About Menu");
	}

	menu.widgets[0] = createWidget(versionText, NULL, NULL, NULL, NULL, -1, 20, FALSE, 255, 255, 255);

	menu.widgets[1] = createWidget(_("Copyright Parallel Realities 2009 - 2012"), NULL, NULL, NULL, NULL, -1, 70, FALSE, 255, 255, 255);

	menu.widgets[2] = createWidget(_("OK"), NULL, NULL, NULL, NULL, -1, 120, TRUE, 255, 255, 255);

	/* Resize */

	resizeMenu(&menu);

	menu.background = addBorder(createSurface(menu.w, menu.h), 255, 255, 255, 0, 0, 0);

	menu.x = (SCREEN_WIDTH - menu.background->w) / 2;
	menu.y = (SCREEN_HEIGHT - menu.background->h) / 2;
}

Menu *initAboutMenu()
{
	menu.action = &doMenu;

	freeAboutMenu();

	loadMenuLayout();

	menu.index = 2;

	menu.returnAction = &showMainMenu;

	return &menu;
}

void freeAboutMenu()
{
	int i;

	if (menu.widgets != NULL)
	{
		for (i=0;i<menu.widgetCount;i++)
		{
			freeWidget(menu.widgets[i]);
		}

		free(menu.widgets);
	}

	if (menu.background != NULL)
	{
		SDL_FreeSurface(menu.background);

		menu.background = NULL;
	}
}

static void showMainMenu()
{
	game.menu = initMainMenu();

	game.drawMenu = &drawMainMenu;
}
