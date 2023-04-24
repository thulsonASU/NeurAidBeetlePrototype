# Project Overview
The goal of this project repository is to showcase the Beetle Arduino microcontroller logic that was used to demonstrate a Minimal Viable Product for ASU team NeurAid in partnership with the Barrow Neurological Institute. <br />
<img src="./images/asu_fultonengineering_horiz_rgb_maroongold_600ppi.png"  width="40%" height="40%">
<img src="./images/Barrow-Logo.png"  width="35%" height="35%"> <br />

## Project Requirements
<br /> **Functional Requirements** <br />
- As for technical functionality, the device must capture the user's voice intensity and
compute the error of that intensity to the ambient environment to gauge an appropriate
target level for the user to reach when communicating with others.
- The device will have a sleep mode where the vibratory motor does not initialize unless
it detects the user attempting to speak. Upon detection of the user's voice, it will
compute the error in real time and vibrate rapidly when the user's voice is low to
encourage a louder response and softly when the user's voice is loud to encourage a
quieter response.
- Users can adjust the system's haptic sensitivity. The system must capture ambient noise
and the user's vocal intensity. The system must compare ambient noise to user vocal
intensity to determine appropriate haptic feedback.

<br /> **Performance requirements** <br />

- Response time: The delay between sensing the audio and output feedback must be as
small as possible (in the order of microseconds). Huge delays cause confusion and false
interpretation for the user.
- Filtering: The background noise must be filtered properly to separate the audio signal
from the noise.
- Adaptability: The device must be capable of adapting to different surroundings and
adjusting its sensitivity according to those surroundings.
- Weight: The prototype must be light, as the user must be able to wear it for longer
periods. The size of the prototype can be reduced by optimizing the design parameters
without compromising the basic functionality of the system.
- Appearance and Visibility: The device must blend in with the body and stay hidden
under the user’s clothes.
- Communication: The data communication between components must be fast and
error-free.
- Protection: The device must be protected from dust and liquids as per the IP54
standard.

<br /> **Usability Requirements** <br />
- Discreteness Requirement: The device should be discrete, allowing the user to
maintain privacy and avoid unwanted attention.
- Justification: If the device is too noticeable (i.e., a hearing aid), the user might avoid
wearing it due to its appearance. Having a discrete product will allow the user to wear
it comfortably and not have to worry about unwanted attention while gaining the
benefits of the device.
- Accessibility Requirement: The device should be accessible for users with
disabilities, including those with motor and auditory impairments.
- Justification: Individuals diagnosed with Parkinson's disease commonly experience a
symptom that affects their fine motor control. As a result, the device must be designed
to be accessible and easy to use to accommodate these symptoms.

## Prototype Board
The scope of the prototype board was simply to demonstrate all functionality for the the project in a compact protoboard configuration. this would assist in getting a better grasp on the final boards size and placement for the housing. By developing this board the team also achieved a better understanding on how these two microphones function and can leverage that knowledge for the final prototype. <br />
<br />
Before jumping into board construction, every component was tested individually as a subsystem. Once that was completed, a rough schematic was drawn up and verified before proceeding with board fabrication. <br />
<img src="./images/breadboard.png"  width="80%" height="80%"> <br />
<img src="./images/rough_schematic.png"  width="80%" height="80%"> <br />
<img src="./images/mvp_board.png"  width="80%" height="80%"> <br />

## Component Breakdown
<br /> **Motor Subsystem** <br />
- 1x ZVP2106A MOSFET
- 1x 1N4007 Silicon Rectifier Diode
- 1x 100 Ohm Resistor
- 1x 104 Ceramic Capacitor

<br /> **Button Subsystem** <br />
- 2x 10k Ohm Resistors in Pull Down Configuration
- 2x mini push buttons
- 2x Jumper wires

<br /> **Power Supply Subsystem** <br />
- 1x LM7805 Linear Voltage Regulator (5v Output)
- 2x 9v DC Batteries
- 1x 106 Ceramic Capacitor
- 1x 104 Ceramic Capacitor
- 1x 2-pin Screw Terminal
- 2x 9v Battery Connectors

<br /> **Power LED Subsystem** <br />
- 1x 220 Ohm Resistor
- 1x Red Light Emitting Diode

<br /> **MAX4466 Microphone Subsystem** <br />
- 1x MAX4466 Microphone Module
- 1x 104 Ceramic Capacitor
- 1x 3-pin female header
- 3x Male female jumper wires

<br /> **MAX9814 Microphone Subsystem** <br />
- 1x MAX9814 Microphone Module
- 1x 5-pin female header
- 3x Male female jumper wires

# Code Breakdown


# How to Use

