#include "Main.h"

void setup() {
  ad.setup();
}

void loop() {
  hi.handle();
  ad.advertise();
}
