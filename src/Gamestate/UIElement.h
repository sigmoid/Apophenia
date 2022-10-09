#pragma once

class UIElement
{
public:

	virtual void Tick(float dt) = 0;
	virtual void Render(std::shared_ptr<Opal::MeshRenderer2D> mesh_renderer, std::shared_ptr<Opal::FontRenderer> font_renderer) = 0;
	virtual void Select(bool is_selected) = 0;
};