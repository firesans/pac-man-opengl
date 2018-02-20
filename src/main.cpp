#include <math.h>
#include <stdio.h>
#include "main.h"
#include "timer.h"
#include "ball.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

Ball ball1, player, Pond, balls[20000], ball_reflector,ball_reflector1;

int score = 0;

float screen_zoom = 1.0;
double xpos = 0,ypos = 0;
int p = 0;

float screen_center_x , screen_center_y;

int frames = 0,reflector_frames = 0;
int max_h = 0;
int reflector_count = 0;
int alive_count =0 ;

int death_flag = 0, death_flag1 = 0;

Timer t60(1.0 / 80);

struct entity{

    VAO* vao;
    float x;
    float y;
    float angle;
};

typedef struct entity entity;

entity ground,pool,grass,trampoline,rect1,rect2,rectIndicator,rectIndicator1;
entity reflectors[10];
entity rod1,rod2,rod3;
entity rod[3];
entity porcupines[10];
entity indicator;
//entity rod1;

float rod1x = -3.67;
float rod2x = -16.00;
float porx = -1.1f;
int sign = 1;
float init_life = -3.6f;

int count=0,mag_status = 0;

void drawGround(entity obj)
{
    static const GLfloat vertex_buffer_data[] = {
        -100.0f, -3.0f, 0.0f,
         100.0f, -3.0f, 0.0f,
        -100.0f, -4.0f, 0.0f,

         100.0f, -3.0f, 0.0f,
         100.0f, -4.0f, 0.0f,
        -100.0f, -4.0f, 0.0f,
    };

    obj.vao = create3DObject(GL_TRIANGLES, 3*2, vertex_buffer_data, COLOR_BROWN, GL_FILL);
    draw3DObject(obj.vao);
}

void drawGrass(entity obj)
{
    static const GLfloat vertex_buffer_data[] = {
        -100.0f, -2.5f, 0.0f,
         100.0f, -2.5f, 0.0f,
        -100.0f, -3.0f, 0.0f,

         100.0f, -2.5f, 0.0f,
         100.0f, -3.0f, 0.0f,
        -100.0f, -3.0f, 0.0f,
    };

    obj.vao = create3DObject(GL_TRIANGLES, 3*2, vertex_buffer_data, COLOR_GREEN, GL_FILL);
    draw3DObject(obj.vao);
}

void drawSemicircle(entity obj,float x,float y,float radius,color_t c)
{
    GLfloat vertex_buffer_data[360*4*3];
    float pi = 3.14;
    int i = 0;
    float cur_theta = 0;

    for(i = 0;i < 360*4*3;i = i+3*3)
    {
        vertex_buffer_data[i+0] = x;
        vertex_buffer_data[i+1] = y;
        vertex_buffer_data[i+2] = 0.0f;

        vertex_buffer_data[i+3] = x + radius * cos(pi*cur_theta / 180.0);
        vertex_buffer_data[i+4] = y + radius * sin(pi*cur_theta / 180.0);
        vertex_buffer_data[i+5] = 0.0f;

        cur_theta -= 1.0;

        vertex_buffer_data[i+6] = x + radius * cos(pi*cur_theta / 180.0);
        vertex_buffer_data[i+7] = y + radius * sin(pi*cur_theta / 180.0);
        vertex_buffer_data[i+8] = 0.0f;

    };
    obj.vao = create3DObject(GL_TRIANGLES, 543, vertex_buffer_data,c,GL_FILL);
    draw3DObject(obj.vao);

}

void drawRectangles(entity obj, float length, float width, float centre, color_t c)
{
    GLfloat vertex_buffer_data[19];
    vertex_buffer_data[0] = -length/2;
    vertex_buffer_data[1] =  width/2;
    vertex_buffer_data[2] = 0.0f;

    vertex_buffer_data[3] = length/2;
    vertex_buffer_data[4] = width/2;
    vertex_buffer_data[5] = 0.0f;

    vertex_buffer_data[6] = -length/2;
    vertex_buffer_data[7] = -width/2;
    vertex_buffer_data[8] = 0.0f;

    vertex_buffer_data[9] = length/2;
    vertex_buffer_data[10] = width/2;
    vertex_buffer_data[11] = 0.0f;

    vertex_buffer_data[12] = length/2;
    vertex_buffer_data[13] = -width/2;
    vertex_buffer_data[14] = 0.0f;

    vertex_buffer_data[15] = -length/2;
    vertex_buffer_data[16] = -width/2;
    vertex_buffer_data[17] = 0.0f;

    obj.vao = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, c, GL_FILL);
    obj.x = centre;
    obj.y = 0.0f;
    draw3DObject(obj.vao);

}

