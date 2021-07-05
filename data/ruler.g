G0 z20 F10000       ; pen up
G0 x200 y150        ; center
G0 z3 F10000        ; pen down
G4 P0.5             ; pause
g91                 ; relative mode

; vertical from center
; first G1 line sets feed rate

G1 x0 y10 F1000
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0

G1 x0 y10
G1 x10 y0
G1 x-10 y0


; middle down

g90                 ; absolute mode
G0 z20 F10000       ; pen up
G0 x200 y150        ; center
G0 z3 F10000        ; pen down
G4 P0.5             ; pause
g91                 ; relative mode

G1 x0 y-10  F1000
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0

G1 x0 y-10
G1 x10 y0
G1 x-10 y0


; middle left

g90                 ; absolute mode
G0 z20 F10000       ; pen up
G0 x200 y150        ; center
G0 z3 F10000        ; pen down
G4 P0.5             ; pause
g91                 ; relative mode


G1 x-10 y0  F1000
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10

G1 x-10 y0
G1 x0 y10
G1 x0 y-10




; middle right

g90                 ; absolute mode
G0 z20 F10000       ; pen up
G0 x200 y150        ; center
G0 z3 F10000        ; pen down
G4 P0.5             ; pause
g91                 ; relative mode


G1 x10 y0  F1000
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

G1 x10 y0
G1 x0 y10
G1 x0 y-10

; box it fast

G4 P0.5             ; pause
G1 Y100  F3000
G1 X-240
G1 Y-200
G1 X240
G1 Y100

; finished

g90                 ; absolute mode
G0 z20 F10000       ; pen up
G0 x200 y150        ; center
