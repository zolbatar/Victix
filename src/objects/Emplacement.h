#include "Object.h"
#include "../model/WorldPosition.h"

class Emplacement : public Object {
protected:
    static float size;
    bool is_player = true;
public:
    Emplacement(float x, float y);

    bool Update() override;

    static void AddEmplacement(float x, float y, WorldPosition &state);

    void Render(cairo_t *cr) override;
};
