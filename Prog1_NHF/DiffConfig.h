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

	float booster_fuelconsumption;
	float booster_particle_time;

	float cannon_shooting_time;
	float bullet_velocity;
	float bullet_lifeTime;
	float bullet_damage;
	float player_health;

	float arena_width;
	float arena_height;

	float missile_damage;
	float missile_velocity;
	float missile_turnrate;

	float bullet_damage_enemy;
	float missile_damage_enemy;
	float enemy_structure_health;
	float enemy_vehicle_health;
	float enemy_aircraft_health;
	float enemy_upgrade_health_factor;

	float enemy_structure_spawnrate_const;
	float enemy_structure_spawnrate_lin;
	float enemy_structure_spawnrate_max;

	float enemy_vehicle_spawnrate_const;
	float enemy_vehicle_spawnrate_lin;
	float enemy_vehicle_spawnrate_max;

	float enemy_aircraft_spawnrate_const;
	float enemy_aircraft_spawnrate_lin;
	float enemy_aircraft_spawnrate_max;

	float diff_structure_score;
	float diff_vehicle_score;
	float diff_aircraft_score;
	float diff_score_time_lin;
	float diff_combo_scaling;
	float diff_combo_max;
	float diff_combo_timeout;
} DiffConfig;

//Load a difficulty from a config file.
bool LoadDiff(DiffConfig* config, const char* filepath, const char* diffname);
