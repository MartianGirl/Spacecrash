// core.h
#ifndef _CORE_H_
#define _CORE_H_

// Math
inline float CORE_FRand(float from, float to)
{
  return from + (to - from) * (rand() / (float)RAND_MAX);
}

inline unsigned CORE_URand(unsigned from, unsigned to)
{
  return from + rand() % ((unsigned)(to - from + 1));
}

inline bool CORE_RandChance(float chance)
{
  return CORE_FRand(0.0f, 1.0f) < chance;
}

inline float CORE_FSquare(float f)
{
  return f * f;
}

// Colors
struct rgba { float r, g, b, a; };
inline rgba makergba(float r, float g, float b, float a)
{
  rgba c;
  c.r = r;
  c.g = g;
  c.b = b;
  c.a = a;
  return c;
}

#define RGBA(rr,gg,bb,aa) makergba(rr/255.0f, gg/255.0f, bb/255.0f, aa/255.0f)
#define COLOR_WHITE makergba(1.0f,1.0f,1.0f,1.0f)

// Bitmap/texture functions
int CORE_LoadBmp(const char filename[], bool wrap);
ivec2 CORE_GetBmpSize(int texix);
GLuint CORE_GetBmpOpenGLTex(int texix);
void CORE_UnloadBmp(int texix);
void CORE_RenderCenteredSprite(vec2 pos, vec2 size, int texix,
    rgba color = COLOR_WHITE, bool additive = false);

// Sound
bool CORE_InitSound();
void CORE_EndSound();
uint CORE_LoadWav(const char filename[]);
void CORE_UnloadWav(uint snd);
void CORE_PlaySound(uint snd, float volume, float pitch);
void CORE_PlayLoopSound(unsigned loopchannel, ALuint snd,
    float volume, float pitch);
void CORE_SetLoopSoundParam(unsigned loopchannel, float volume, float pitch);

#endif
// vim: set et sw=2 ts=2:
