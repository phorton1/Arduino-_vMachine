#include <grblTouchUI.h>

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("touchUI::examples::basic.ino setup() started");

    touchUI_init();

    Serial.println("touchUI::examples::basic.ino setup() started");
    delay(500);
}

void loop()
{

}
