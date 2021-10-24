#include "Pill.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Opal/Logger.h"

Pill::Pill(glm::vec2 startPos, float startRot)
{
    mRotation = (rand() % 10000) / 1000;
    mPosition = startPos;
    if((rand() % 100) < 50)
    {
        GenerateEllipse();
    }
    else
    {
        GenerateCapsule();
    }

    mSpeed = 250 + rand() % 60;
    mAngularSpeed = (float)(rand() % 100 - 50) / 100.0f;
}

void Pill::Update(float dTime)
{
    mPosition.y += mSpeed * dTime;
    mRotation += mAngularSpeed * dTime;
}

Opal::Mesh2D *Pill::GetMesh()
{
    if(mMesh == nullptr)
        return nullptr;
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(mPosition.x, mPosition.y, 0));
    model = glm::rotate(model, mRotation, glm::vec3(0,0,-1));
    mMesh->SetModelMatrix(model);
    return mMesh;
}

void Pill::GenerateCapsule()
{
    glm::vec4 color = GetRandomCapsuleColor();
    float radius = (float)(30 + rand() % 30) * 0.5f;
    float ellipseA = 1.0f, ellipseB = 1.0f;

    float numTris = 100;
    glm::vec2 startPos = glm::vec2(0,0);
    glm::vec2 firstPos, curPos, lastPos;
    lastPos = startPos;

    float height = 50 + rand()%30;

    bool first = true;

    glm::vec2 firstPoint, lastPoint;

    // Circle bottom half
    for(int i = 0; i < numTris; i++)
    {
        glm::vec2 newStart = startPos;
        newStart.y += height/2;
        float theta = (float)i / (float) numTris;
        theta *= 3.14159f;

        curPos = glm::vec2(ellipseA * cos(theta), ellipseB * sin(theta)) * radius;
        curPos.y += height/2;

        if(!first)
        {
            mVerts.push_back(newStart.x);
            mVerts.push_back(newStart.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
            mVerts.push_back(lastPos.x);
            mVerts.push_back(lastPos.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
            mVerts.push_back(curPos.x);
            mVerts.push_back(curPos.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
        }
        

        lastPos = curPos;

        if(first)
        {
            firstPoint = curPos;
        }
        else if(i == numTris - 1)
        {
            lastPoint = curPos;
        }

        first = false;
    }

    // From botom to middle
    glm::vec2 midPointL = glm::vec2(-radius, 0);
    glm::vec2 midPointR = glm::vec2(radius, 0);


    mVerts.push_back(midPointR.x);
    mVerts.push_back(midPointR.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);
    mVerts.push_back(firstPoint.x);
    mVerts.push_back(firstPoint.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);
    mVerts.push_back(lastPoint.x);
    mVerts.push_back(lastPoint.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);

    mVerts.push_back(lastPoint.x);
    mVerts.push_back(lastPoint.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);
    mVerts.push_back(midPointL.x);
    mVerts.push_back(midPointL.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);
    mVerts.push_back(midPointR.x);
    mVerts.push_back(midPointR.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);


    color = GetRandomCapsuleColor();

    first = true;
    // circle top half;

    for(int i = 0; i < numTris; i++)
    {
        glm::vec2 newStart = startPos;
        newStart.y -= height/2;
        float theta = (float)i / (float) numTris;
        theta *= 3.14159f;
        theta += 3.14159f;

        curPos = glm::vec2(ellipseA * cos(theta), ellipseB * sin(theta)) * radius;
        curPos.y -= height/2;

        if(!first)
        {
            mVerts.push_back(newStart.x);
            mVerts.push_back(newStart.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
            mVerts.push_back(lastPos.x);
            mVerts.push_back(lastPos.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
            mVerts.push_back(curPos.x);
            mVerts.push_back(curPos.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
        }
        

        lastPos = curPos;

        if(first)
        {
            firstPoint = curPos;
        }
        else if(i == numTris - 1)
        {
            lastPoint = curPos;
        }

        first = false;
    }

    // Top to middle
    mVerts.push_back(midPointR.x);
    mVerts.push_back(midPointR.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);
    mVerts.push_back(firstPoint.x);
    mVerts.push_back(firstPoint.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);
    mVerts.push_back(lastPoint.x);
    mVerts.push_back(lastPoint.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);

    mVerts.push_back(firstPoint.x);
    mVerts.push_back(firstPoint.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);
    mVerts.push_back(midPointR.x);
    mVerts.push_back(midPointR.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);
    mVerts.push_back(midPointL.x);
    mVerts.push_back(midPointL.y);
    mVerts.push_back(color.r);
    mVerts.push_back(color.g);
    mVerts.push_back(color.b);
    mVerts.push_back(color.a);

    mMesh = Opal::Game::Instance->Renderer->CreateMesh(mVerts.size()/3);
    mMesh->SetVertices(mVerts.data(), mVerts.size() * sizeof(float));
}

void Pill::GenerateEllipse()
{
    float r = (float)(rand() % 100) / 100.0f;
    glm::vec4 color = glm::vec4(r, 0.3f, 0.3f, 1.0f);
    glm::vec4 dividerColor = glm::vec4(r * 0.5f, 0.3f * 0.5f, 0.3f * 0.5f, 1.0f);
    float radius = (float)(30 + rand() % 30) * 0.5f;
    float ellipseA = 1.0f, ellipseB = 1.0f;
    ellipseB += (float)(rand() % 100) / 400;
    if(ellipseB < 1.2f)
    {
        ellipseB = 1;
    }

    float numTris = 100;
    glm::vec2 startPos = glm::vec2(0,0);
    glm::vec2 firstPos, curPos, lastPos;
    lastPos = startPos;

    float height = 5;

    bool first = true;

    glm::vec2 firstPoint, lastPoint;

    // Circle bottom half
    for(int i = 0; i < numTris; i++)
    {
        glm::vec2 newStart = startPos;
        newStart.y += height/2;
        float theta = (float)i / (float) numTris;
        theta *= 3.14159f;

        curPos = glm::vec2(ellipseA * cos(theta), ellipseB * sin(theta)) * radius;
        curPos.y += height/2;

        if(!first)
        {
            mVerts.push_back(newStart.x);
            mVerts.push_back(newStart.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
            mVerts.push_back(lastPos.x);
            mVerts.push_back(lastPos.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
            mVerts.push_back(curPos.x);
            mVerts.push_back(curPos.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
        }
        

        lastPos = curPos;

        if(first)
        {
            firstPoint = curPos;
        }
        else if(i == numTris - 1)
        {
            lastPoint = curPos;
        }

        first = false;
    }

    // From botom to middle
    glm::vec2 midPointL = glm::vec2(-radius, 0);
    glm::vec2 midPointR = glm::vec2(radius, 0);

    float darken = 1.2f;

    mVerts.push_back(midPointR.x);
    mVerts.push_back(midPointR.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);
    mVerts.push_back(firstPoint.x);
    mVerts.push_back(firstPoint.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);
    mVerts.push_back(lastPoint.x);
    mVerts.push_back(lastPoint.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);

    mVerts.push_back(lastPoint.x);
    mVerts.push_back(lastPoint.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);
    mVerts.push_back(midPointL.x);
    mVerts.push_back(midPointL.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);
    mVerts.push_back(midPointR.x);
    mVerts.push_back(midPointR.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);

    first = true;
    // circle top half;

    for(int i = 0; i < numTris; i++)
    {
        glm::vec2 newStart = startPos;
        newStart.y -= height/2;
        float theta = (float)i / (float) numTris;
        theta *= 3.14159f;
        theta += 3.14159f;

        curPos = glm::vec2(ellipseA * cos(theta), ellipseB * sin(theta)) * radius;
        curPos.y -= height/2;

        if(!first)
        {
            mVerts.push_back(newStart.x);
            mVerts.push_back(newStart.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
            mVerts.push_back(lastPos.x);
            mVerts.push_back(lastPos.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
            mVerts.push_back(curPos.x);
            mVerts.push_back(curPos.y);
            mVerts.push_back(color.r);
            mVerts.push_back(color.g);
            mVerts.push_back(color.b);
            mVerts.push_back(color.a);
        }
        

        lastPos = curPos;

        if(first)
        {
            firstPoint = curPos;
        }
        else if(i == numTris - 1)
        {
            lastPoint = curPos;
        }

        first = false;
    }

    // Top to middle
    mVerts.push_back(midPointR.x);
    mVerts.push_back(midPointR.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);
    mVerts.push_back(firstPoint.x);
    mVerts.push_back(firstPoint.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);
    mVerts.push_back(lastPoint.x);
    mVerts.push_back(lastPoint.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);

    mVerts.push_back(firstPoint.x);
    mVerts.push_back(firstPoint.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);
    mVerts.push_back(midPointR.x);
    mVerts.push_back(midPointR.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);
    mVerts.push_back(midPointL.x);
    mVerts.push_back(midPointL.y);
    mVerts.push_back(color.r * darken);
    mVerts.push_back(color.g * darken);
    mVerts.push_back(color.b * darken);
    mVerts.push_back(color.a * darken);

    mMesh = Opal::Game::Instance->Renderer->CreateMesh(mVerts.size()/3);
    mMesh->SetVertices(mVerts.data(), mVerts.size() * sizeof(float));
}

glm::vec4 Pill::GetRandomCapsuleColor()
{
    return Opal::OpalMath::HSVtoRGB(rand()%255, 80, 70);
}