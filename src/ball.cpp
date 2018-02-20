#include "ball.h"
#include "main.h"

Ball::Ball(float x, float y, float radius,float n, color_t color) {

    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    speedx = 0.01;
    speedy = 0.01;

    this->radius = radius;
    GLfloat vertex_buffer_data[360*4*3];
    float pi = 3.14;
    int i = 0;
    float cur_theta = 0;

    for(i = 0;i < 360*4*3;i = i+3*3)
    {
        vertex_buffer_data[i+0] = 0.0f;
        vertex_buffer_data[i+1] = 0.0f;
        vertex_buffer_data[i+2] = 0.0f;

        vertex_buffer_data[i+3] = 0.0f + radius * cos(pi*cur_theta / 180.0);
        vertex_buffer_data[i+4] = 0.0f + radius * sin(pi*cur_theta / 180.0);
        vertex_buffer_data[i+5] = 0.0f;

        cur_theta += 1.0;

        vertex_buffer_data[i+6] = 0.0f + radius * cos(pi*cur_theta / 180.0);
        vertex_buffer_data[i+7] = 0.0f + radius * sin(pi*cur_theta / 180.0);
        vertex_buffer_data[i+8] = 0.0f;

    };

    this->object = create3DObject(GL_TRIANGLES,  n, vertex_buffer_data, color, GL_FILL);
}

void Ball::draw(glm::mat4 VP)
{
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, -1));
    rotate          = rotate * glm::translate(glm::vec3(0, 0, 0));  //rotation of the balls
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Ball::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Ball::tick() {
    this->position.x -= speedx;
    //this->position.y -= speedy;
}

bounding_box_t Ball::bounding_box() {
    float x = this->position.x, y = this->position.y;
    float r = this->radius;
    bounding_box_t bbox = { x, y, radius, radius };
    return bbox;
}


