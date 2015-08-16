// GNU/Linux test code
#include <unistd.h>
#include <GL/glfw.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#define SCR_WIDTH 640
#define SCR_HEIGHT 480
#define SCR_FULLSCREEN 0

typedef unsigned char byte;

int main(int argc, char *argv[])
{
  if (glfwInit() == GL_TRUE)
  {
    if (glfwOpenWindow(
          SCR_WIDTH, SCR_HEIGHT, 0, 0, 0, 0, 0, 0,
          SCR_FULLSCREEN ? GLFW_FULLSCREEN : GLFW_WINDOW) == GL_TRUE)
    {
      // Build texture
      GLuint texid;
      byte pixels[256 * 256 * 4];
      for (int i = 0; i < 256; ++i)
      {
        for (int j = 0; j < 256; ++j)
        {
          pixels[4*(i*256 + j)    ] = static_cast<byte>( 3 * (i + j));
          pixels[4*(i*256 + j) + 1] = static_cast<byte>(17 * (2 * i + j));
          pixels[4*(i*256 + j) + 2] = static_cast<byte>(23 * (3 * i + 251 * j));
          pixels[4*(i*256 + j) + 3] = static_cast<byte>(0xFF);
        }
      }
      glGenTextures(1, &texid);
      glBindTexture(GL_TEXTURE_2D, texid);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256,
          0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

      // Init sound
      ALCcontext *context;
      ALCdevice *device;
      ALuint sndsource;
      ALuint sndbuffer;
      device = alcOpenDevice(NULL);
      if (device)
      {
        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
        alGenSources(1, &sndsource);
        alGetError();

        // Build sound effect & play
        byte snddata[128 * 1024];
        for (int i = 0; i < 128 * 1024; ++i)
          snddata[i] = static_cast<byte>(rand());
        alGenBuffers(1, &sndbuffer);
        alBufferData(sndbuffer, AL_FORMAT_STEREO8, snddata, 128 * 1024, 44100);
        alSourcei(sndsource, AL_BUFFER, sndbuffer);
        alSourcei(sndsource, AL_LOOPING, AL_TRUE);
        alSourcePlay(sndsource);
      }

      // Set up rendering
      glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);  // Sets up clipping
      glClearColor(0.0f, 0.1f, 0.3f, 0.0f);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0, 1.0);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // Go!
      float angle = 0.0f;

      while (glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
      {
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texid);
        
        glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, 1.0f);
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2f(-0.9f, -0.9f);
        glTexCoord2d(1.0, 0.0); glVertex2f( 0.9f, -0.9f);
        glTexCoord2d(1.0, 1.0); glVertex2f( 0.9f,  0.9f);
        glTexCoord2d(0.0, 1.0); glVertex2f(-0.9f,  0.9f);
        glEnd();
        glPopMatrix();

        angle += 1.0f;

        glfwSwapBuffers();
      }

      // End all
      glDeleteTextures(1, &texid);
      alDeleteSources(1, &sndsource);
      alDeleteBuffers(1, &sndbuffer);
      context = alcGetCurrentContext();
      device = alcGetContextsDevice(context);
      alcMakeContextCurrent(NULL);
      alcDestroyContext(context);
      alcCloseDevice(device);

      glfwCloseWindow();
    }
    glfwTerminate();
  }

  return 0;
}
