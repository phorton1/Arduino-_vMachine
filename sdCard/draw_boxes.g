g90                 ; absolute
g54                 ; machine coords
g0 z-70             ; pen up (absolute)
g4 p3               ; pause

; outer box, absolute machine coordinates

g0 x0 y0
g1 z-90 f10000      ; pen down (absolute)
g1 x400 f1000
g1 y300 f1000
g1 x0   f1000
g1 y0   f1000
g0 z-70             ; pen up (absolute)
g4 p3

; 12x9 box

g55
g0 x-10 y0
g1 z0 f10000        ; pen down (coord system)
g1 x314.8 f1000     ; end 12x9 x + 10
g0 z20              ; pen up

g0 x304.8 y-10      ; end 12x9, y-10
g1 z0 f10000        ; pen down (coord system)
g1 y238.6 f1000     ; end 12x9 x + 10
g0 z20              ; pen up

g0 x314.8 y228.6    ; end 12x9, y+10
g1 z0 f10000        ; pen down (coord system)
g1 x-10 f1000       ; end 12x9 x + 10
g0 z20              ; pen up

g0 x0 y238.6        ; start 12x9, y+10
g1 z0 f10000        ; pen down (coord system)
g1 y-10 f1000       ; end 12x9 x + 10
g0 z20              ; pen up

g4 p3


; 8x11.5 box

g56
g0 x-10 y0
g1 z0 f10000        ; pen down (coord system)
g1 x289.4 f1000     ; end 8.5x11 x + 10
g0 z20              ; pen up

g0 x279.4 y-10      ; end 8.5x11, y-10
g1 z0 f10000        ; pen down (coord system)
g1 y225.9 f1000     ; end 8.5x11 x + 10
g0 z20              ; pen up

g0 x289.4 y215.9    ; end 8.5x11, y+10
g1 z0 f10000        ; pen down (coord system)
g1 x-10 f1000       ; end 8.5x11 x + 10
g0 z20              ; pen up

g0 x0 y225.9        ; start 8.5x11, y+10
g1 z0 f10000        ; pen down (coord system)
g1 y-10 f1000       ; end 8.5x11 x + 10
g0 z20              ; pen up

g4 p3

g54                 ; machine coords
g0 x200 y150 z-70   ; centered pen up
