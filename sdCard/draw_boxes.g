; A script to draw three boxes on the fresh work area once everything is working
;
; - outline the 400x300 mm work area
; - draw the inner 12x9" page size with hash marks
; - draw the inner 11x8.5 page size with hash marks
;
; THIS SCRIPT ASSUMES YOU HAVE SETUP the gmachine coordinates systems 1 thru 3
; which correspond to the gcodes G54, G55, and G56 respectively
;
;   G54 - is left unchanged (0,0,0) and used to return the machine to "absolute coordinates"
;   G55 - sets 0,0 to the bottom left corner of a centered piece of 12x9" paper
;   G56 - sets 0,0 to the bottom left corner of a centered piece of 11x8/5" paper
;
;   In G55 and G56 we also set zZero to "-90" degrees, as that is the location of
;   "pen down" on the RC Servo.
;
; These coordinate systems are setup on a calibrated and working vMachine with the
; default configuration, by issuing the following commands:
;
;       g10 L2 p2 x47.6 y35.7 z-90          ; setup 12x9 coordinate system on G55
;       g10 L2 p3 x60.3 y42.05 z-90         ; setup 11x8.5" coordinate system on G56
;
; When these commands have been issued the following command could be used
; to take you to the upper right corner of each page (the dimensions of
; a 12x9" and 11x8.5" piece of paper, in mm, respectively):
;
;       g0 x304.8 y228.6                    ; go to upper right corner of 12x9" page in G55 mode
;       g0 x279.4 y215.9                    ; go to upper right corner of 11x8.5" page in G56 mode


g90                 ; absolute
g54                 ; machine coords
g0 z-70             ; pen up (absolute)
g4 p1               ; pause

; outer box, absolute machine coordinates

g0 x0 y0
g1 z-90 f10000      ; pen down (absolute)
g1 x400 f1400       ; right
g1 y300 f1400       ; up
g1 x0   f1400       ; left
g1 y0   f1400       ; down
g0 z-70             ; pen up (absolute)
g4 p1               ; pause

; 12x9" box

g55                 ; switch to 12x9" coordinate system

g0 x-10 y0          ; start hash mark to left of box
g1 z0 f10000        ; pen down (coord system)
g1 x314.8 f1400     ; end 12x9 x + 10
g0 z20              ; pen up

g0 x304.8 y-10      ; end 12x9, y-10
g1 z0 f10000        ; pen down (coord system)
g1 y238.6 f1400     ; end 12x9 x + 10
g0 z20              ; pen up

g0 x314.8 y228.6    ; end 12x9, y+10
g1 z0 f10000        ; pen down (coord system)
g1 x-10 f1400       ; end 12x9 x + 10
g0 z20              ; pen up

g0 x0 y238.6        ; start 12x9, y+10
g1 z0 f10000        ; pen down (coord system)
g1 y-10 f1400       ; end 12x9 x + 10
g0 z20              ; pen up

g4 p1               ; pause


; 11.5x8" box

g56                 ; 11x8.5 coordinate system
g0 x-10 y0
g1 z0 f10000        ; pen down (coord system)
g1 x289.4 f1400     ; end 8.5x11 x + 10
g0 z20              ; pen up

g0 x279.4 y-10      ; end 8.5x11, y-10
g1 z0 f10000        ; pen down (coord system)
g1 y225.9 f1400     ; end 8.5x11 x + 10
g0 z20              ; pen up

g0 x289.4 y215.9    ; end 8.5x11, y+10
g1 z0 f10000        ; pen down (coord system)
g1 x-10 f1400       ; end 8.5x11 x + 10
g0 z20              ; pen up

g0 x0 y225.9        ; start 8.5x11, y+10
g1 z0 f10000        ; pen down (coord system)
g1 y-10 f1400       ; end 8.5x11 x + 10
g0 z20              ; pen up

g4 p1               ; pause

g54                 ; machine coords
g0 x0 y300          ; move out of way (upper left of work area)
