#include "SandboxLayer.h"
#include <glm/glm.hpp>

using namespace GLCore;
using namespace GLCore::Utils;


struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};


SandboxLayer::SandboxLayer()
	: m_CameraController(16.0f / 9.0f)
{
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnAttach()
{
    GLint n;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);
    std::cout << "Max Vetext attributes: " << n << '\n';

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
    std::cout << "sizeof(Vertex): " << sizeof(Vertex) << " offsetof(Vertex, position): " << offsetof(Vertex, position) << '\n';

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, color));
    std::cout << "sizeof(Vertex): " << sizeof(Vertex) << " offsetof(Vertex, color): " << offsetof(Vertex, color) << '\n';
	
    // This buffer would need to be updated we have a possibility of 1000 Vertices so we really need
    // this buffer to be capable of doing that.  It is left at four to try the dynamic buffer...
    // See TheCherno video: https://youtu.be/5df3NvQNzUs?t=720

    uint32_t indices[] = { 0,  1,  2,  2,  3,  0,
                           4,  5,  6,  6,  7,  4, 
                           8,  9, 10, 10, 11,  8,
                          12, 13, 14, 14, 15, 12};

	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
}
static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
    int loc = glGetUniformLocation(shader, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}


// This is crap, becasue I used arrays they cannot be updated.
// If we used other Vec2 Vec3 Vec4 with indiviual variables and 
// not array then we could:  v0.position = {-0.531250, -0.625000, 0.0f};
// more readable that below!
static std::array< Vertex, 4> CreateQuad(float x, float y)
{
    float size = 0.5f;

    Vertex v0 = {glm::vec3(x, y, 0.0f), {glm::vec4(0.9f, 0.0f, 0.0f, 1.0f)}};
    Vertex v1 = {glm::vec3(x+size, y, 0.0f), {glm::vec4(0.0f, 0.9f, 0.0f, 1.0f)}};
    Vertex v2 = {glm::vec3(x+size, y+size, 0.0f), {glm::vec4(0.0f, 0.0f, 0.9f, 1.0f)}};
    Vertex v3 = {glm::vec3(x, y+size, 0.0f), {glm::vec4(0.5f, 0.5f, 0.0f, 1.0f)}};
    // v0.position[0] += .0125;
    return {v0,v1,v2,v3};
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

    // Set dynamic buffer...

    auto q0 = CreateQuad(-0.531250, -0.625000);
    Vertex vertices[4 * 4];
    memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex));

    q0 = CreateQuad(0.031250, -0.625000);
    memcpy(vertices + q0.size(), q0.data(), q0.size() * sizeof(Vertex));

    q0 = CreateQuad(0.031250, -0.062500);
    memcpy(vertices + (q0.size() * 2), q0.data(), q0.size() * sizeof(Vertex));

    q0 = CreateQuad(-0.531250, -0.062500);
    memcpy(vertices + (q0.size() * 3), q0.data(), q0.size() * sizeof(Vertex));
    
    //

	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());

    int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));

	location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");
	glUniform4fv(location, 1, glm::value_ptr(m_SquareColor));

	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, nullptr);
}

void SandboxLayer::OnImGuiRender()
{
    ImGui::Begin("Controls");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);       
	ImGui::End();
 }