void drawReflectors(glm::mat4 VP,float px, float py, float angle)
{
    Ball ball_reflector;
    entity rod;
    ball_reflector = Ball(px,py,0.35,360*4,COLOR_CYAN);
    ball_reflector.draw(VP);

    ball_reflector.tick();
    glm::mat4 MVP = VP * Matrices.model;

    glm::vec3 Axis = glm::vec3(0.0,0.0,1.0);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f),angle, Axis) * Matrices.model ;
    glm::mat4 translate_rod = glm::translate(glm::mat4(1.0f), glm::vec3(0.35, 0.2, 0.0));
    //glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(px, py, 0.0f));
    //glm::mat4 translate_back = glm::translate(glm::mat4(1.0f), glm::vec3(-px, -py, 0.0f));

    MVP = MVP * (translate_rod * rotate);
    //* translate * translate_rod;

    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    drawRectangles(rod,0.05,0.25f,0.0,COLOR_BROWN);

}

void drawTriangles(entity obj, float n, float side, color_t c)
{
    GLfloat vertex_buffer_data[1000];
    //float side = 0.1f;
    float a = 0.0f;
    for(int i = 0; i < 9*n; i = i+9){

        vertex_buffer_data[i+0] = a;
        vertex_buffer_data[i+1] = 0.0f;
        vertex_buffer_data[i+2] = 0.0f;

        vertex_buffer_data[i+3] = a + side/2;
        vertex_buffer_data[i+4] = side/2;
        vertex_buffer_data[i+5] = 0.0f;

        vertex_buffer_data[i+6] = a + side;
        vertex_buffer_data[i+7] = 0.0f;
        vertex_buffer_data[i+8] = 0.0f;

        a = a + side;
    }

    obj.vao = create3DObject(GL_TRIANGLES, 3*n, vertex_buffer_data, c, GL_FILL);
    obj.x = a;
    draw3DObject(obj.vao);

}

/* Render the scene with openGL */
/* Edit this function according to your assignment */

//rod[0].x = -3.68f;

