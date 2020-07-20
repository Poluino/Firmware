#ifndef FirstBootCheck_h
#define FirstBootCheck_h

#include <Arduino.h>
#include <SPIFFS.h>
// library interface description
class FirstBootCheck
{
  // user-accessible "public" interface
  public:
    FirstBootCheck();
    int begin();
    int check_config();

  // library-accessible "private" interface
  private:
    int spiffs_init;
    void doSomethingSecret(void);
};

#endif