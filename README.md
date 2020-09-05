# Welcome
Welcome to my little project 

## TODO
 - Finnish doccumentation
 - Add WiringPi to cmake (if needed)
 - Implement a lock mechanism for emergency stop
 - change ON/OFF command on axis 5

 ## SoftPWM

SoftPWM is an alternative to PWM, where the hardware does not support PWM 
functionallity on that pin. SoftPWM is slower, but offers more flexibility.

SoftPWM is enabled from the [CMakeLists.txt](./CMakeLists.txt) file:

```
 add_compile_definitions(SOFTPWM)
```
If you would like to disable SoftPWM, you can comment out the line:

```
# add_compile_definitions(SOFTPWM)
```

Please note that in order to use hardware PWM, the pin should support
this functionallity. The easiest way to see if there are such pins is 
to run `gpio readall` and see which pins have "PWM" in their name.
