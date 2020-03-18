#include "SandboxLayer.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <unistd.h>

#include "stb_image.h"

using namespace GLCore;
using namespace GLCore::Utils;

//////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texcoords;
    glm::vec1 TextureID;    // Not really using this ....  
};

//static glm::vec3 TransPoint(glm::vec3 vector, float tx, float ty)
//{
//    glm::vec4 vec(vector, 1.0f);
//    glm::mat4 trans = glm::mat4(1.0f);
//    trans = glm::translate(trans, glm::vec3(tx, ty, 0.0f));
//    glm::vec3 result = glm::vec3(trans * vec);
//    return result;
//}

static glm::vec3 RotatePoint(glm::vec3 vector, float Degrees, float tx, float ty)
{
    // Combine scale and rotate matrix
    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 scale = glm::scale(identity, glm::vec3(0.5f, 0.5f, 0.0f));
    identity = glm::rotate(identity, glm::radians(Degrees), glm::vec3(0.0f, 0.0f, 1.0f));
    identity = scale * identity;
    // Perform th scale and rotate...
    glm::vec3 result = glm::vec3(identity * glm::vec4(vector, 1.0f));
    // Translate the sclaed and rotated obj to world position.
    glm::vec4 vec(result, 1.0f);    
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(tx, ty, 0.0f));
    result = glm::vec3(trans * vec);
    return result;
}


static std::array< Vertex, 4> ScaleRotateTranslate(std::array< Vertex, 4> q, float Degrees, float tx, float ty)
{
    auto q0 = q;
    q0[0].position = RotatePoint(q0[0].position, Degrees, tx, ty);
    q0[1].position = RotatePoint(q0[1].position, Degrees, tx, ty);
    q0[2].position = RotatePoint(q0[2].position, Degrees, tx, ty);
    q0[3].position = RotatePoint(q0[3].position, Degrees, tx, ty);
    return q0;
} 


static void setFrame(std::array< Vertex, 4>& q0, glm::vec3 texture)
{
    q0[0].texcoords = glm::vec2(texture.x, 0);
    q0[1].texcoords = glm::vec2(texture.x + texture.z, 0);
    q0[2].texcoords = glm::vec2(texture.x + texture.z, 1);
    q0[3].texcoords = glm::vec2(texture.x, 1);
}
static std::array< Vertex, 4> CreateQuad(float x, float y, float size)
{


    Vertex v0 = {glm::vec3(x, y, 0.0f),           
                {glm::vec4(0.9f, 0.0f, 0.0f, 1.0f)}, 
                {glm::vec2(0.0f, 0.0f)},
                {glm::vec1(0.0f)}
                };

    Vertex v1 = {glm::vec3(x+size, y, 0.0f),      
                {glm::vec4(0.0f, 0.9f, 0.0f, 1.0f)}, 
                {glm::vec2(1.0f, 0.0f)}, 
                {glm::vec1(0.0f)}
                };

    Vertex v2 = {glm::vec3(x+size, y+size, 0.0f), 
                {glm::vec4(0.0f, 0.0f, 0.9f, 1.0f)}, 
                {glm::vec2(1.0f, 1.0f)},
                {glm::vec1(0.0f)}
                };

    Vertex v3 = {glm::vec3(x, y+size, 0.0f),      
                {glm::vec4(0.5f, 0.5f, 0.0f, 1.0f)}, 
                {glm::vec2(0.0f, 1.0f)},
                {glm::vec1(0.0f)}
                };
    return {v0,v1,v2,v3};
}

uint32_t indices[] = { 
     0,  1,  2,  2,  3,  0,
     4,  5,  6,  6,  7,  4, 
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
};

static GLuint LoadTexture(const std::string& path)
{
    int t_Width, t_Height, t_BPP;
    unsigned char* t_LocalBuffer;
    GLuint t_TextureID;

	stbi_set_flip_vertically_on_load(1);
	t_LocalBuffer = stbi_load(path.c_str(), &t_Width, &t_Height, &t_BPP, STBI_rgb_alpha);

    if (t_LocalBuffer == NULL) {
        std::cout << "Cannot load texture" << '\n';
        std::cout << stbi_failure_reason() << '\n'; 
        exit(-1);
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &t_TextureID);	
	glBindTexture(GL_TEXTURE_2D, t_TextureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, t_TextureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t_Width, t_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, t_LocalBuffer);
	
    std::cout << t_Width << 'x' << t_Height << '\n';

    if(t_LocalBuffer)
		stbi_image_free(t_LocalBuffer);

    return t_TextureID;
}

/////////////////////////////////////////////////////////////////////////////////////////////


SandboxLayer::SandboxLayer()
	: m_CameraController(16.0f / 9.0f)
{
    stbi_set_flip_vertically_on_load(1);
    char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) 
       std::cout << cwd << '\n';
}

