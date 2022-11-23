#include "DiffConfig.h"

#define STRING_CHECK_SET_VAL(ptr, idstring, val, valname) if(strcmp(idstring, #valname) == 0) ptr-> ## valname = (float)atof(val);

int DiffIniHandler(const void* user, const char* section, const char* id, const char* value) 
{
	DiffConfig* ptr = user;

	STRING_CHECK_SET_VAL(ptr, id, value, viewport_scale);
	STRING_CHECK_SET_VAL(ptr, id, value, peek_scale);

	STRING_CHECK_SET_VAL(ptr, id, value, g);
	STRING_CHECK_SET_VAL(ptr, id, value, thrust_idle);
	STRING_CHECK_SET_VAL(ptr, id, value, thrust_booster);
	STRING_CHECK_SET_VAL(ptr, id, value, lift_coeff);
	STRING_CHECK_SET_VAL(ptr, id, value, drag_coeff);
	STRING_CHECK_SET_VAL(ptr, id, value, plane_mass);

	STRING_CHECK_SET_VAL(ptr, id, value, booster_fuel);
	STRING_CHECK_SET_VAL(ptr, id, value, booster_fuelconsumption);
	STRING_CHECK_SET_VAL(ptr, id, value, booster_recharge_rate);
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
	STRING_CHECK_SET_VAL(ptr, id, value, missile_lifetime);
	STRING_CHECK_SET_VAL(ptr, id, value, missile_reload_enemy);
	STRING_CHECK_SET_VAL(ptr, id, value, missile_engagement_range);
	STRING_CHECK_SET_VAL(ptr, id, value, bullet_damage_enemy);
	STRING_CHECK_SET_VAL(ptr, id, value, missile_damage_enemy);

	STRING_CHECK_SET_VAL(ptr, id, value, structure_health);
	STRING_CHECK_SET_VAL(ptr, id, value, vehicle_health);
	STRING_CHECK_SET_VAL(ptr, id, value, aircraft_health);
	STRING_CHECK_SET_VAL(ptr, id, value, upgrade_health_factor);

	STRING_CHECK_SET_VAL(ptr, id, value, structure_score);
	STRING_CHECK_SET_VAL(ptr, id, value, vehicle_score);
	STRING_CHECK_SET_VAL(ptr, id, value, aircraft_score);
	STRING_CHECK_SET_VAL(ptr, id, value, wave_scaling);
	STRING_CHECK_SET_VAL(ptr, id, value, balloon_chance);

	return 0;
}

bool LoadConfig(DiffConfig* config, const char* filepath)
{
	return mIni_File(filepath, DiffIniHandler, config) >= 0;
}
