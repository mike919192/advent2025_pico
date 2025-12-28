New Project
Select options
 - Board type: Pico
 - Console over USB
 - Generate C++ code

input files are modified to have 1 SUB byte at the end of them

Serial configuration is setup in putty before hand.
COM4 and 115200 baud
Saved as pi_pico

Run the embedded program so that it is waiting at scanf

"C:\Program Files\PuTTY\plink.exe" pi_pico < C:\repos\advent2025_pico\day1\input.txt