#include "renderer.h"
#include "log.h"
#include <linmath.h/linmath.h>
#include "event/event.h"

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{// vertices array in the range [−1, 1] normalized-device coordinates
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// Callback function for printing debug statements
static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    Log::error("%d: %s of %s severity, raised from %s: %s\n",
        id, _type.c_str(), _severity.c_str(), _source.c_str(), msg);
}

namespace Engine {

    Renderer::Renderer() {
        Log::setLogLevel(Log::Level::INFO);
    }

    void Renderer::onEvent(Event& e) {
        //Log::info ( "event" );
        if (e.GetEventType() == EventType::WindowResize)
        {
            Log::info(e.ToString());
        }
    }

    int Renderer::init(const std::string& title, bool fullscreen) {

        m_window->setEventCallback(std::bind(&Renderer::onEvent, this, std::placeholders::_1));

        // Load all OpenGL functions using the glfw loader function
        // If you use SDL you can use: https://wiki.libsdl.org/SDL_GL_GetProcAddress
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Log::error("Failed to initialize OpenGL context");
            return EXIT_FAILURE;
        }
        // The following function calls should be made directly after OpenGL
        // initialization.
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugMessageCallback, NULL);


        // Successfully loaded OpenGL
        Log::debug("Loaded OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

        //glfwSetKeyCallback(m_window->w, key_callback);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        /*Returns n currently unused names for buffer objects in the array buffers.
        The names returned in buffers do not have to be a contiguous set of
        integers.*/
        glGenBuffers(1, &m_vertex_buffer);
        /*glBindBuffer() does three things : 1. When using buffer of an unsigned
        integer other than zero for the first time, a new buffer object is created
        and assigned that name. 2. When binding to a previously created buffer
        object, that buffer object becomes the active buffer object. 3. When
        binding to a buffer value of zero, OpenGL stops using buffer objects for
        that target.*/
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        /*Allocates size storage units (usually bytes) of OpenGL server memory for
        storing data or indices. Any previous data associated with the currently
        bound object will be deleted.
        usage Provides a hint as to how the data will be readand written after allocation*/
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(m_vertex_shader, 1, &vertex_shader_text, NULL);
        glCompileShader(m_vertex_shader);

        m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_fragment_shader, 1, &fragment_shader_text, NULL);
        glCompileShader(m_fragment_shader);

        m_program = glCreateProgram();
        glAttachShader(m_program, m_vertex_shader);
        glAttachShader(m_program, m_fragment_shader);
        glLinkProgram(m_program);

        m_mvp_location = glGetUniformLocation(m_program, "MVP");
        m_vpos_location = glGetAttribLocation(m_program, "vPos");
        m_vcol_location = glGetAttribLocation(m_program, "vCol");

        glEnableVertexAttribArray(m_vpos_location);
        glVertexAttribPointer(m_vpos_location, 2, GL_FLOAT, GL_FALSE,
            sizeof(vertices[0]), (void*)0);
        glEnableVertexAttribArray(m_vcol_location);
        glVertexAttribPointer(m_vcol_location, 3, GL_FLOAT, GL_FALSE,
            sizeof(vertices[0]), (void*)(sizeof(float) * 2));
    }

    int Renderer::runRenderLoop() {

        //while (!m_window->isClosed())
        //{
            float ratio;
            mat4x4 m, p, mvp;

            int width = m_window->getWidth();
            int height = m_window->getHeight();
            ratio = width / (float)height;

            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT);

            mat4x4_identity(m);
            mat4x4_rotate_Z(m, m, (float)glfwGetTime());
            mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            mat4x4_mul(mvp, p, m);

            glUseProgram(m_program);
            glUniformMatrix4fv(m_mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            m_window->onUpdate();

        //}

        return EXIT_SUCCESS;
    }

}