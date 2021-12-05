
#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../../Opal/Graphics/Mesh2D.h"
#include "../../Opal/OpalMath.h"
#include "AttractableComponent.h"

#include <vector>
#include <string>
#include <glm/vec4.hpp>

class DragComponent : public Opal::Component
{
    public:
    DragComponent(float amt);
    ~DragComponent();
    virtual void OnAdded() override;
    virtual void OnStart() override;
    virtual void Update(float dTime) override;
    virtual void Render(std::shared_ptr<Opal::BatchRenderer2D> ctx) override;
    virtual void OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB) override;
    virtual void Serialize() override;
    virtual void Deserialize() override;
    virtual std::shared_ptr<Component> Clone() override;

    private:
    float mFactor = 1;
    std::shared_ptr<Opal::VelocityComponent> mVelocity;
};