#ifndef SCENENODE_H
#define SCENENODE_H

#include <d3d11.h>
#include <directxmath.h>

#include "BindableCollection.h"


class SceneNode : public Drawable
{
public:
    SceneNode();
    ~SceneNode();

};

#endif // SCENENODE_H