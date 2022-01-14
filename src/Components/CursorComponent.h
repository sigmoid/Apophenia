#pragma once

#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/Graphics/Mesh2D.h"
#include "AttractableComponent.h"

#include <vector>
#include <string>
#include <glm/vec4.hpp>

class CursorComponent : public Opal::Component
{
    public:
    CursorComponent();
    ~CursorComponent();
    virtual void OnAdded() override;
    virtual void OnStart() override;
    virtual void Update(float dTime) override;
    virtual void Render(std::shared_ptr<Opal::BatchRenderer2D> ctx) override;
    virtual void OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB) override;
    virtual void Serialize() override;
    virtual void Deserialize() override;
    virtual std::shared_ptr<Component> Clone() override;

    void ToggleInput(bool takeInput);

    void Kill();
    bool GetKill();

    void SetDrunk(bool isdrunk);

    bool GetAlive();
    void SetAlive(bool val);
    void Reset();
    std::vector<std::string> GetResponse(bool onlyCore = false);
    std::shared_ptr<Opal::Mesh2D> GetMesh();

    private:
    float mNoiseScale = 0.05, mNoiseFreq = 1;
    float mTimeSinceBirth = 0;
    std::vector<float> mLastRadii;
    float mLerpFactor = 0.6f;
    float mMaxSpeed = 20, mCurrentSpeed = 0, mLastSpeed = 0, mAcceleration = 400;
    float mUpperBound = 1080 - 100, mLowerBound = 100;
    float mAttractionModifier = 25.0f;
    int mUpBinding = GLFW_KEY_W, mDownBinding = GLFW_KEY_S;
    bool mTakeInput = true;
    std::shared_ptr<Opal::TransformComponent> mTransform = nullptr;
    std::shared_ptr<AttractableComponent> mAttractable = nullptr;

    std::vector<std::string> mCurrentResponse;
    bool mAlive = true;
    bool mKilled = false;
    bool mIsDrunk = false;

    float mRadius = 34.0f, mMaxRadius = 69.0f, mNumTris = 128, mNoiseIntensity = 10;
    float mAttractionScale = 1200.0f;
    int mAttractionFalloff = 5;
    glm::vec4 mMeshColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    void UpdateVerts();
    std::vector<Attractor> mAttractors;
    std::shared_ptr<Opal::Mesh2D> mMesh;
    std::vector<float> mVertices;

    std::vector<std::string> mRealResponse;
};