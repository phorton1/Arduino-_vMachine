; draw the defult 8x11 paper square
; set G56 to 8.5x11 area

g10 L2 p3 x60.3 y42.05 z0

g0 z20              ; safe position
g56                 ; turn on coordinate system
g0 x0 y0            ; go to Left Bottom (0,0)
g1 z0 f1000         ; pen down

g91                 ; draw hash marks relative
x-10
x10
y-10
y10
g90

x279.4 f1000        ; Right bottom
g91                 ; draw hash marks relative
x10
x-10
y-10
y10
g90

y215.9              ; Right Top
g91                 ; draw hash marks relative
x10
x-10
y10
y-10
g90

X0                  ; Left Top
g91                 ; draw hash marks relative
x-10
x10
y10
y-10
g90

y0                  ; back to Left Bottom (0,0)
g54                 ; back to default coordinate system
g0 z20              ; pen up
