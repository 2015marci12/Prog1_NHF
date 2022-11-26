#pragma once
#include "Core.h"
#include "mIni.h"
#include <stdbool.h>

//All of the different options to fine-tune gameplay.
typedef struct DiffConfig
{

	float viewport_scale;
	float peek_scale;

	float g;
	float thrust_idle;
	float thrust_booster;
	float lift_coeff;
	float drag_coeff;
	float plane_mass;

	float booster_fuel;
	float booster_fuelconsumption;
	float booster_recharge_rate;
	float booster_particle_time;

	float cannon_shooting_time;
	float bullet_velocity;
	float bullet_lifeTime;
	float bullet_damage;
	float player_health;

	float arena_width;
	float arena_height;
	float wall_damage;

	float missile_damage;
	float missile_velocity;
	float missile_turnrate;
	float missile_lifetime;
	float missile_reload_enemy;
	float missile_engagement_range;

	float bullet_damage_enemy;
	float missile_damage_enemy;

	float structure_health;
	float vehicle_health;
	float aircraft_health;
	float upgrade_health_factor;

	float structure_score;
	float vehicle_score;
	float aircraft_score;
	float wave_scaling;
	float powerup_chance;

	float powerup_score;
	float powerup_missiles;
	float powerup_bombs;
	float powerup_health;
} DiffConfig;

//Load the game configuration from a file.
bool LoadConfig(DiffConfig* config, const char* filepath);
