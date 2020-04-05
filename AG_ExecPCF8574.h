// ver.3.0 2020-03-24


// ********************************************************************************************************************* //
// класс для внешних пинов и расширителей: на входе _pin - active - логический уровень включения:                         //
// ********************************************************************************************************************* //
#ifndef _PCF8574_ESP_H
#include <PCF8574_esp.h>   // https://github.com/WereCatf/PCF8574_ESP
#endif

#ifndef AG_ExecPCF8574
#define AG_ExecPCF8574
#include <Arduino.h> // нужно для ардуино-функций

// ======================================== T E R M O S T A T . E X P A N D E R ======================================== //
  class ThermostatExpander : public PCF857x {
    public:
      ThermostatExpander(uint8_t address);   // address - адрес расширителя, active  - битовая маска уровней вкл/выкл устройств; cooler - кондей/вентилятор, для реле = 0
      //входные параметры --
      int8_t    min[8];
      int8_t    max[8];
      int8_t    thres[8];
      int8_t    critical[8];
      float     paramCurrent[8];
      bool      modeAuto[8];            // 1 - Авто вкл. , 0 - ручной режим
      bool      cmd[8];                 // 1 - вкл. ,      0 - выкл. Команда <- MQTT
      bool      state[8];               // 1 - вкл. ,      0 - выкл. Реальное состояние вывода контроллера -> MQTT
      char      stateChar[8][2] = {{'2','\0'}, {'3','\0'}, {'4','\0'}, {'5','\0'}, {'6','\0'}, {'7','\0'}, {'8','\0'}};
      char      cmdChar[8][2]   = {{'8','\0'}, {'7','\0'}, {'6','\0'}, {'5','\0'}, {'4','\0'}, {'3','\0'}, {'2','\0'}};
      //--------------------
      void      init(uint8_t active, uint8_t cooler);
      void      on(uint8_t _pin);
      void      off(uint8_t _pin);
      bool      getState(uint8_t _pin);
      bool      stateChanged(uint8_t _pin);
      bool      cmdChanged(uint8_t _pin);
      void      run(uint8_t _pin);
      void      runAuto(uint8_t _pin);
      void      runMin(uint8_t _pin);
      void      runMax(uint8_t _pin);
      void      runMinMax(uint8_t _pin);
      void      setParamCurrent(uint8_t _pin, float _param);
    private:
      bool      _active[8];
      bool      _cooler[8];              // 1 - Нагреватель 0 - охлаждатель (кондей)
      bool      _on[8], _off[8];
      bool      _stateOld[8];
      bool      _cmdOld[8];
      bool      _cmdChanged[8];
      bool      _stateChanged[8];
      uint32_t  _timeLast[8];
  };
  ///---------------------------------------------------------------///  

  #endif