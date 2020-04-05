
#include "AG_ExecPCF8574.h"  
#include <PCF8574_esp.h>   

    // конструктор: на входе _pin - номер вывода, 
    //              active - логический уровень включения: 
    //              1 - (1-вкл., 0-выкл.), 0 - (1-выкл., 0-вкл.) 
    //              cooler - 1 кондиционер, 0 - нагреватель
    ThermostatExpander:: ThermostatExpander(uint8_t address) : PCF857x(address, &Wire) {
      PCF857x :: begin();
    }
    ///---------------------------------------------------------------///

    void ThermostatExpander:: init(uint8_t active, uint8_t cooler){
      for (uint8_t idx=0; idx<=7; idx++){
       // if ((active & (0b00000001<<idx))) _active[idx] = true;
       // else _active[idx] = false;
        
       _active[idx] =  (active & (0b00000001<<idx));
       if (_active[idx]) {
           _on[idx] =   true;
           _off[idx] =  false; }
        else {
            _off[idx] =  true; 
            _on[idx] =   false;
       }
       _cooler[idx] = (cooler & (0b00000001<<idx));
       modeAuto[idx] = true;
       paramCurrent[idx] = 777.0;  
       off(idx);
       delay(100);
       _stateOld[idx] = getState(idx);
       cmdChanged(idx);
      }
    }

    void ThermostatExpander:: on(uint8_t _pin){
     PCF857x :: write(_pin, _on[_pin]);
    }

    void ThermostatExpander:: off(uint8_t _pin){
     PCF857x :: write(_pin, _off[_pin]);
    }

    bool ThermostatExpander:: getState(uint8_t _pin) {
     if (_active[_pin]) {
        if (PCF857x :: read(_pin) != 0)  {state[_pin]=1; dtostrf((state[_pin]), 1, 0, stateChar[_pin]);}
       else                              {state[_pin]=0; dtostrf((state[_pin]), 1, 0, stateChar[_pin]);}
        }
      else {
       if (PCF857x :: read(_pin) != 0) {state[_pin]=0; dtostrf((state[_pin]), 1, 0, stateChar[_pin]);}
        else                           {state[_pin]=1; dtostrf((state[_pin]), 1, 0, stateChar[_pin]);}
     }
      return state[_pin];    
    }

    bool ThermostatExpander:: cmdChanged(uint8_t _pin){
      if (_cmdOld[_pin] != cmd[_pin]){
        _cmdOld[_pin]=cmd[_pin];
       _cmdChanged[_pin] = true;
     }
     else _cmdChanged[_pin] = false;
     return _cmdChanged[_pin];
    }

    // можно использовать вместо getState()
    bool ThermostatExpander:: stateChanged(uint8_t _pin){ 
    bool __state = getState(_pin); 
    if (__state != _stateOld[_pin]) {
        _stateOld[_pin] = __state;
        _stateChanged[_pin] = true;
    }
    else _stateChanged[_pin] = false;
    return _stateChanged[_pin];
    }

    // для нагревателя использовать обе функции: runMin(); runMax(); либо только runMin() для "Анти-замерзания"
    // runMinMax() для нагревателя включает "Анти-замерзание" сам по себе. 

    // run() - для реле
    void ThermostatExpander:: run(uint8_t _pin) {
      if (cmd[_pin]) {
        on(_pin);
      }
      else {
        off(_pin);
      }
    }

    void ThermostatExpander:: runAuto(uint8_t _pin){
      if (modeAuto[_pin]){
        if (cmd[_pin] == 1) {
            if (!_cooler[_pin]){
                if (paramCurrent[_pin] >= (float)max[_pin]) off(_pin);
                if (paramCurrent[_pin] <= (float)min[_pin]) on(_pin);  
            }
            else {
            if (paramCurrent[_pin] >= (float)max[_pin]) on(_pin);
            if (paramCurrent[_pin] <= (float)min[_pin]) off(_pin); 
            }
        }
        else {
            if (!_cooler[_pin]) runMin(_pin);
            else off(_pin);
        }
      }
      else{ }
    };

    // Только для нагревателя. Включ если темп < min, min+thres - отключается 
    void ThermostatExpander:: runMin(uint8_t _pin){       
      if (cmd[_pin] != 1) {
        if (paramCurrent[_pin] <= (float)(min[_pin])) on(_pin);
          if (paramCurrent[_pin] >= (float)(min[_pin]+thres[_pin])) off(_pin); 
      }
    };

  // нагрев:      включ. если темп <= max; отключ. темп >= max-thres
  // кондиционер: включ. если темп >= max; отключ. темп <= max-thres
    void ThermostatExpander:: runMax(uint8_t _pin){       
      if (cmd[_pin] == 1) {
        if (!_cooler[_pin]){
          if (paramCurrent[_pin] <= (float)(max[_pin]-thres[_pin])) on(_pin);
          if (paramCurrent[_pin] >= (float)(max[_pin])) off(_pin); 
        }
       else {
          if (paramCurrent[_pin] <= (float)(max[_pin]-thres[_pin])) off(_pin);
          if (paramCurrent[_pin] >= (float)(max[_pin])) on(_pin);      
        }
      }
    };

    // используется, в основном, для кондиционера и вентилятора
    // нагрев:      от min до max
    // кондиционер: включ. если темп >= max; отключ. темп <= min
    void ThermostatExpander:: runMinMax(uint8_t _pin){
      if (cmd[_pin] == 1) {
          if (!_cooler){
          if (paramCurrent[_pin] >= (float)max[_pin]) off(_pin);
          if (paramCurrent[_pin] <= (float)min[_pin]) on(_pin);  
        }
      else {
          if (paramCurrent[_pin] >= (float)max[_pin]) on(_pin);
          if (paramCurrent[_pin] <= (float)min[_pin]) off(_pin); 
        }
      }
      else {
        if (!_cooler[_pin]) runMin(_pin);
        else off(_pin);
      }
    };

    void ThermostatExpander:: setParamCurrent(uint8_t _pin, float _param){
        // перевод цифрового входного пераметра в символьный вид
    }


// ===================================== E N D T E R M O S T A T . E X P A N D E R ===================================== //
