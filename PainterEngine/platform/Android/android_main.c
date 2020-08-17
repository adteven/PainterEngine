//PainterEngine framework
#include "PainterEngine_Application.h"


//
#include "jni.h"
#include "errno.h"
//opengl es
#include "EGL/egl.h"
#include "GLES3/gl3.h"


//Android Native activity support
#include "android_native_app_glue.h"

//timer
#include "time.h"
#include "../../kernel/PX_Object.h"


static const char PX_AndroidEngine_GL_VS[]="attribute vec4 vPosition;attribute vec4 vTex;varying vec2 textureCoordinate;void main(){gl_Position = vPosition;textureCoordinate = vTex.xy;}";
static const char PX_AndroidEngine_GL_PS[]="precision mediump float;varying highp vec2 textureCoordinate;uniform sampler2D tex;void main(){gl_FragColor = texture2D(tex, textureCoordinate);}";

static const GLfloat PX_AndroidEngine_squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
};

static const GLfloat PX_AndroidEngine_texturePosition[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
};

typedef  struct
{
    struct android_app *pAndroidApp;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    GLuint vertexShader;
    GLuint pixelsShader;
    GLuint programObject;
    GLuint vPosition;
    GLuint vTex;
    GLuint renderTexture;
    int32_t width;
    int32_t height;

    px_bool PainterEngineReady;
}PX_AndroidEngine;

static PX_AndroidEngine engine;


unsigned int timeGetTime()
{
    unsigned int uptime = 0;
    struct timespec on;
    if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
        uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
    return uptime;
}

static GLuint PX_AndroidEngine_LoadShader(GLenum type,const px_char *shaderSources)
{
    GLuint shader;
    GLint compiled;

    shader=glCreateShader(type);
    if(shader==0) return  PX_FALSE;

    glShaderSource(shader,1,&shaderSources,PX_NULL);
    glCompileShader(shader);

    glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);

    if(!compiled)
    {
      glDeleteShader(shader);
        return PX_FALSE;
    }
    return  shader;
}

static px_bool  PX_AndroidEngine_InitializeDisplay(PX_AndroidEngine *engine)
{
    const int attrib_list[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;
    EGLConfig ConfigsDesc[64];
    px_int i;
    GLint linked;
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);



    eglInitialize(display, 0, 0);
    eglChooseConfig(display, attribs, PX_NULL,0, &numConfigs);
    if(numConfigs>PX_COUNTOF(ConfigsDesc))
    {
        numConfigs=PX_COUNTOF(ConfigsDesc);
    }
    eglChooseConfig(display, attribs,ConfigsDesc, numConfigs, &numConfigs);

    for (i=0; i < numConfigs; i++)
    {
        EGLint r, g, b, d, a;
        if (eglGetConfigAttrib(display, ConfigsDesc[i], EGL_RED_SIZE, &r)   &&
            eglGetConfigAttrib(display, ConfigsDesc[i], EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, ConfigsDesc[i], EGL_BLUE_SIZE, &b)  &&
            eglGetConfigAttrib(display, ConfigsDesc[i], EGL_ALPHA_SIZE, &a)&&
            eglGetConfigAttrib(display, ConfigsDesc[i], EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 &&a == 8&&d==0 )
        {
            config = ConfigsDesc[i];
            break;
        }
    }

    if (i == numConfigs) {
        config = ConfigsDesc[0];
    }

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, engine->pAndroidApp->window, NULL);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        return PX_FALSE;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    // Initialize GL state.
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE0);
    glEnable(GL_TEXTURE1);
    glDisable(GL_DEPTH_TEST);

    //Initialize GL shader
    engine->vertexShader=PX_AndroidEngine_LoadShader(GL_VERTEX_SHADER,PX_AndroidEngine_GL_VS);
    if(engine->vertexShader==0)
        return  PX_FALSE;
    engine->pixelsShader=PX_AndroidEngine_LoadShader(GL_FRAGMENT_SHADER,PX_AndroidEngine_GL_PS);
    if(engine->pixelsShader==0)
        return PX_FALSE;

    engine->programObject=glCreateProgram();
    if(engine->programObject==0)
        return PX_FALSE;

    glAttachShader(engine->programObject,engine->vertexShader);
    glAttachShader(engine->programObject,engine->pixelsShader);

    glLinkProgram(engine->programObject);

    glGetProgramiv(engine->programObject,GL_LINK_STATUS,&linked);
    if(!linked)
    {
        glDeleteProgram(engine->programObject);
        return PX_FALSE;
    }

    glGenTextures(1,&engine->renderTexture);

    engine->vPosition=glGetAttribLocation(engine->programObject,"vPosition");
    engine->vTex=glGetAttribLocation(engine->programObject,"vTex");

    glViewport(0,0,engine->width,engine->height);

//PainterEngine Initialize
    if(!engine->PainterEngineReady)
    {
        if(!PX_ApplicationInitialize(&App,w,h)) return PX_FALSE;
    }
    else
    {
        PX_Object_Event e;
        e.Event=PX_OBJECT_EVENT_WINDOWRESIZE;
        PX_Object_Event_SetWidth(&e,w);
        PX_Object_Event_SetHeight(&e,h);
        PX_ApplicationPostEvent(&App,e);
    }
    engine->PainterEngineReady=PX_TRUE;
    return PX_TRUE;
}

static px_bool  PX_AndroidEngine_FreeDisplay(PX_AndroidEngine *engine)
{
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }

    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;

    glDetachShader(engine->programObject,engine->vertexShader);
    glDetachShader(engine->programObject,engine->pixelsShader);
    glDeleteShader(engine->pixelsShader);
    glDeleteShader(engine->vertexShader);
    glDeleteProgram(engine->programObject);

    return  PX_TRUE;
}

