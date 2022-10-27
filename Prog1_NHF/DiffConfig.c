#include "DiffConfig.h"

#define STRING_CHECK_SET_VAL(ptr, idstring, val, valname) if(strcmp(idstring, #valname) == 0) ptr-> ## valname = (float)atof(val);

struct PassDiffAndName 
{
	DiffConfig* ptr;
	const char* diffname;
};

int DiffIniHandler(const void* user, const char* section, const char* id, const char* value) 
{
	struct PassDiffAndName* data = user;
	DiffConfig* ptr = data->ptr;

	if (strcmp(section, data->diffname) != 0) return 0;
	STRING_CHECK_SET_VAL(ptr, id, value, viewport_scale);
	STRING_CHECK_SET_VAL(ptr, id, value, peek_scale);

	STRING_CHECK_SET_VAL(ptr, id, value, g);
	STRING_CHECK_SET_VAL(ptr, id, value, thrust_idle);
	STRING_CHECK_SET_VAL(ptr, id, value, thrust_booster);
	STRING_CHECK_SET_VAL(ptr, id, value, lift_coeff);
	STRING_CHECK_SET_VAL(ptr, id, value, drag_coeff);
	STRING_CHECK_SET_VAL(ptr, id, value, plane_mass);

	STRING_CHECK_SET_VAL(ptr, id, value, booster_fuelconsumption);
	STRING_CHECK_SET_VAL(ptr, id, value, booster_particle_time);
	STRING_CHECK_SET_VAL(ptr, id, value, cannon_shooting_time);
	STRING_CHECK_SET_VAL(ptr, id, value, bullet_velocity);
	STRING_CHECK_SET_VAL(ptr, id, value, bullet_lifeTime);
	STRING_CHECK_SET_VAL(ptr, id, value, bullet_damage);
	STRING_CHECK_SET_VAL(ptr, id, value, player_health);

	STRING_CHECK_SET_VAL(ptr, id, value, arena_width);
	STRING_CHECK_SET_VAL(ptr, id, value, arena_height);

	STRING_CHECK_SET_VAL(ptr, id, value, missile_damage);
	STRING_CHECK_SET_VAL(ptr, id, value, missile_velocity);
	STRING_CHECK_SET_VAL(ptr, id, value, missile_turnrate);
	STRING_CHECK_SET_VAL(ptr, id, value, bullet_damage_enemy);
	STRING_CHECK_SET_VAL(ptr, id, value, missile_damage_enemy);

	STRING_CHECK_SET_VAL(ptr, id, value, enemy_structure_health);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_vehicle_health);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_aircraft_health);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_upgrade_health_factor);

	STRING_CHECK_SET_VAL(ptr, id, value, enemy_structure_spawnrate_const);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_structure_spawnrate_lin);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_structure_spawnrate_max);

	STRING_CHECK_SET_VAL(ptr, id, value, enemy_vehicle_spawnrate_const);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_vehicle_spawnrate_lin);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_vehicle_spawnrate_max);

	STRING_CHECK_SET_VAL(ptr, id, value, enemy_aircraft_spawnrate_const);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_aircraft_spawnrate_lin);
	STRING_CHECK_SET_VAL(ptr, id, value, enemy_aircraft_spawnrate_max);

	STRING_CHECK_SET_VAL(ptr, id, value, diff_structure_score);
	STRING_CHECK_SET_VAL(ptr, id, value, diff_vehicle_score);
	STRING_CHECK_SET_VAL(ptr, id, value, diff_aircraft_score);
	STRING_CHECK_SET_VAL(ptr, id, value, diff_score_time_lin);
	STRING_CHECK_SET_VAL(ptr, id, value, diff_combo_scaling);
	STRING_CHECK_SET_VAL(ptr, id, value, diff_combo_max);
	STRING_CHECK_SET_VAL(ptr, id, value, diff_combo_timeout);
	return 0;
}

bool LoadDiff(DiffConfig* config, const char* filepath, const char* diffname)
{
	struct PassDiffAndName p;
	p.ptr = config;
	p.diffname = diffname;
	return mIni_File(filepath, DiffIniHandler, &p) >= 0;
}
