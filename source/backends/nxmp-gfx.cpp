#include "nxmp-gfx.h"

#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
//#include "utils.h"
#include <switch.h>

#include "stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

using namespace glm;

namespace nxmpgfx{
	
	
	float CurrentVolume = 0.0f;
	
	ImVec4 Text_color = ImVec4(255, 255, 255, 1.00f);
	ImVec4 Active_color = ImVec4(0, 255, 203, 1.00f);
	ImVec4 Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
	ImVec4 Window_Bg_color = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	ImVec4 OptsTab_Bg_color = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	ImVec4 Popup_Bg_color = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	ImVec4 NavHover_color = ImVec4(0.0f, 0.0f, 0.0f, 0.20f);
	ImVec4 HeaderHover_color = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	ImVec4 Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
	ImVec4 ButtonActive_color = ImVec4(0, 255, 203, 0.30f);
	
		
	const char* glsl_version = "#version 430 core";
	GLFWwindow *window;

	GLuint WIDTH = 1280;
	GLuint HEIGHT = 720;
	
	GLuint mpv_fbo;
	GLuint mpv_fbotexture;	
	mpv_opengl_fbo fbo;
	mpv_render_param fbo_params[3];
	
	GLuint prog;
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
	
	
	GLuint VBO_Progress;
	GLuint VAO_Progress;
	
	bool docked = false;
	
	bool splashloaded = false;
	
	bool B_A_PRESS = false;
	bool B_B_PRESS = false;
	bool B_X_PRESS = false;
	bool B_Y_PRESS = false;
	bool B_DU_PRESS = false;
	bool B_DD_PRESS = false;
	bool B_DL_PRESS = false;
	bool B_DR_PRESS = false;

	bool B_TL_PRESS = false;
	bool B_TR_PRESS = false;


	bool B_R_PRESS = false;
	bool B_L_PRESS = false;
	bool B_ZR_PRESS = false;
	bool B_ZL_PRESS = false;

	bool B_PLUS_PRESS = false;
	bool B_MINUS_PRESS = false;
	
	bool B_AX_L_UP_PRESS = false;
	bool B_AX_L_DOWN_PRESS = false;
	bool B_AX_L_LEFT_PRESS = false;
	bool B_AX_L_RIGHT_PRESS = false;
	

	
	typedef struct{
		bool pressed = false;
		std::chrono::time_point<std::chrono::system_clock> eventchrono;
	}axixstate_struct;
	
	axixstate_struct AX_R_UP;
	axixstate_struct AX_R_DOWN;
	axixstate_struct AX_R_LEFT;
	axixstate_struct AX_R_RIGHT;
	
	
	static void errorCallback(int errorCode, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", errorCode, description);
	}
	
	inline uint64_t bit_set_to(uint64_t number, unsigned int n, bool x) {
		return (number & ~((unsigned int)1 << n)) | ((unsigned int)x << n);
	}
	
	inline uint64_t bit_set(uint64_t number, unsigned int n) {
		return number | ((unsigned int)1 << n);
	}


