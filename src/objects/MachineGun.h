#include "Emplacement.h"

class MachineGun : public Emplacement {
public:
    MachineGun(float x, float y);

    bool Update() override;
};
