#include <vector>
#include "Object.h"
#include "../model/WorldPosition.h"

class Emplacement : public Object {
protected:
    static float size;

    static void RenderInternal(float x, float y, float a, Player player, bool outline, bool valid);

public:
    Emplacement(float x, float y, Player player);

    bool Update() override;

    enum Type Type() override;

    static void AddEmplacement(float x, float y, bool final, Player player);

    static void Clear();

    static void Restore();

    void Render() override;
};
