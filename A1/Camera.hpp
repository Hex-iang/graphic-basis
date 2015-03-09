#pragma once

// =============================================================================================
// Camera class
// Some part of this class is retrieved from the online toturial on http://learnopengl.org

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const GLfloat SPEED      =  20.0f;
const GLfloat SENSITIVTY =  0.25f;
const GLfloat FOV        =  45.0f; // field of view


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    vec4 Position;
    vec4 Front;
    vec4 Up;
    vec4 Right;
    vec4 WorldUp;
    vec4 Center;

    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Fov;
    GLfloat Radius;
    GLfloat Alpha;

    // Constructor with vectors
    Camera(vec4 position = vec4(0.0f, 0.0f, 0.0f), vec4 up = vec4(0.0f, 1.0f, 0.0f), vec4 center = vec4(0.0f, 0.0f, -1.0f)) 
    : MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Fov(FOV), Alpha(0.0f)
    {
        this->Position = position;
        this->Center = center;
        
        this->Front = normalize(center - position); 

        this->Radius = distance(position, center);

        this->WorldUp = up;

        this->updateCameraVectors();
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    mat4 GetViewMatrix()
    {
        return lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    void RotateCamera(GLfloat increment, GLfloat dt, GLfloat camera_speed = 100.0f)
    {
        this->Alpha += increment * dt * camera_speed; 

        if (this->Alpha >= 360)
            this->Alpha = 0;
        else if(this->Alpha <= 0)
            this->Alpha = 360;


        GLfloat camX = this->Radius * sin(radians(this->Alpha));
        GLfloat camZ = this->Radius * cos(radians(this->Alpha));

        this->Position = vec4(camX, this->Position.y, camZ, 1.0f);

// #ifdef DEBUG
//         cout << "Camera Alpha: " << this->Alpha << endl;
//         cout << "Camera Position: x=" << this->Position.x << ", y=" << this->Position.y << ", z=" << this->Position.z << endl;
// #endif
        this->updateCameraVectors();
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void MoveCamera(Camera_Movement direction, GLfloat dt)
    {
        GLfloat velocity = this->MovementSpeed * dt;

        if (direction == FORWARD)
            this->Position += this->Front * velocity;
        if (direction == BACKWARD)
            this->Position -= this->Front * velocity;
        if (direction == LEFT)
            this->Position -= this->Right * velocity;
        if (direction == RIGHT)
            this->Position += this->Right * velocity;

        this->Radius = distance(this->Position, this->Center);


#ifdef DEBUG
        cout << "Camera Radius: " << this->Radius << endl;
#endif
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ChangeFOV(GLfloat yoffset)
    {

        if (this->Fov >= 1.0f && this->Fov <= 60.0f)
            this->Fov -= yoffset;

        // cout << "Field of View in Degree: " << this->Fov << endl;

        if (this->Fov <= 1.0f)
            this->Fov = 1.0f;
        if (this->Fov >= 60.0f)
            this->Fov = 60.0f;
    }

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        vec4 front;

        this->Front = normalize(this->Center - this->Position);
        
#ifdef DEBUG
        cout << "Normalized Front: " << this->Front.x << ", " << this->Front.y << ", " << this->Front.z << endl;
#endif
        // Also re-calculate the Right and Up vector
        this->Right = normalize(cross(this->Front, this->WorldUp));  
        this->Up    = normalize(cross(this->Right, this->Front));
    }
};