SandboxLayer::~SandboxLayer()
{
}
// This is crap, becasue I used arrays they cannot be updated.
// If we used other Vec2 Vec3 Vec4 with indiviual variables and 
// not array then we could:  v0.position = {-0.531250, -0.625000, 0.0f};
// more readable that below!

void SandboxLayer::OnAttach()
{
    GLint n;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);
    std::cout << "Max Vetext attributes: " << n << '\n';
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &n);
    std::cout << "Max Texture Units    : " << n << '\n';

	EnableGLDebugging();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);

	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, position));
 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, color));
 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, texcoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, TextureID));

    // This buffer would need to be updated we have a possibility of 1000 Vertices so we really need
    // this buffer to be capable of doing that.  It is left at four to try the dynamic buffer...
    // See TheCherno video: https://youtu.be/5df3NvQNzUs?t=720

	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    m_TextureID = LoadTexture("assets/textures/exp3.jpg"); 
}

void SandboxLayer::OnDetach()
{
	glDeleteVertexArrays(1, &m_QuadVA);
	glDeleteBuffers(1, &m_QuadVB);
	glDeleteBuffers(1, &m_QuadIB);
}


void SandboxLayer::OnEvent(Event& event)
{
	m_CameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressedEvent>(
		[&](MouseButtonPressedEvent& e)
		{
			m_SquareColor = m_SquareAlternateColor;
			return false;
		});
	dispatcher.Dispatch<MouseButtonReleasedEvent>(
		[&](MouseButtonReleasedEvent& e)
		{
			m_SquareColor = m_SquareBaseColor;
			return false;
		});
    // Handle window resizing events.    
    dispatcher.Dispatch<WindowResizeEvent>(
        [&](WindowResizeEvent& e)
        {
            glViewport(0,0, e.GetWidth(), e.GetHeight());
            return false;
        });
}

void SandboxLayer::OnUpdate(Timestep ts)
{
    static int frameCount = 0;

    static float Degrees1 = 0.0f;
    static float Degrees2 = 359.0f;
    static float xMove = -1.5625;

    static int SpriteIndex = 0;
    

    frameCount++;
    if(frameCount % 3 == 0)
        ++SpriteIndex &= 15;

	m_CameraController.OnUpdate(ts);

    // Set dynamic buffer...
    #define Squares 6
    
    Vertex vertices[Squares * 4];

    auto q0 = CreateQuad(-0.5f, -0.5f, 1.0f);
    auto q1 = ScaleRotateTranslate(q0, Degrees1,-0.2813*1.5, -0.275*1.5);
    setFrame(q1, glm::vec3((SpriteIndex * 0.0625f), 1.0f, 0.0625f));

    memcpy(vertices, q1.data(), q1.size() * sizeof(Vertex));

    q1 = ScaleRotateTranslate(q0, Degrees1,0.28125*1.5, -0.275*1.5);
    setFrame(q1, glm::vec3((SpriteIndex * 0.0625f), 1.0f, 0.0625f));
    memcpy(vertices + (q1.size() * 1), q1.data(), q1.size() * sizeof(Vertex));

    q1 = ScaleRotateTranslate(q0, Degrees1,0.28125*1.5, 0.2875*1.5);
    setFrame(q1, glm::vec3((SpriteIndex * 0.0625f), 1.0f, 0.0625f));
    memcpy(vertices + (q1.size() * 2), q1.data(), q1.size() * sizeof(Vertex));

    q1 = ScaleRotateTranslate(q0, Degrees1,-0.2813*1.5, 0.2875*1.5);
    setFrame(q1, glm::vec3((SpriteIndex * 0.0625f), 1.0f, 0.0625f));
    memcpy(vertices + (q1.size() * 3), q1.data(), q1.size() * sizeof(Vertex));

    q1 = ScaleRotateTranslate(q0, Degrees2, 0.0f, 0.0f);
    setFrame(q1, glm::vec3((SpriteIndex * 0.0625f), 1.0f, 0.0625f));
    memcpy(vertices + (q1.size() * 4), q1.data(), q1.size() * sizeof(Vertex));

    q1 = ScaleRotateTranslate(q0, 0.0f, xMove, -0.75f);
    setFrame(q1, glm::vec3((SpriteIndex * 0.0625f), 1.0f, 0.0625f));
    memcpy(vertices + (q1.size() * 5), q1.data(), q1.size() * sizeof(Vertex));

    Degrees1 += 1.0f;
    if(Degrees1 >= 359.0f)
        Degrees1 = 0.0f;

    Degrees2 -= 2.0f;
    if(Degrees2 <= 0.0f)
        Degrees2 = 359.0f;

    xMove += 0.01f;
    if(xMove > 2.25)
        xMove = -2.0;

	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());

    glBindTextureUnit(0, m_TextureID);

    int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));

	location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");
	glUniform4fv(location, 1, glm::value_ptr(m_SquareColor));

	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, nullptr);
}

void SandboxLayer::OnImGuiRender()
{
    ImGui::Begin("Controls");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);       
	ImGui::End();
 }
