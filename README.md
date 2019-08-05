# Avionic Email Counter

A functional art project consisting of a Boeing (Teledyne Controls P/N 65003-005) Turbine Gas Temperature gauge that I have reverse engineered to display the number of unread emails, followed by total number of emails, in an Outlook profile.

The number of unread emails are shown on the dial indicator, 0 through a max of 10. The number of total email items in an inbox are shown on the 3 digit 7 segment display. The example image below shows 8 unread emails and 982 total emails.

![Indicator Dial](https://raw.githubusercontent.com/hshutan/Avionic-Email-Counter/master/TGT-OutlookSerial/tgtdial.jpg)

This project has two components. The first is the avionics display itself, using the original stepper motor and original shift register driven 7 segment displays.
The OEM stepper motor and shift registers are driven by an Arduino listening for serial commands, causing adjustments to the dial indicator (servo) and 7 segment displays.

The second component is a Windows C# application that uses Microsoft's Outlook interop to read the number of unread items, as well as the number of total items, in an Outlook profile's inbox folder. This C# program then sends those two numbers via serial to the Arduino driven avionic display.
No sensitive Outlook profile data or information is transferred, only simple numbers.
The Arduino avionic display can be used on its own via serial, without the Windows C# component.

The Arduino driven TGT avionic display listens for serial command strings that look like ``8,982.``
In this instance, the Arduino will then set the dial indicator to point to "8" and set the 7 segment displays to show "982" as seen in the example image above.

The graphical portion of the Windows C# software is simple, it allows for selection of COM port, followed by start, stop, and reset functionality.

![Windows Application](https://raw.githubusercontent.com/hshutan/Avionic-Email-Counter/master/TGT-OutlookSerial/ss.png)
