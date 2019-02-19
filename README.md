# dynamometer
Electric winch two motor testing 

Two motors are coupled via a Gates belt.  One motor operates in torque mode and the other acts as a regenerative brake in speed mode.  The battery bank supplies power to make up for losses.  One load cells measure the belt tensioner cable tension and another load cell the torque from the motor that is braking, i.e. a Prony brake.

This project clones mnay of the routines developed in the 'mxusartusbcan' project, and uses STM32CubeMX with FreeRTOS on a DiscoveryF4 board connected to other hardware interfaces.
