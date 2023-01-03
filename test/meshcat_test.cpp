#include "MeshcatCpp/Material.h"
#include "MeshcatCpp/Shape.h"
#include <MeshcatCpp/Meshcat.h>


int main() {
  MeshcatCpp::Meshcat meshcat;
  // meshcat.set_property("/Background", "visible", false);

  MeshcatCpp::Material m;
  // m.wireframe = true;
  // meshcat.set_object("box", MeshcatCpp::Box(0.1, 0.1, 0.1), m);
  // meshcat.set_object("sphere", MeshcatCpp::Sphere(1), m);
  meshcat.set_object("e", MeshcatCpp::Ellipsoid(1, 1.5, 0.5));



  meshcat.join();

  return 0;
}