	bool GLTxtLoadFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height,bool flip = false){
		int image_width = 0;
		int image_height = 0;
		stbi_set_flip_vertically_on_load( flip? 1:0 );
		unsigned char* image_data = stbi_load(filename.c_str(), &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;
		
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    
		*out_texture = id;
		*out_width = image_width;
		*out_height = image_height;
		stbi_set_flip_vertically_on_load(0);
		return true;
	}
	
	void AttachShader( GLuint program, GLenum type, const char* src )
	{
		GLuint shader = glCreateShader( type );
		glShaderSource( shader, 1, &src, NULL );
		glCompileShader( shader );
		//CheckStatus( shader, true );
		glAttachShader( program, shader );
		glDeleteShader( shader );
	}
	
	const char* vert = 1 + R"GLSL(
	#version 330 core
	layout(location = 0) in vec3 aPos;
	layout(location = 1) in vec3 aColor;
	layout(location = 2) in vec2 aTexCoord;

	out vec3 ourColor;
	out vec2 TexCoord;

	void main() {
		gl_Position = vec4(aPos, 1.0);
		ourColor = aColor;
		TexCoord = aTexCoord;
	}
	)GLSL";

	const char* frag = 1 + R"GLSL(
	#version 330 core
	out vec4 FragColor;

	in vec3 ourColor;
	in vec2 TexCoord;

	uniform sampler2D ourTexture;

	void main() {
		FragColor = texture(ourTexture, TexCoord);
	}
	)GLSL";

	float vertices[] =
		{
			-1.0f, -1.0f, 0.0f,  1.0f,1.0f,0.0f,  0.0f,0.0f,
			 1.0f, -1.0f, 0.0f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
			-1.0f,  1.0f, 0.0f,  1.0f,1.0f,0.0f,  0.0f,1.0f
		};

		unsigned int indices[] =
		{
			0,1,2,
			2,3,0
		};

	unsigned int splashtexture;
	
	void loopAppletMode(){

		while (appletMainLoop())
		{
			glfwPollEvents();
			GLFWgamepadstate state;
			
			
			if ((GLFW_TRUE == glfwGetGamepadState(GLFW_JOYSTICK_1, &state)))
			{
				if(state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS){
					break;
				}
			}
			
		
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Render stuff
			glUseProgram( prog );
			unsigned int texture_unit = 0;
			glActiveTexture( GL_TEXTURE0 + texture_unit );
			glUniform1i( glGetUniformLocation( prog, "ourTexture" ), texture_unit );
			glBindTexture(GL_TEXTURE_2D, splashtexture);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			
			
			glfwSwapBuffers(window);
		}

	}
	
	
	void Init_Backend(bool isdocked,bool vsync){
		

		glfwSetErrorCallback(errorCallback);
		docked = isdocked;
		
		
		WIDTH = handheldWidth; HEIGHT = handheldHeight;
		if(docked){
			WIDTH = dockedWidth; HEIGHT = dockedHeight;
		}
		
		if (!glfwInit()){
			return;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		window = glfwCreateWindow(WIDTH, HEIGHT, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
		if (window == nullptr)
			return;
		glfwMakeContextCurrent(window);
		if(vsync){
			glfwSwapInterval(1);
		}else{
			glfwSwapInterval(0);
		}
		
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);



	}
	
	
	void Init_Backend_AppletMode(bool isdocked){

		docked = isdocked;
		prog = glCreateProgram();
		AttachShader( prog, GL_VERTEX_SHADER, vert );
		AttachShader( prog, GL_FRAGMENT_SHADER, frag );
		glLinkProgram( prog );
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		
		int image_width,image_height;
		
		GLTxtLoadFromFile("romfs:/appletmode.png",&splashtexture,&image_width,&image_height,true);
		
		if(docked){
			glViewport(0,0,1920,1080);
		
		}else{
			glViewport(0,0,1280,720);
		
		}

		
	}
	
	
	struct vertex_data{
		vec4 coord;
		vec2 uv;
	};
	
	
	
	void Init_Backend_Splash(bool isdocked){
		docked = isdocked;


		if(!splashloaded){
			prog = glCreateProgram();
			AttachShader( prog, GL_VERTEX_SHADER, vert );
			AttachShader( prog, GL_FRAGMENT_SHADER, frag );
			glLinkProgram( prog );
		
		
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
			
			
			

			int image_width,image_height;
			
			GLTxtLoadFromFile("romfs:/splash.png",&splashtexture,&image_width,&image_height,true);
			splashloaded=true;
		}
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		if(docked){
			glViewport(0,0,1920,1080);
		
		}else{
			glViewport(0,0,1280,720);
		
		}
        
		glUseProgram( prog );
		
		
		
        unsigned int texture_unit = 0;
        glActiveTexture( GL_TEXTURE0 + texture_unit );
        glUniform1i( glGetUniformLocation( prog, "ourTexture" ), texture_unit );
        glBindTexture(GL_TEXTURE_2D, splashtexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
		
		/*
		glEnable(GL_SCISSOR_TEST);
		glScissor(0.5, 0.5, 100.0, 100.0);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		*/
		
        glfwSwapBuffers(window);

	
	}
	
	void updateSplash(int perc){



		int barsize = 980;
		if(docked)barsize=1620;
		
		int wsize = perc*barsize/100;
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram( prog );
		
        unsigned int texture_unit = 0;
        glActiveTexture( GL_TEXTURE0 + texture_unit );
        glUniform1i( glGetUniformLocation( prog, "ourTexture" ), texture_unit );
        glBindTexture(GL_TEXTURE_2D, splashtexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
		
		glEnable(GL_SCISSOR_TEST);
		glScissor(150.0,40.0, wsize, 30.0);
		glClearColor(0.859f, 0.118f, 0.29f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		
        glfwSwapBuffers(window);
		
	}
	
	
	void Init_ImGui(bool isdocked){

		docked = isdocked;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void) io;
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		io.IniFilename = nullptr;

			
		ImGui::StyleColorsDark();
			
	

		//NXLOG::DEBUGLOG("Init IMGUI GLFW BACKEND\n");
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		//NXLOG::DEBUGLOG("Init IMGUI OPENGL BACKEND\n");
		ImGui_ImplOpenGL3_Init(glsl_version);
		



		if(docked)ImGui::GetIO().FontGlobalScale = 1.5f;
		if(!docked)ImGui::GetIO().FontGlobalScale = 1.0f;
		//ImGuiStyle& style = ImGui::GetStyle();
		//style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.62f);
		
	}
	
	
	void Destory_ImGui(){

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		
	}
	
	void Destroy_Backend(){

		glfwDestroyWindow(window);
		glfwTerminate();


	}
	
	void Destroy(){

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();


	}
	
	void NewFrame(){


		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();


	}

	void Render_PreMPV(){
		ImGui::Render();
		ImGuiIO &io = ImGui::GetIO();

		glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));  // we have to set the Viewport on every cycle because mpv_render_context_render internally rescales the fb of the context(?!)...
			


	}
	void Render_PostMPV(){

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);


	}
	
	void Resize(float w,float h){

		if(w == 1280.0){
			ImGui::GetIO().FontGlobalScale = 1.0f;
		}else{
			ImGui::GetIO().FontGlobalScale = 1.5f;
		}
		WIDTH = static_cast<int>(w);
		HEIGHT = static_cast<int>(h);
		glfwSetWindowSize(window,w,h);


	
	}
	
	
	uint64_t Process_UI_Events(std::chrono::time_point<std::chrono::system_clock> myeventtime){
		
		uint64_t ret_event = 0;
		



		
		glfwPollEvents();
		GLFWgamepadstate state;
		
		
		
		if ((GLFW_TRUE == glfwGetGamepadState(GLFW_JOYSTICK_1, &state)))
        {
			if(state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS && !B_A_PRESS){
				ret_event = bit_set(ret_event,BUT_A);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS && !B_B_PRESS){
				ret_event = bit_set(ret_event,BUT_B);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS && !B_X_PRESS){
				ret_event = bit_set(ret_event,BUT_X);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS && !B_Y_PRESS){
				ret_event = bit_set(ret_event,BUT_Y);
			}
			
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS && !B_DU_PRESS){
				ret_event = bit_set(ret_event,BUT_DUP);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS && !B_DD_PRESS){
				ret_event = bit_set(ret_event,BUT_DDOWN);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS && !B_DL_PRESS){
				ret_event = bit_set(ret_event,BUT_DLEFT);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS && !B_DR_PRESS){
				ret_event = bit_set(ret_event,BUT_DRIGHT);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS && !B_DR_PRESS){
				ret_event = bit_set(ret_event,BUT_DRIGHT);
			}
			
			
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.25f && !B_ZR_PRESS){
				ret_event = bit_set(ret_event,BUT_ZR);
			}
			
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0.25f && !B_ZL_PRESS){
				ret_event = bit_set(ret_event,BUT_ZL);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS && !B_R_PRESS){
				ret_event = bit_set(ret_event,BUT_R);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS && !B_L_PRESS){
				ret_event = bit_set(ret_event,BUT_L);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS && !B_TR_PRESS){
				ret_event = bit_set(ret_event,BUT_TR);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS && !B_TL_PRESS){
				ret_event = bit_set(ret_event,BUT_TL);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS && state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS && !B_TR_PRESS){
				ret_event = bit_set(ret_event,BUT_TR);
				ret_event = bit_set(ret_event,BUT_TL);
			}
			
		
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_START ] == GLFW_PRESS && !B_PLUS_PRESS){
				ret_event = bit_set(ret_event,BUT_PLUS);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_BACK ] == GLFW_PRESS && !B_MINUS_PRESS){
				ret_event = bit_set(ret_event,BUT_MINUS);
			}
			
			
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]<-0.25f && !B_AX_L_UP_PRESS){
				ret_event = bit_set(ret_event,B_AX_L_UP);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]>0.25f && !B_AX_L_DOWN_PRESS){
				ret_event = bit_set(ret_event,B_AX_L_DOWN);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]>0.25f && !B_AX_L_RIGHT_PRESS){
				ret_event = bit_set(ret_event,B_AX_L_RIGHT);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]<-0.25f && !B_AX_L_LEFT_PRESS){
				ret_event = bit_set(ret_event,B_AX_L_LEFT);
			}
			
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]>0.25f /*&& !AX_R_DOWN.pressed*/){
				ret_event = bit_set(ret_event,B_AX_R_DOWN);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]<-0.25f /*&& !AX_R_UP.pressed*/){
				ret_event = bit_set(ret_event,B_AX_R_UP);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]>0.25f && !AX_R_RIGHT.pressed){
				ret_event = bit_set(ret_event,B_AX_R_RIGHT);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]<-0.25f && !AX_R_LEFT.pressed){
				ret_event = bit_set(ret_event,B_AX_R_LEFT);
			}
			
			
			B_ZL_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]>-0.75f;
			B_ZR_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]>-0.75f;
			
			B_A_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;
			B_X_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;
			B_B_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS;
			B_Y_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS;
			B_DD_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS;
			B_DU_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS;
			
			B_DL_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS;
			B_DR_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS;

			B_R_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS;
			B_L_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS;
			B_TR_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS;
			B_TL_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS;

			B_PLUS_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS;
			B_MINUS_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS;
			
			
			
			B_AX_L_UP_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]<-0.25f;
			B_AX_L_DOWN_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]>0.25f;
			B_AX_L_LEFT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]>0.25f;
			B_AX_L_RIGHT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]<-0.25f;
			
			/*
			B_AX_R_UP_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]>0.25f;
			B_AX_R_DOWN_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]<-0.25f;
			B_AX_R_LEFT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]>0.25f;
			B_AX_R_RIGHT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]<-0.25f;
			*/
			
			//AX_R_UP.pressed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]>0.25f;
			//AX_R_DOWN.pressed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]<-0.25f;
			//AX_R_LEFT.pressed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]>0.25f;
			//AX_R_RIGHT.pressed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]<-0.25f;
			
			
			//auto AX_R_UP_CHRONO = std::chrono::duration_cast<std::chrono::microseconds>(myeventtime-AX_R_UP.eventchrono);
			
			
		}
		
	
	
	return ret_event;
	}
	
	bool WindowShouldClose(){

		return glfwWindowShouldClose(window);

	}
	
	void UniFontLoader(bool latinonly){
		//NXLOG::DEBUGLOG("Init Font Array\n");
		/*
		if(LoadSystemFonts){
		*/
			PlFontData standard, extended, chinese, korean;
			static ImWchar extended_range[] = {0xE000, 0xE152};
        
			if ((R_SUCCEEDED(plGetSharedFontByType(std::addressof(standard), PlSharedFontType_Standard))) &&
				R_SUCCEEDED(plGetSharedFontByType(std::addressof(extended), PlSharedFontType_NintendoExt)) &&
				R_SUCCEEDED(plGetSharedFontByType(std::addressof(chinese), PlSharedFontType_ChineseSimplified)) &&
				R_SUCCEEDED(plGetSharedFontByType(std::addressof(korean), PlSharedFontType_KO))) {
					
				u8 *px = nullptr;
				int w = 0, h = 0, bpp = 0;
				ImFontConfig font_cfg;
				
				font_cfg.FontDataOwnedByAtlas = false;
				ImGui::GetIO().Fonts->AddFontFromMemoryTTF(standard.address, standard.size, 20.0, std::addressof(font_cfg), ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
				font_cfg.MergeMode = true;
				
				ImGui::GetIO().Fonts->AddFontFromMemoryTTF(extended.address, extended.size, 20.0, std::addressof(font_cfg), extended_range);
				
				if (!latinonly) {
					ImGui::GetIO().Fonts->AddFontFromMemoryTTF(standard.address, standard.size, 20.0, std::addressof(font_cfg), ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
					ImGui::GetIO().Fonts->AddFontFromMemoryTTF(chinese.address,  chinese.size,  20.0, std::addressof(font_cfg), ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
					ImGui::GetIO().Fonts->AddFontFromMemoryTTF(korean.address,   korean.size,   20.0, std::addressof(font_cfg), ImGui::GetIO().Fonts->GetGlyphRangesKorean());
					
					
				}
				
				// build font atlas
				ImGui::GetIO().Fonts->GetTexDataAsAlpha8(std::addressof(px), std::addressof(w), std::addressof(h), std::addressof(bpp));
				ImGui::GetIO().Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
				ImGui::GetIO().Fonts->Build();
			}
		
	}


	
Tex load_texture_from_mem(unsigned char *data,unsigned int data_size){	
		int image_width = 0;
		int image_height = 0;
		int comp = 0;
	
		unsigned char* image_data = stbi_load_from_memory((const stbi_uc *)data, data_size, &image_width, &image_height, 0, 4); 
		
		if (image_data == NULL){
			return {};
		}
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		
		//if(comp == 3)
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
		//else if(comp == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

		return Tex{id,image_width,image_height};
}

	
	void Create_VO_FrameBuffer(float w,float h){
	
	
		glGenFramebuffers(1, &mpv_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, mpv_fbo);

		glGenTextures(1, &mpv_fbotexture);
		glBindTexture(GL_TEXTURE_2D, mpv_fbotexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpv_fbotexture, 0);

		
		
		fbo = {
					.fbo = static_cast<int>(mpv_fbo),
					.w = static_cast<int>(w),
					.h = static_cast<int>(h),
			};
			
		int __fbo_one = 1;
		fbo_params[0] = {MPV_RENDER_PARAM_OPENGL_FBO, &fbo};
		fbo_params[1] = {MPV_RENDER_PARAM_FLIP_Y, &__fbo_one};
		fbo_params[2] = {(mpv_render_param_type)0};



	}
	
	void Rescale_VO_Framebuffer(float _width, float _height){

		glBindTexture(GL_TEXTURE_2D, mpv_fbotexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpv_fbotexture, 0);

		//glBindRenderbuffer(GL_RENDERBUFFER, mpv_rbo);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mpv_rbo);

		fbo = {
					.fbo = static_cast<int>(mpv_fbo),
					.w = static_cast<int>(_width),
					.h = static_cast<int>(_height),
			};

	}
	
	void SetFullScreen(bool fullscreen){
		
	}
	
	
	void Draw_VO(){

		ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(WIDTH,HEIGHT),ImGuiCond_Always);
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.0));
		
		if(ImGui::Begin("##videowindow",nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoNavFocus)){
					ImGui::Image((void*)(intptr_t)mpv_fbotexture, ImVec2(WIDTH,HEIGHT));
		}
		ImGui::End();
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();

	}
	
	unsigned int getWidth(){


		return docked?1920:1280;

	}
	unsigned int getHeight(){


		return docked?1080:720;

	}
	
	void Resize_VO(float w,float h){

	}
	
	mpv_render_param * getMPV_Params(){
		return fbo_params;
	}
	

	GLuint getFBO_Texture(){
		return mpv_fbotexture;
	}

	
	void setEnableTouch(bool value){

		if(value){
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}



	}
	
	
	void setBGAlpha(float value){
		Window_Bg_color.w = value;
	}

	
	void SetColorTheme(int themecolor){
		
		ColorSetId currentTheme;
		Result rc = setsysInitialize();
		if (R_SUCCEEDED(rc)) {
		setsysGetColorSetId(&currentTheme);
		}
		
		if(themecolor == 0){
			SetDarkTheme();
		}else if(themecolor == 1){
			SetLightTheme();
		} else{
			if(currentTheme == 1){
				SetDarkTheme();
			}else{
				SetLightTheme();
			}
		}
	}
	
	
	void SetDarkTheme(){
		Text_color = ImVec4(255, 255, 255, 1.00f);
		Active_color = ImVec4(0, 255, 203, 1.00f);
		Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
		Window_Bg_color = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		OptsTab_Bg_color = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		Popup_Bg_color = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		NavHover_color = ImVec4(0.0f, 0.0f, 0.0f, 0.20f);
		HeaderHover_color = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
		ButtonActive_color = ImVec4(0, 255, 203, 0.30f);
		
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.62f);
		style.Colors[ImGuiCol_Text] = Text_color;
		
	}
	void SetLightTheme(){
		Text_color = ImVec4(0, 0, 0, 1.00f);
		Active_color = ImVec4(0, 0, 255, 1.00f);
		Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
		Window_Bg_color = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		OptsTab_Bg_color = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		Popup_Bg_color = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		NavHover_color = ImVec4(1.0f, 1.0f, 1.0f, 0.20f);
		HeaderHover_color = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
		Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
		ButtonActive_color = ImVec4(0, 0, 255, 0.30f);
		
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.62f);
		style.Colors[ImGuiCol_Text] = Text_color;
		
	}
	
}