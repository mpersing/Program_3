#ifndef __RENDERENGINE
#define __RENDERENGINE

#include "GLHelper.h"
#include "WorldState.h"
#include "glm/gtc/matrix_transform.hpp"

class RenderEngine
{
public:
	RenderEngine()
	{
		initialized = false;
	}
	
	~RenderEngine()
	{
		if(initialized)
		{
			glDeleteVertexArrays(1, &clockArray);
		}
	}
	
	void display(WorldState const & world)
	{
		if(!initialized)
			init(world.getModel());
		Model const & model = world.getModel();
		float time = world.getCurrentTime();
		
		//clear the old frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		checkGLError("clear");
		
		//use shader
		glUseProgram(clockProg);
		
		//TODO upload transformation matrices to control the clock
		//to build the clock:
		//create the vertices in Model, display here
		//use the tick object 12 times with different rotations
		//use a transform to rotate the large hand clockwise
		//use a transform to rotate the small hand clockwise at 1/12 the large hand rate
		
		//TODO handle the qwer transforms
		//apply the z rotation before the y rotation
		//figure out the rest of the transform order
		
		glUniform1f(timeSlot, world.getCurrentTime());
		checkGLError("uniform");
		
		//draw model
		glBindVertexArray(clockArray);
		
		glDrawArrays(GL_LINE_LOOP, model.getObjectStart(0), model.getObjectSize(0));

		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0, 1, 1));
		glUniformMatrix4fv(transformSlot, 1, GL_FALSE, &rot[0][0]);
		glDrawArrays(GL_LINES, model.getObjectStart(1), model.getObjectSize(1));

		glm::mat4 ident = glm::mat4(1.0f);
		glUniformMatrix4fv(transformSlot, 1, GL_FALSE, &ident[0][0]);
		glDrawArrays(GL_LINES, model.getObjectStart(2), model.getObjectSize(2));

		glm::mat4 funny = glm::translate(glm::mat4(1.0f), glm::vec3(time * 0.1, time * 0.1, 0));
		funny = funny * rot;
		glUniformMatrix4fv(transformSlot, 1, GL_FALSE, &funny[0][0]);
		glDrawArrays(GL_LINES, model.getObjectStart(3), model.getObjectSize(3));
		checkGLError("draw");
		
		//cleanup
		glBindVertexArray(0);
		glUseProgram(0);
		checkGLError("render");
	}
	
private:
	bool initialized;
	
	GLuint clockProg;
	GLuint clockArray;
	
	GLint timeSlot;
	GLint transformSlot;
	
	void setupShader()
	{
		char const * vertPath = "Shaders/simple.vert";
		char const * redPath = "Shaders/simple.frag";
		
		clockProg = ShaderManager::shaderFromFile(&vertPath, &redPath, 1, 1);
		
		checkGLError("shader");
	}
	
	void setupBuffers(Model const & model)
	{
		
		GLint positionSlot;
		GLint colorSlot;
		GLuint positionBuffer;
		GLuint colorBuffer;
		
		positionSlot = glGetAttribLocation(clockProg, "pos");
		colorSlot =    glGetAttribLocation(clockProg, "color");
		timeSlot =     glGetUniformLocation(clockProg, "time");
		transformSlot = glGetUniformLocation(clockProg, "T");
		
		glGenVertexArrays(1, &clockArray);
		glBindVertexArray(clockArray);

		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), model.getPositions(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Do the same thing for the color data
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.getColorBytes(), model.getColors(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(colorSlot);
		glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindVertexArray(0);
		
		checkGLError("setup");
	}
	
	float initLoader()
	{
		float ver = 0.0f;
#ifdef GLEW
		glewExperimental = GL_TRUE;
		
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
		
		if (GLEW_VERSION_1_1) { ver = 1.1f; }
		if (GLEW_VERSION_1_2) { ver = 1.2f; }
		if (GLEW_VERSION_1_3) { ver = 1.3f; }
		if (GLEW_VERSION_1_4) { ver = 1.4f; }
		if (GLEW_VERSION_1_5) { ver = 1.5f; }
		
		if (GLEW_VERSION_2_0) { ver = 2.0f; }
		if (GLEW_VERSION_2_1) { ver = 2.1f; }
		
		if (GLEW_VERSION_3_0) { ver = 3.0f; }
		if (GLEW_VERSION_3_1) { ver = 3.1f; }
		if (GLEW_VERSION_3_2) { ver = 3.2f; }
		if (GLEW_VERSION_3_3) { ver = 3.3f; }
		
		if (GLEW_VERSION_4_0) { ver = 4.0f; }
		if (GLEW_VERSION_4_1) { ver = 4.1f; }
		if (GLEW_VERSION_4_2) { ver = 4.2f; }
		if (GLEW_VERSION_4_3) { ver = 4.3f; }
		if (GLEW_VERSION_4_4) { ver = 4.4f; }
		if (GLEW_VERSION_4_5) { ver = 4.5f; }
#endif
		
#ifdef GL3W
		if (gl3wInit()) {
			fprintf(stderr, "failed to initialize OpenGL\n");
		}
		
		if (gl3wIsSupported(1, 1)) { ver = 1.1f; }
		if (gl3wIsSupported(1, 2)) { ver = 1.2f; }
		if (gl3wIsSupported(1, 3)) { ver = 1.3f; }
		if (gl3wIsSupported(1, 4)) { ver = 1.4f; }
		if (gl3wIsSupported(1, 5)) { ver = 1.5f; }
		
		if (gl3wIsSupported(2, 0)) { ver = 2.0f; }
		if (gl3wIsSupported(2, 1)) { ver = 2.1f; }
		
		if (gl3wIsSupported(3, 0)) { ver = 3.0f; }
		if (gl3wIsSupported(3, 1)) { ver = 3.1f; }
		if (gl3wIsSupported(3, 2)) { ver = 3.2f; }
		if (gl3wIsSupported(3, 3)) { ver = 3.3f; }
		
		if (gl3wIsSupported(4, 0)) { ver = 4.0f; }
		if (gl3wIsSupported(4, 1)) { ver = 4.1f; }
		if (gl3wIsSupported(4, 2)) { ver = 4.2f; }
		if (gl3wIsSupported(4, 3)) { ver = 4.3f; }
		if (gl3wIsSupported(4, 4)) { ver = 4.4f; }
		if (gl3wIsSupported(4, 5)) { ver = 4.5f; }
#endif
		
		return ver;
	}
	
	void init(Model const & model)
	{
		initialized = true;
		
		float ver = initLoader();
		if( ver < 1.0f ) {
			printf("OpenGL is not supported.\n");
			exit(1);
		}
		printf("OpenGL version %.1f is supported.\n", ver);
		
		//setup all shader parts
		setupShader();
		
		//setup buffers
		setupBuffers(model);
	}
};

#endif