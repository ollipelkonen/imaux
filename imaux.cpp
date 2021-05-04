#include <memory>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
#include <SDL.h>
#include <SDL_opengl.h>

#include "lib/picopng.h"


int numLayers = 0;
int numNodes = 0;
int numConnections = 0;
unsigned int texture;

#include "layer.h"



unsigned int createTexture(unsigned char* data, int width, int height)
{
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  return texture;
}


ImVector<char*>       Items;
static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }


void    ClearLog()
{
  for (int i = 0; i < Items.Size; i++)
    free(Items[i]);
  Items.clear();
}

void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
  // FIXME-OPT
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
  buf[IM_ARRAYSIZE(buf)-1] = 0;
  va_end(args);
  Items.push_back(Strdup(buf));
}


void DrawLog()
{
  const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
  if (ImGui::BeginPopupContextWindow())
  {
    if (ImGui::Selectable("Clear")) ClearLog();
    ImGui::EndPopup();
  }
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
  for (int i = 0; i < Items.Size; i++)
  {
    const char* item = Items[i];

    ImVec4 color;
    bool has_color = false;
    if (strstr(item, "[error]"))          { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
    else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
    if (has_color)
        ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextUnformatted(item);
    if (has_color)
        ImGui::PopStyleColor();
  }

  if ( ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
    ImGui::SetScrollHereY(1.0f);


  ImGui::PopStyleVar();
  ImGui::EndChild();
}



std::chrono::high_resolution_clock::time_point t1, t2;

void timeStart()
{
  t1 = std::chrono::high_resolution_clock::now();
}

void timeEnd(std::string s)
{
  t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> time_span = t2 - t1;
  //std::cout << "____  " << s << "  took " << time_span.count() << " ms\n";
  AddLog( "_____ %s took %f ms", s.c_str(), time_span.count() );
}



void initImaux()
{
  int size = 2;
  timeStart();
  auto d = std::make_unique<Layer<double>>(size);
  auto n = d->addChild( size*=2 );
  do {
    n = n->addChild( size*size );
    size *= 2;
  }
  while ( size < 64 );
  //n = n->addChild( size*size );
  /*n = n->addChild( size*size );
  n = n->addChild( size*size );
  n = n->addChild( size*size );*/
  n = n->addChild( 320*200 );

  for (uint i=0; i<d->nodes.size(); i++)
    d->nodes[i]->value = 0.5;
  AddLog( "____ layers: %i   nodes: %i   numConnections: ", numLayers, numNodes, numConnections);

  timeEnd("initialization");

  timeStart();
  d->test();
  timeEnd("test");

  timeStart();
  d->calc();
  timeEnd("calc");

  //auto m = d->getLast();
  auto m = d->next->next;
  for (auto const& nod: m->nodes)
  {
    //std::cout << nod->value << "   ";
    AddLog( "%f", nod->value );
  }

  d->debugPrint();


  Layer<double>* last = d->getLast();
  int w = (int)sqrt(last->size);
  unsigned int* image = new unsigned int[w*w];

  double max = -100;
  for (int a=0; a<w*w; a++)
  {
    auto v = last->nodes[a]->value;
    if ( v>max ) max = v;
  }

  for (int a=0; a<w*w; a++)
  {
    unsigned char v = last->nodes[a]->value / max;
    v = -a;
    //v = (unsigned char)a;
    image[a] = (0xFF<<24) + (v<<16) + (v<<8) + v;
  }
  AddLog("create texture %ix%i max: %d ", w, w, max);
  texture = createTexture((unsigned char*)image, w, w);
  delete[] image;
}

void windows()
{
    //if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("__ Error: %s\n", SDL_GetError());
        return;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



int width = 320, height = 200;
unsigned int* data = new unsigned int[width*height];
for (int a=0;a<width*height; a++ )
{
  unsigned char v = (unsigned char)a;
  data[a] = (0xFF<<24) + (v<<16) + (v<<8) + v;
}
texture = createTexture( (unsigned char*)data, width, height );
delete[] data;

std::thread first(initImaux);

    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("imaux");


            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color


            ImVec2 size = {320,200};
ImGui::Image( reinterpret_cast<ImTextureID*>(texture), size);

            DrawLog();

            static char buf[128] = "blablabla";
            ImGui::InputText("##Text", buf, IM_ARRAYSIZE(buf));

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);


            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }


        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    first.join();
    SDL_Quit();

}



int main()
{

  windows();

  return 0;
}




/*

from: http://neuralnetworksanddeeplearning.com/chap1.html#implementing_our_network_to_classify_digits

net.SGD(training_data, 30, 10, 3.0, test_data=test_data)
def SGD(self, training_data, epochs, mini_batch_size, eta, test_data=None):

mini_batches = [
                training_data[k:k+mini_batch_size]
                for k in xrange(0, n, mini_batch_size)]
update_mini_batch(self, mini_batch, eta):

   for x, y in mini_batch:
            delta_nabla_b, delta_nabla_w = self.backprop(x, y)

-> activation = x
 delta = self.cost_derivative(activations[-1], y) * sigmoid_prime(zs[-1])
    nabla_b[-1] = delta
  nabla_w[-1] = np.dot(delta, activations[-2].transpose())


  z = np.dot(w, activation)+b
            zs.append(z)
            activation = sigmoid(z)

*/