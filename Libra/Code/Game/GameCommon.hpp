#pragma once
struct Vec2;
struct Rgba8;
struct Vertex_PCU;
class App;
class Game;
class Renderer;
class RandomNumberGenerator;
class InputSystem;
class AudioSystem;
class Texture;
class NamedStrings;
class SpriteSheet;
class SpriteAnimDefinition;

typedef size_t SoundID;
typedef size_t SoundPlaybackID;

enum Music
{
	MUSIC_NULL = -1,

	MUSIC_WELCOME,
	MUSIC_BGM,
	MUSIC_PAUSE,
	MUSIC_UNPAUSE,
	MUSIC_START,
	MUSIC_QUIT,
	MUSIC_PLAYER_SHOOT,
	MUSIC_ENEMY_SHOOT,
	MUSIC_PLAYER_HIT,
	MUSIC_ENEMY_HIT,
	MUSIC_PLAYER_DIE,
	MUSIC_ENEMY_DIE,
	MUSIC_VICTORY,

	MUSIC_NUM
};

enum TexturePics
{
	TEXTURE_NULL = -1,

	TEXTURE_TANKBASE,
	TEXTURE_TANKTOP,
	TEXTURE_TERRAIN,
	TEXTURE_ENEMY_ARIES,
	TEXTURE_ENEMY_LEO,
	TEXTURE_ENEMY_CAPRICORN,
	TEXTURE_ENEMY_SCORPIO_BASE,
	TEXTURE_ENEMY_SCORPIO_TOP,
	TEXTURE_PLAYER_BULLET,
	TEXTURE_ENEMY_BULLET,
	TEXTURE_PLAYER_BOLT,
	TEXTURE_ENEMY_BOLT,
	TEXTURE_ATTRACT_SCREEN,
	TEXTURE_VICTORY_SCREEN,
	TEXTURE_YOUDIED_SCREEN,
	TEXTURE_TEST_ANIM,
	TEXTURE_EXPLOSION,

	TEXTURE_NUM
};

extern App*						g_theApp;
extern Renderer*				g_theRenderer;
extern RandomNumberGenerator*	g_rng;
extern InputSystem*				g_theInput;
extern AudioSystem*				g_theAudio;
extern Game*					g_theGame;
extern SpriteSheet*				g_terrainSprites;
extern SpriteAnimDefinition*	g_explosionAnim;
extern int						TERRAIN_SPRITE_WIDTH;
extern int						TERRAIN_SPRITE_HEIGHT;

extern bool						g_debugDraw;
extern bool						g_debugNoclip;
extern bool						g_debugHeatMap;
extern bool						g_fastSpeed;
extern bool						g_slowSpeed;

extern SoundID					g_musicLibrary[MUSIC_NUM];
extern SoundPlaybackID			g_lastPlayedMusic[MUSIC_NUM];
extern Texture*					g_textureLibrary[TEXTURE_NUM];

constexpr int			MAP_NUM = 7;