static void PX_AndroidEngine_Render(PX_AndroidEngine* engine) {
    if (engine->display == NULL)
    {
        // No display.
        return;
    }

    //vertex data


    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(engine->programObject);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,engine->renderTexture);
    glTexImage2D(GL_TEXTURE_2D,\
    0,\
    GL_RGBA,\
    App.runtime.RenderSurface.width,\
    App.runtime.RenderSurface.height,\
    0,\
    GL_RGBA,\
    GL_UNSIGNED_BYTE,\
    App.runtime.RenderSurface.surfaceBuffer\
    );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glUniform1i(glGetAttribLocation(engine->programObject,"tex"), 0);

    glVertexAttribPointer(engine->vPosition, 2, GL_FLOAT, GL_FALSE, 0,PX_AndroidEngine_squareVertices);
    glEnableVertexAttribArray(engine->vPosition);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glVertexAttribPointer(engine->vTex, 2, GL_FLOAT, 0, 0, PX_AndroidEngine_texturePosition);
    glEnableVertexAttribArray(engine->vTex);

    eglSwapBuffers(engine->display, engine->surface);
}

static px_int PX_AndroidEngine_Input(struct android_app* app, AInputEvent* event) {
    PX_AndroidEngine* engine = (PX_AndroidEngine*)app->userData;
    px_int32 EventType=AInputEvent_getType(event);
    px_int32 EventCount=AMotionEvent_getPointerCount(event);
    px_int32 action=AMotionEvent_getAction(event);
    px_float cursorx_scale,cursory_scale;
    px_int i;
    px_float x,y;
    PX_Object_Event e;
    if(!engine->PainterEngineReady) return  0;
    cursorx_scale=App.runtime.surface_width*1.0f/App.runtime.window_width;
    cursory_scale=App.runtime.surface_height*1.0f/App.runtime.window_height;
    switch (action&AMOTION_EVENT_ACTION_MASK)
    {
        case AMOTION_EVENT_ACTION_MOVE:
        {
            for (i = 0; i < EventCount; ++i)
            {
                x=AMotionEvent_getX(event, i);
                y=AMotionEvent_getY(event, i);
                e.Event=PX_OBJECT_EVENT_CURSORDRAG;
                PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
                PX_Object_Event_SetCursorY(&e,y*cursory_scale);
                PX_Object_Event_SetCursorIndex(&e,(action&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>8);
                PX_ApplicationPostEvent(&App,e);
            }
        }
        break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        case AMOTION_EVENT_ACTION_DOWN:
        {
            for (i = 0; i < EventCount; ++i)
            {
                x=AMotionEvent_getX(event, i);
                y=AMotionEvent_getY(event, i);
                e.Event=PX_OBJECT_EVENT_CURSORDOWN;
                PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
                PX_Object_Event_SetCursorY(&e,y*cursory_scale);
                PX_Object_Event_SetCursorIndex(&e,(action&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>8);
                PX_ApplicationPostEvent(&App,e);
            }
        }
        break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_UP:
        {
            for (i = 0; i < EventCount; ++i)
            {
                x=AMotionEvent_getX(event, i);
                y=AMotionEvent_getY(event, i);
                e.Event=PX_OBJECT_EVENT_CURSORUP;
                PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
                PX_Object_Event_SetCursorY(&e,y*cursory_scale);
                PX_Object_Event_SetCursorIndex(&e,(action&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>8);
                PX_ApplicationPostEvent(&App,e);
            }
        }
        break;
    }

    return 0;
}

void PX_AndroidEngine_CMD(struct android_app* app, int32_t cmd)
{
    PX_AndroidEngine* engine = (PX_AndroidEngine*)app->userData;
    switch (cmd)
        {
            case APP_CMD_SAVE_STATE:
                break;
            case APP_CMD_INIT_WINDOW:
                if (engine->pAndroidApp->window != NULL)
                    PX_AndroidEngine_InitializeDisplay(engine);
                break;
            case APP_CMD_TERM_WINDOW:
                PX_AndroidEngine_FreeDisplay(engine);
                break;
            case APP_CMD_GAINED_FOCUS:
                break;
            case APP_CMD_LOST_FOCUS:
                break;
        }
}


ANativeActivity* PX_nativeActivity=PX_NULL;
AAssetManager* PX_assetManager=PX_NULL;

void android_main(struct android_app *pApp)
{

    px_dword lastUpdateTime,elpased;
    int events;
    struct android_poll_source *pSource;
    pApp->userData=&engine;
    pApp->onAppCmd = PX_AndroidEngine_CMD;
    pApp->onInputEvent = PX_AndroidEngine_Input;
    engine.pAndroidApp=pApp;
    PX_nativeActivity = pApp->activity;
    PX_assetManager = PX_nativeActivity->assetManager;


    lastUpdateTime=timeGetTime();
    do
    {
        while (ALooper_pollAll(0, PX_NULL, &events, (void **) &pSource) >= 0)
        {
            if (pSource)
            {
                pSource->process(pApp, pSource);
                if (pApp->destroyRequested != 0) {
                    PX_AndroidEngine_FreeDisplay(&engine);
                    return;
                }
            }
        }
        if(engine.PainterEngineReady)
        {
            elpased=timeGetTime()-lastUpdateTime;
            lastUpdateTime=timeGetTime();
            PX_ApplicationUpdate(&App,elpased);
            PX_ApplicationRender(&App,elpased);
        }

        PX_AndroidEngine_Render(&engine);
    } while (!pApp->destroyRequested);
}