void draw(GLFWwindow *window) {
    // clear the color and depth in the frame buffer

    //if(indicator.x >= -1.9f)
    //{
    //    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    quit(window);
    //}

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    // glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    // glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    // glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!

    Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model



    ball_reflector.draw(VP);

    MVP = VP * Matrices.model;
    glm::vec3 Axis = glm::vec3(0.0,0.0,1.0);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f),15.0f, Axis);
    glm::mat4 translate_rod = glm::translate(glm::mat4(1.0f), glm::vec3(0.32, -0.2, 0.0));
    //glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(px, py, 0.0f));
    //glm::mat4 translate_back = glm::translate(glm::mat4(1.0f), glm::vec3(-px, -py, 0.0f));

    MVP = MVP * (translate_rod * rotate) ;
    //* translate * translate_rod;

    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    drawRectangles(rod[0],0.1,0.75f,-3.68,COLOR_GREY);

    rod[0].angle = 15.0f;
    rod1x += (- ball_reflector.speedx);
    rod[0].x = rod1x;
    rod[0].y = -0.2 ;


    ball_reflector1.draw(VP);

    MVP = VP * Matrices.model;
    glm::vec3 Axis2 = glm::vec3(0.0,0.0,1.0);
    glm::mat4 rotate2 = glm::rotate(glm::mat4(1.0f),15.0f, Axis2);
    glm::mat4 translate_rod2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.32, -0.2, 0.0));
    //glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(px, py, 0.0f));
    //glm::mat4 translate_back = glm::translate(glm::mat4(1.0f), glm::vec3(-px, -py, 0.0f));

    MVP = MVP * (translate_rod2 * rotate2) ;
    //* translate * translate_rod;

    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    drawRectangles(rod[1],0.1,0.75f,-3.68,COLOR_GREY);

    rod[1].angle = 15.0f;
    rod1x += (- ball_reflector.speedx);
    rod[1].x = rod2x;
    rod[1].y = 1.8 ;

    //porcupines

    porx += sign * 0.001;
    porcupines[0].x = porx;
    porcupines[0].y = -0.623f;

    glm::mat4 translate_porcupine = glm::translate(glm::mat4(1.0f), glm::vec3(porcupines[0].x, porcupines[0].y, 0.0));
    glm::mat4 MVP1 = glm::mat4(1.0f);
    MVP1 = MVP1 * translate_porcupine;

    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP1[0][0]);
    drawTriangles(porcupines[0],3,0.1f,COLOR_ORANGE);
    //porx += 0.01;

    //porcupines[0].x += porx;
    //printf("%f  %f\n",porcupines[0].x,player.position.x);

    if(porcupines[0].x > -0.4f)
    {
        sign = -1;
    }

    if(porcupines[0].x < -1.6f)
    {
        sign = +1;
    }

    Matrices.model = glm::mat4(1.0f);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    drawGround(ground);
    drawGrass(grass);
    drawSemicircle(pool,1,-2.5,1,COLOR_SKYBLUE);
    drawSemicircle(trampoline, 3,-1.50,0.75,COLOR_RED);


    MVP = VP* glm::translate(glm::mat4(), glm::vec3(-3.0f, -3.5f, 0.0f)) * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    drawRectangles(rectIndicator,0.75,0.05,0.0,COLOR_GREEN);

    MVP = VP* glm::translate(glm::mat4(), glm::vec3(-2.25f, -3.5f, 0.0f)) * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    drawRectangles(rectIndicator1,0.75,0.05,0.0,COLOR_RED);


    if(death_flag || death_flag1) {
        init_life += 0.1;
        indicator.x = init_life;
    }
    indicator.y = -3.8f;
    Matrices.model = glm::mat4(1.0f);
    MVP = VP * glm::translate(glm::mat4(), glm::vec3(indicator.x,indicator.y,0.0f)) * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    drawTriangles(indicator,1,0.5f,COLOR_PINK);

    Matrices.model = glm::mat4(1.0f);
    MVP = VP * glm::translate(glm::mat4(), glm::vec3(2.225f, -2.0f, 0.0f)) * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    drawRectangles(rect1,0.1,1,0.0,COLOR_RED);

    MVP = VP * glm::translate(glm::mat4(), glm::vec3(3.75f, -2.0f, 0.0f)) * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    drawRectangles(rect2,0.1,1,0.0,COLOR_RED);

    //Matrices.model = glm::mat4(1.0f);

    //actual place
    player.draw(VP);

    int i = 0;
    for(i = 0;i< 20;i++)
    {
        balls[i].draw(VP);
    }
    // Scene render
    ball1.draw(VP);

    //Magnets
    if(frames > 500  && count < 200){

    Matrices.model = glm::mat4(1.0f);
    MVP = VP * glm::translate(glm::mat4(), glm::vec3(-2.25f, 2.25f, 0.0f)) * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    drawRectangles(rod1,0.75,0.25,0.0,COLOR_RED);

    Matrices.model = glm::mat4(1.0f);
    MVP = VP * glm::translate(glm::mat4(), glm::vec3(-2.5f, 2.05f, 0.0f)) * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    drawRectangles(rod2,0.25,0.5,0.0,COLOR_RED);

    Matrices.model = glm::mat4(1.0f);
    MVP = VP * glm::translate(glm::mat4(), glm::vec3(-2.25f, 1.70f, 0.0f)) * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    drawRectangles(rod3,0.75,0.25,0.0,COLOR_RED);

    count++;
    mag_status = 1;

    }

    else mag_status = 0;

    if(count == 200) count = 0;
    if(frames >= 700) frames = 0;

    frames ++;

}

int detect_collision_reflector(bounding_box_t a, entity obj){

   // printf("pos: %f %f -- %f %f\n",a.x,a.y,obj.x,obj.y);

    if((abs(a.x - obj.x) < (a.width + 0.05)) &&
            (abs(a.y - obj.y) < (a.height + 0.35)))
    {
        printf("detected\n");
        return 1;
    }
    else
        return 0;

}

float gravity = 0.0003;
float collide_yes = 0;

