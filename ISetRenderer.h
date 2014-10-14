#ifndef ISETRENDERER_H
#define ISETRENDERER_H

class IRenderer;

class ISetRenderer {
public:
    virtual void setRenderer(IRenderer *renderer) = 0;
};

#endif // ISETRENDERER_H
