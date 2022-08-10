#include "Button.h"
 
Button::Button(std::string text, glm::vec4 bounds, std::shared_ptr<Opal::Renderer> renderer, std::function<void()> on_click)
{
    mText = text;
    mMesh = renderer->CreateMesh(250);
    mBounds = bounds;
    mOnClick = on_click;
}

void Button::Tick(float dt)
{
    GenerateMesh();

    glm::vec2 mousePos = Opal::InputHandler::GetMousePos();

    mIsHovering = false;
    if(mousePos.x < mBounds.z && mousePos.x > mBounds.x && 
        mousePos.y < mBounds.w && mousePos.y > mBounds.y)
    {
        mIsHovering = true;  
    }

    if(mIsHovering && Opal::InputHandler::GetLeftMouseButtonDown())
    {
        mOnClick();
    }
}

void Button::Render(std::shared_ptr<Opal::MeshRenderer2D> mesh_renderer, std::shared_ptr<Opal::FontRenderer> font_renderer)
{
    mesh_renderer->Submit(mMesh);

    float width = font_renderer->MeasureText(mText);
    float middleBoundsX = mBounds.x + (mBounds.z - mBounds.x)/2.0f;
    float middleBoundsY = mBounds.y + (mBounds.w - mBounds.y)/2.0f;

    font_renderer->RenderString(mText, middleBoundsX - width/2.0f,
        middleBoundsY + font_renderer->GetSize() * 0.25f,
        mTextColor.r,
        mTextColor.g,
        mTextColor.b,
        mTextColor.a, 
        1.0f);

}

void Button::GenerateMesh()
{
    mVertices.clear();

    glm::vec4 color = (mIsHovering) ? mMeshHoverColor : mMeshColor;

    // Triangle 1
    // top left
    mVertices.push_back(mBounds.x);
    mVertices.push_back(mBounds.y);
    mVertices.push_back(color.r);
    mVertices.push_back(color.g);
    mVertices.push_back(color.b);
    mVertices.push_back(color.a);
    // top right
    mVertices.push_back(mBounds.z);
    mVertices.push_back(mBounds.y);
    mVertices.push_back(color.r);
    mVertices.push_back(color.g);
    mVertices.push_back(color.b);
    mVertices.push_back(color.a);
    // bottom right
    mVertices.push_back(mBounds.z);
    mVertices.push_back(mBounds.w);
    mVertices.push_back(color.r);
    mVertices.push_back(color.g);
    mVertices.push_back(color.b);
    mVertices.push_back(color.a);

    // Triangle 2
    // bottom right
    mVertices.push_back(mBounds.z);
    mVertices.push_back(mBounds.w);
    mVertices.push_back(color.r);
    mVertices.push_back(color.g);
    mVertices.push_back(color.b);
    mVertices.push_back(color.a);
    // bottom left
    mVertices.push_back(mBounds.x);
    mVertices.push_back(mBounds.w);
    mVertices.push_back(color.r);
    mVertices.push_back(color.g);
    mVertices.push_back(color.b);
    mVertices.push_back(color.a);
    // top left
    mVertices.push_back(mBounds.x);
    mVertices.push_back(mBounds.y);
    mVertices.push_back(color.r);
    mVertices.push_back(color.g);
    mVertices.push_back(color.b);
    mVertices.push_back(color.a);

    mMesh->SetVertices(mVertices.data(), mVertices.size() * sizeof(float));
}