void detect_pond()
{
    if(detect_innercollision(player.bounding_box(),1,-2.5) && player.position.y <= -2.50f)
    {
        collide_yes = 1;
        player.speedy = 0.00f;


        if(player.position.x >= 0.0f && player.position.x <= 0.65f)
        {
           player.position.x += 0.002;
           player.position.y -= 0.0007;
        }

        if(player.position.x >= 0.65f && player.position.x <= 1.0f)
        {
            player.position.x += 0.001;
            player.position.y -= 0.0007;
        }

        if(player.position.x >= 1.0f && player.position.x <= 1.35f)
        {
            player.position.x -= 0.0009;
            player.position.y -= 0.0007;
        }

        if(player.position.x >= 1.35f && player.position.x <= 2.00f)
        {
            player.position.x -= 0.0009;
            player.position.y -= 0.0007;
        }


       // printf("Detected\n");

    }
    if(player.position.y <= -3.39f)
    {
        player.position.y = -3.39f;
        player.speedy = 0.1f;
        //pond_flag = 1;
    }

}

void detect_trampoline()
{
    if(player.position.x >= 2.25f && player.position.x <= 3.75 && player.position.y <= -1.35f)
    {
        player.position.y = player.position.y  + 1.555f;
        player.speedy += 0.001;
    }
}

int detect_magnet()
{
    if(player.position.y <= 3.0f && player.position.y >= 1.50f && player.position.x >= -2.25f && mag_status == 1)
    {
        //player.position.x -= 0.009;
        player.position.x -= 0.09;
        return 1;
    }
    else return 0;
}

void detect_reflector(entity obj)
{
    if(detect_collision_reflector(player.bounding_box(),obj))
    {
        player.rotation = 2*obj.angle;
        //player.speedy = 0;
        player.position.x += 1*cos(2*90.00 - 2*obj.angle);
        player.position.y += 1*sin(2*90.00 - 2*obj.angle);
    }
}

int detect_collision_porcupine(bounding_box_t a,entity obj)
{
    //printf("value of y : %f \n",player.position.y);
    if((a.y == -2.35f) &&
            (a.x > obj.x*4 && a.x < obj.x*4 + 3*0.3f))
    {
        //printf("detected\n");
        return 1;
    }
    else
        return 0;
}


int detect_porcupine(entity obj)
{
    if(detect_collision_porcupine(player.bounding_box(),obj))
    {
        player.set_position(0.0f,0.0f);
        return 1;
    }
    else
        return 0;

}


void tick_input(GLFWwindow *window ,double mouse_x, double mouse_y) {

    int left  = glfwGetKey(window, GLFW_KEY_A);
    int right = glfwGetKey(window, GLFW_KEY_D);
    int up = glfwGetKey(window,GLFW_KEY_SPACE);
    int down = glfwGetKey(window, GLFW_KEY_DOWN);

    glfwGetCursorPos(window,&xpos,&ypos);
    //printf("%lf",&xpos);
    double diff = xpos - mouse_x;
    //printf("%lf",diff);
    player.position.x += (xpos - mouse_x);

    player.speedy += gravity;
    player.position.y -= 0.1*player.speedy;

    if (left){
            player.position.x -= 0.01;
    }
    if (right){
            player.position.x +=  0.01;
    }

    if(up){

        player.position.y = player.position.y + 0.25*player.speedy ;
        player.speedy -= gravity;
    }

    //edge conditions for reproducing the balls again into the playground

    for(int k = 0; k<20; k++)
    {
        if(balls[k].position.x >= 4.3f)
        {
            balls[k].position.x = -4.1f;
        }
    }

    //player ball edge critical conditions

    if(player.position.x >= (4.0f-player.radius))
    {
        player.position.x = 0.0f;
        player.position.y = -2.35f;
    }

    if(player.position.x <= -(4.0f-player.radius))
    {
        player.position.x = 0.0f;
        player.position.y = -2.35f;
    }

    // water pond position considered
    if(player.position.y <= -(2.5f-player.radius) && (player.position.x < 0.0f || player.position.x > 2.00f))
    {
        player.position.y = -2.35f;
    }

    if(player.position.y >= (4.0f-player.radius))
    {
        player.position.y = -2.35f;
    }

    //printf("speedy : %f ",player.speedy);
    detect_trampoline();
    death_flag = detect_magnet();
    detect_pond();
    detect_reflector(rod[0]);
    //detect_reflector(rod[1]);
    death_flag1 = detect_porcupine(porcupines[0]);
}

