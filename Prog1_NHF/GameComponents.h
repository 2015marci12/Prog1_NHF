#pragma once
#include "Core.h"

#include "GameScene.h"

//A component that imitates airplane-like movement.
typedef struct PlaneComponent
{
	float liftcoeff;
	float dragcoeff;
	float thrust;
	float mass;
} PlaneComponent;

//Register the plane component type in a scene.
void RegisterPlane(Scene_t* scene);
//Update the entities that have a plane component.
void MovePlanes(Game* game, float dt);

//A component that holds all information concerning a player.
typedef struct PlayerComponent
{
	float fuel;
	float max_fuel;
	Timer_t boosterParticleTimer;
	Timer_t shootingTimer;
	int selected_weapon;
	bool releasedAfterFiring;
	uint32_t MissileAmmo;
	uint32_t BombAmmo;
} PlayerComponent;

//Regster the player component type in a scene.
void RegisterPlayer(Scene_t* scene);
//Update the player entity.
void UpdatePlayer(Game* game, InputState* input, float dt);

//A component representing the health of an entity. Can call a callback function on death.
//Produces particles depending on damage.
typedef struct HealthComponent
{
	float health;
	float max_health;
	float invincibility_time;
	Timer_t lastHit;
	Timer_t lastParticle;
	uint64_t score;
	HealthIsZeroCallback_t cb;
} HealthComponent;

//Register the health component type in a scene.
void RegisterHealth(Scene_t* scene);
//Update entities with a health component.
void UpdateHealth(Game* game, float dt);

//The type of projectile a component denotes. Affects the death particle effects.
typedef enum ProjectileType 
{
	BULLET,
	MISSILE,
	BOMB,
} ProjectileType;

//A component that denotes a projectile entity that deals damage.
typedef struct ProjectileComponent
{
	float damage;
	ProjectileType type;

} ProjectileComponent;

//A component that marks an entity as a tank and is responsible for its behaviour.
typedef struct TankHullAI
{
	bool IsMissleTruck;
} TankHullAI;

//Register the tank AI component type in a scene.
void RegisterTankAIs(Scene_t* scene);
//Update the tanks.
void UpdateTankAIs(Game* game, float dt, entity_t player);

//An entity that is a gun turret targeting the player. A child of another entity, either tank or building.
typedef struct GunTurretAI 
{
	Timer_t reloadTimer;
} GunTurretAI;

//Register the gun AI component type in a scene.
void RegisterGunAIs(Scene_t* scene);
//Update the gun turrets.
void UpdateGunTurretAIs(Game* game, float dt, entity_t player);

//A component that guides a missile to a target. If the target is destroyed, the missile will travel in a straight line.
typedef struct MissileGuidanceComponent
{
	entity_t target;
	Timer_t lifeTime;
	Timer_t particleTimer;
} MissileGuidanceComponent;

//Register the missile guidance component type in a scene.
void RegisterMissiles(Scene_t* scene);
//Update missile entities.
void UpdateMissiles(Game* game, float dt);

//An entity that fires missiles in regular intervals.
typedef struct MissileLauncherAI 
{
	Timer_t realoadTimer;
} MissileLauncherAI;

//Register the missile launcher component type.
void RegisterMissileLaunchers(Scene_t* scene);
//Update all missile launchers.
void UpdateMissileLaunchers(Game* game, float dt);

//A component responsible for fighter jet behaviour.
typedef struct FighterAI 
{
	Timer_t reloadTimer;
	int burstCounter;
} FighterAI;

//Register the figter AI component type.
void RegisterFighterAI(Scene_t* scene);
//Update all enemy fighter jets in the air.
void UpdateFighters(Game* game, float dt);

//An enum that denotes the type of a bonus powerup.
typedef enum PowerupType 
{
	HEALTH,
	MISSILES,
	BOMBS,
	SCORE,
	POWERUP_MAX,
} PowerupType;

//A component that denotes a powerup entity.
typedef struct PowerupComponent 
{
	PowerupType type;
} PowerupComponent;

//Register the powerup component type.
void RegisterPowerup(Scene_t* scene);
//Resolve collisions that involve a powerup entity.
void ResolveCollisionPowerup(Game* game, entity_t a, entity_t b);

//A component that animates the radar towers.
typedef struct BonusTowerAnimComponent 
{
	Timer_t time;
} BonusTowerAnimComponent;

//Register bonus tower components.
void RegisterBonusTower(Scene_t* scene);
//Update the bonus tower animations.
void UpdateBonusTowers(Game* game);

//An entity that marks every enemy and stores whether they are upgraded versions of their respective type.
typedef struct EnemyTagComponent 
{
	bool upgraded;
} EnemyTagComponent;

//A component that marks the walls.
typedef struct WallTagComponent 
{
	char dummy;
} WallTagComponent;

//Register the tag components.
void RegisterTags(Scene_t* scene);
//Resolve collisions involving the wall and deal out wall damage.
void ResolveCollisionWall(Game* game, entity_t a, entity_t b);

//Register projectile components.
void RegisterProjectile(Scene_t* scene);
//Resove collisions involving projectiles, and deal damage.
void ResolveCollisionProjectiles(Game* game, entity_t a, entity_t b);

//Spawn a bonus.
void SpawnPowerup(Game* game, mat4 transform, PowerupType type);

//Spawn a radar entity.
void SpawnRadar(Game* game, vec2 Pos, bool flip, bool upgrade);
//Spawn a building.
void SpawnTurret(Game* game, vec2 Pos, bool flip, bool MissileTurret, bool upgrade);
//Spawn a tank or missile truck.
void SpawnTank(Game* game, vec2 Pos, bool flip, bool MissileTruck, bool upgrade);
//Spawn a missile
void SpawnMissile(Game* game, vec2 Pos, vec2 Dir, int32_t alliegence, float damage, entity_t target);
//Spawn a figter jet.
void SpawnFighter(Game* game, vec2 Pos, bool Upgrade);

//Register all non-standard components used in the game.
void RegisterGameComponents(Scene_t* scene);