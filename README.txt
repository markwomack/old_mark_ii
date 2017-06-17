RobotBase is a base platform Arduino sketch that I can use to program my robot.
My current robot is using an Arduino Motor Shield to drive the motors, and that
is the pin configuration that I have defined.  You may need to modify it for
your own motor shield set up.  I also have an HC-SR04 ultrasonic sensor that
is used to detect the distance to the nearest obstacle in front of the robot.
I am using the distance measurement with the current state of the motors to
determine behavior of the robot.  Right now it is programmed to avoid obstacles
closer that 30cm by spinning left until the obstacle is greater than 30cm.

You can use RobotBase to program your own robot behavior.  You can add more or
different sensors, each with their own input, to drive the behavior or the
robot.

Code by Mark Womack
https://codebender.cc/sketch:144189

-----
ButtonExecutor is a class that allows you to develop a sketch that has its
execution controlled by a push button switch.  Once the sketch is loaded into
the Arduino execution will of the sketch code will not start until the push
button is pressed.  Execution is ended when the push button is again pressed.
The state of the sketch can be reset and execution started again with the next
button press.

More details on ButtonExecutor can be found at the codebender link below.

Code by Mark Womack
https://codebender.cc/sketch:142912
 
-----
Timer and Event classes are forks of original classes by Simon Monk.  I have
included them in this project for completeness, but I did not modify them from
their original forked versions.
Information on forked version:
https://github.com/JChristensen/Timer

This library was originally developed by Dr. Simon Monk to avoid problems that
arise when you start trying to do much inside the `loop()` function.  It can
change pin values or run a callback function.  See Dr. Monk's original page 
[here](http://www.doctormonk.com/2012/01/arduino-timer-library.html)).

The library does not interfere with the built-in timers, it just uses 
`millis()` in a basic type of scheduler to decide when something needs doing.

-----
SerialDebug and Cascade classes are forks of the original classes by Alexander
Brevig and David A. Mellis.  I have included them for completeness and I only
modified them so they would work with Arduino 1.0.