int k = 0;
void tick_elements() {

    ball1.tick();
    ball1.speedx = -0.01;
    ball1.speedy = 0;
    ball_reflector.tick();
    ball_reflector1.tick();
    player.tick();
    int collide_ball = 0;

    for(int k = 0;k < 20; k++)
    {
        balls[k].tick();
    }


    //if(detect_collision(player.bounding_box(),ball_reflector.bounding_box()))
    //{
    //  ball_reflector.set_position(-4.10f,ball_reflector.position.y);
    //  player.rotation = 2*rod[0].angle;
    //}

    for(int p = 0;p < 20;p++)
    {
        if (detect_collision(balls[p].bounding_box(), player.bounding_box())) {

            balls[p].set_position(-4.50f,balls[p].position.y);
            //player.speedx = player.speedx + 0.0001;
            player.speedy = player.speedy + 0.009;
            player.position.y += player.speedy;
            collide_ball = 1;
            score = score+10;
            printf("SCORE :- %d\n",score);
        }
    }
   // zooming();

}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    ball1       = Ball(0, 0, 0.2, 360*4, COLOR_RED);
    player       = Ball(-1, -1, 0.15, 360*2.5, COLOR_YELLOW);
    ball_reflector = Ball(-4, 0, 0.35, 360*4, COLOR_CYAN);
    ball_reflector1 = Ball(-16, 2, 0.35, 360*4, COLOR_CYAN);

    float value_x, value_y;
    int j = 0;

    while(j < 20)
    {
            float random_x = -3.5 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(6.5)));
            float random_y = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(4.5)));

            value_x = random_x;
            value_y = random_y;

            if( j% 3 == 0)
            {
                balls[j] = Ball(value_x, value_y, 0.2f, 360*4, COLOR_BLUE);
                balls[j].speedx = balls[j-2].speedx + -0.0001;
            }
            else
            {
                balls[j] = Ball(value_x, value_y, 0.25f, 360*4, COLOR_PINK);
                balls[j].speedx = balls[j-1].speedx + -0.005;
            }
            j++;

    }
    player.speedx = 0.00f;
    player.speedy = 0.001f;
    player.speedx = -player.speedx;

    ball_reflector.speedx = -ball_reflector.speedx;
    ball_reflector1.speedx = -ball_reflector1.speedx;

    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (0 / 256.0, 0 / 256.0, 0 / 156.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main(int argc, char **argv) {
    srand(time(0));
    int width  = 1000;
    int height = 1000;

    window = initGLFW(width, height);
    //drawGround();
    initGL (window, width, height);

    int j=0;
    /* Draw in loop */

    double mx,my;
    indicator.x = -3.6f;

    while (!glfwWindowShouldClose(window) ) {
        // Process timers

         if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw(window);
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);
            tick_elements();
            //zooming();
            glfwGetCursorPos(window,&mx,&my);
            tick_input(window,mx,my);
            reset_screen(window);
        }

        glfwPollEvents();

        // Poll for Keyboard and mouse events
    }
    printf("%d",score);
    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) < (a.width + b.width)) &&
            (abs(a.y - b.y) < (a.height + b.height));
}

bool detect_innercollision(bounding_box_t a,float px,float py)
{
    return (pow(abs(a.x - px),2) + pow(abs(a.y - py),2) >= pow(abs(a.width - 1),2));
}

void reset_screen(GLFWwindow *window) {

    int move_right = glfwGetKey(window,GLFW_KEY_RIGHT);
    int move_left = glfwGetKey(window,GLFW_KEY_LEFT);

    int zoom_in = glfwGetKey(window,GLFW_KEY_UP);
    int zoom_out = glfwGetKey(window,GLFW_KEY_DOWN);

    if(move_right)
    {
        screen_center_x -= 0.01;
    }
    if(move_left)
    {
        screen_center_x += 0.01;
    }

    if(zoom_in)
    {
        screen_zoom += 0.01;
    }
    if(zoom_out)
    {
        screen_zoom -= 0.01;
    }


    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;

    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
