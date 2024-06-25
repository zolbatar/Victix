#include <vector>
#include "Object.h"
#include "../model/WorldPosition.h"

class Emplacement : public Object {
protected:

    static void
    RenderInternal(Emplacement *emplacement, float x, float y, float a, Player player, bool outline, bool valid);

    float charge = 0.0;

public:
    static float size;
    int cost;

    Emplacement(float x, float y, Player player);

    bool Update() override;

    void Activate() override;

    bool ReadyToActivate() override;

    enum Type GetType() override;

    static void AddEmplacement(float x, float y, bool final, Player player);

    static void Clear();

    static void Restore();

    void Render() override;
};