constexpr int			MAP_DIMENSION_X[MAP_NUM]	= { 25, 25, 32 };
constexpr int			MAP_DIMENSION_Y[MAP_NUM]	= { 32, 25, 20 };
//constexpr int			MAP_ARIES_NUM[MAP_NUM]		= { 2, 3, 4 };
//constexpr int			MAP_LEO_NUM[MAP_NUM]		= { 2, 3, 4 };
//constexpr int			MAP_SCORPIO_NUM[MAP_NUM]	= { 2, 3, 4 };
//constexpr float			MAP_SPRINKLE1[MAP_NUM]		= { 10.f, 20.f, 30.f};
//constexpr float			MAP_SPRINKLE2[MAP_NUM]		= { 10.f, 20.f, 30.f };
//constexpr int			TERRAIN_SPRITESHEET_X = 8;
//constexpr int			TERRAIN_SPRITESHEET_Y = 8;
//constexpr float			WORLD_SIZE_X = 25.f;
//constexpr float			WORLD_SIZE_Y = 32.f;
//constexpr float			WORLD_CAMERA_SIZE_X = 16.f;
//constexpr float			WORLD_CAMERA_SIZE_Y = 8.f;
//constexpr float			SCREEN_CAMERA_SIZE_X = 1600.f;
//constexpr float			SCREEN_CAMERA_SIZE_Y = 800.f;
//constexpr float			WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
//constexpr float			WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
//constexpr float			THICKNESS_DEBUGDRAW = 0.05f;
//constexpr float			ENTITY_DEFAULT_SPEED = 0.8f;
//constexpr float			ENTITY_VISIABLE_DISTANCE = 10.f;
//
//constexpr float			PLAYER_PHYSICS_RADIUS = 0.3f;
//constexpr float			PLAYER_COSMETIC_RADIUS = 0.5f;
//constexpr float			PLAYER_TEXTURE_RADIUS = 0.5f;
//constexpr float			PLAYER_ANGULAR_VELOCITY = 180.f;
//constexpr float			PLAYER_TURRET_ANGULAR_VELOCITY = 360.f;
//constexpr float			PLAYER_SPEED = 1.f;
//constexpr int			PLAYER_HEALTH = 20;
//
//constexpr float			ARIES_PHTSICS_RADIUS = 0.3f;
//constexpr float			ARIES_COSMETIC_RADIUS = 0.5f;
//constexpr float			ARIES_TEXTURE_RADIUS = 0.5f;
//constexpr float			ARIES_SPEED = 0.5f;
//constexpr float			ARIES_ANGULAR_VELOCITY = 90.f;
//constexpr int			ARIES_HEALTH = 5;
//
//constexpr float			LEO_PHTSICS_RADIUS = 0.3f;
//constexpr float			LEO_COSMETIC_RADIUS = 0.5f;
//constexpr float			LEO_TEXTURE_RADIUS = 0.5f;
//constexpr float			LEO_SPEED = 0.5f;
//constexpr float			LEO_ANGULAR_VELOCITY = 90.f;
//constexpr int			LEO_HEALTH = 5;
//
//constexpr float			SCORPIO_PHTSICS_RADIUS = 0.3f;
//constexpr float			SCORPIO_COSMETIC_RADIUS = 0.5f;
//constexpr float			SCORPIO_TEXTURE_RADIUS = 0.5f;
//constexpr float			SCORPIO_ANGULAR_VELOCITY = 90.f;
//constexpr float			SCORPIO_MAXDISTANCE = 10.f;
//constexpr int			SCORPIO_HEALTH = 5;
//
//constexpr float			BULLET_TEXTURE_RADIUS = 0.05f;
//constexpr int			BULLET_BOUNCING_TIMES = 2;
//
//constexpr float			GOOD_BULLET_SPEED = 5.f;
//constexpr float			EVIL_BULLET_SPEED = 5.f;
//constexpr float			GOOD_BOLT_SPEED = 5.f;
//constexpr float			EVIL_BOLT_SPEED = 5.f;

void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& start, float forwardOrientationDegree,float length, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& start, Vec2 const& end, float thickness, Rgba8 color);
void DrawRay(Vec2 const& start, float forwardOrientationDegree, float length, float thickness, Rgba8 const& color);
void DrawCircle(Vec2 const& position, float radius, Rgba8 color);
void DrawSquare(Vec2 const& start, Vec2 const& end, Rgba8 color);
void DrawStarRing(Vec2 const& center, float radius, float thickness, Rgba8 color);
void DrawStarCircle(Vec2 const& center, float radius, float thickness, float gap, Rgba8 color);
void DrawLaser(Vec2 const& center, float radius, float thickness, float startDegree, float endDegree, Rgba8 color);
void DrawStarCurve(Vec2 const& center, float radius, float thickness, float startDegree, float endDegree, Rgba8 color);
void DrawStarCurves(Vec2 const& center, float radius, float thickness, float gap, float width, float startDegree, float endDegree, Rgba8 color);
void PlayMusicLoop(Music music);
void PlayMusic(Music music);
void StopMusic(Music music);
void PauseMusic(Music music);
void ResumeMusic(Music music);
void SetMusicSpeed(Music music, float speed);