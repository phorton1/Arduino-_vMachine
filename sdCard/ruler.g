; this gcode file prints a 200x160mm cross-hair ruler from the center
; of the 400x300 machine work area in absolute machine coordinates.
;
; It assumes that the machine is in absolute machine coordintes.
; This is either the case in a brand new machine, for which no other
; coordinate systems have been setup, or, by convention in our machine
; with "coordinate system 1", which corresponds to the G54 gcode, which
; is left as 0,0,0.  G55 in our machine is setup as a centered 12"x9" piece
; of paper, and G56 as a centered 11x8.5" piece of paper.


G54                 ; change to absolute 400x300 vMachine coordinate system

; In the default vMachine, the z-Axis servo varies from 0 degrees to -90 degrees,
; where -90 degrees is in contact with the paper (pen down), and -70 degrees is
; safely off the paper (pen up),   In the G55/G56 coordinate systems, 0 is defined
; as -90 degrees so that G0 Z0 is "pen down" and G0 Z20 is "pen up", but this file
; uses -70 and -90 for the z coordinates.

G0 z-70             ; pen up
G0 x200 y150        ; center
G4 P0.5             ; pause
G1 z-90 F10000      ; pen down
g91                 ; relative mode

; 80 mm up vertical from center
; first G1 line sets feed rate

G1 y10 F1000        ; up 10
G1 x10              ; right
G1 x-10             ; left

G1 y10
G1 x10
G1 x-10

G1 y10
G1 x10
G1 x-10

G1 y10
G1 x10
G1 x-10

G1 y10
G1 x10
G1 x-10

G1 y10
G1 x10
G1 x-10

G1 y10
G1 x10
G1 x-10

G1 y10
G1 x10
G1 x-10



; middle down

g90                 ; absolute mode
G0 z-70 F10000      ; pen up
G0 x200 y150        ; center
G4 P0.5             ; pause
G0 z-90 F10000      ; pen down
g91                 ; relative mode

G1 y-10 F1000       ; down 10
G1 x10
G1 x-10

G1 y-10
G1 x10
G1 x-10

G1 y-10
G1 x10
G1 x-10

G1 y-10
G1 x10
G1 x-10

G1 y-10
G1 x10
G1 x-10

G1 y-10
G1 x10
G1 x-10

G1 y-10
G1 x10
G1 x-10

G1 y-10
G1 x10
G1 x-10



; middle left

g90                 ; absolute mode
G0 z-70 F10000      ; pen up
G0 x200 y150        ; center
G4 P0.5             ; pause
G0 z-90 F10000      ; pen down
g91                 ; relative mode

G1 x-10 F1000       ; left 10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10

G1 x-10
G1 y10
G1 y-10


; middle right

g90                 ; absolute mode
G0 z-70 F10000      ; pen up
G0 x200 y150        ; center
G0 z-90 F10000      ; pen down
G4 P0.5             ; pause
g91                 ; relative mode


G1 x10 F1000        ; right 10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10

G1 x10
G1 y10
G1 y-10



; finished

g90                 ; absolute mode
G0 z-70 F10000      ; pen up
G0 x0 y300          ; top left corner
