#pragma once
#include "SceneNode.h"
#include "Drawable.h"

namespace Bind {
	class Bindable;
}

template<class T>
class DrawableBase : public SceneNode
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !staticBinds.empty();
	}
	static void AddStaticBind(Bind::Bindable* bind)
	{
		staticBinds.push_back(std::move(bind));
	}
private:
	const std::vector<Bind::Bindable*>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<Bind::Bindable*> staticBinds;
};

template<class T>
std::vector<Bind::Bindable*> DrawableBase<T>::staticBinds;
