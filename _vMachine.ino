
#include <Grbl.h>

void setup()
{
    delay(2000);
    grbl_init();
}

void loop()
{
    run_once();
}
