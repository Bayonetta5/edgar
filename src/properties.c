#include "properties.h"

extern void loadSpritesFromFile(char *);
extern void loadAnimationData(char *);

void loadProperties(char *name, Entity *e)
{
	int i, j, index;
	char path[255], line[MAX_LINE_LENGTH];
	FILE *fp;
	
	sprintf(path, "data/props/%s.props", name);
	
	index = -1;
	
	for (i=0;i<MAX_PROPS_FILES;i++)
	{
		if (strcmp(properties[i].name, name) == 0)
		{
			index = i;
			
			break;
		}
	}
		
	if (index == -1)
	{
		for (i=0;i<MAX_PROPS_FILES;i++)
		{
			if (strlen(properties[i].name) == 0)
			{
				printf("Loading properties for %s to slot %d\n", name, i);
				
				fp = fopen(path, "rb");
				
				if (fp == NULL)
				{
					printf("Failed to open properties file %s\n", path);
					
					exit(1);
				}
				
				strcpy(properties[i].name, name);
				
				j = 0;
				
				while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
				{
					if (j == MAX_PROPS_ENTRIES)
					{
						printf("Cannot add any more properities\n");
						
						exit(1);
					}
					
					if (line[0] == '#' || line[0] == '\n')
					{
						continue;
					}
					
					sscanf(line, "%s %s", properties[i].key[j], properties[i].value[j]);
					
					if (strcmp(properties[i].key[j], "GFX_FILE") == 0)
					{
						loadSpritesFromFile(properties[i].value[j]);
					}
					
					if (strcmp(properties[i].key[j], "ANIM_FILE") == 0)
					{
						loadAnimationData(properties[i].value[j]);
					}
					
					j++;
				}
				
				fclose(fp);
				
				break;
			}
		}
		
		if (i == MAX_PROPS_FILES)
		{
			printf("No free slots for properties file %s\n", name);
			
			exit(1);
		}
	}
	
	index = 0;
	
	for (j=0;j<MAX_PROPS_ENTRIES;j++)
	{
		if (strcmp(properties[i].key[j], "HEALTH") == 0)
		{
			e->health = atoi(properties[i].value[j]);
		}
		
		else if (strcmp(properties[i].key[j], "STAND_LEFT") == 0)
		{
			e->animation[STAND_LEFT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "STAND_RIGHT") == 0)
		{
			e->animation[STAND_RIGHT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "WALK_LEFT") == 0)
		{
			e->animation[WALK_LEFT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "WALK_RIGHT") == 0)
		{
			e->animation[WALK_RIGHT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "JUMP_LEFT") == 0)
		{
			e->animation[JUMP_LEFT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "JUMP_RIGHT") == 0)
		{
			e->animation[JUMP_RIGHT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "ATTACK_1_LEFT") == 0)
		{
			e->animation[ATTACK_1_LEFT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "ATTACK_1_RIGHT") == 0)
		{
			e->animation[ATTACK_1_RIGHT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "DIE_LEFT") == 0)
		{
			e->animation[DIE_LEFT] = atoi(properties[i].value[j]);
			
			index++;
		}
		
		else if (strcmp(properties[i].key[j], "DIE_RIGHT") == 0)
		{
			e->animation[DIE_RIGHT] = atoi(properties[i].value[j]);
			
			index++;
		}
	}
	
	if (index == 0)
	{
		printf("No animations defined for %s\n", name);
		
		exit(1);
	}
	
	e->currentAnim = e->animation[0];
}