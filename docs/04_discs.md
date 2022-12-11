### `discs : Disc[]`
List of discs.



## Disc

Discs are circular physical objects that are placed in the stadium, they can move and collide with other discs.

### `pos : float[]`
The starting position of the object in array form [x, y].

### `speed : float[]`
The starting speed of the object in array form [x, y].

### `gravity : float[]`
The gravity vector of the object in array form [x, y].

### `radius : float`
The radius of the disc.

### `invMass : float`
The inverse of the disc's mass.

### `damping : float`
The damping factor of the disc.

### `color : Color`
The disc fill color. Supports "transparent" color.
Default value: `"FFFFFF"`

### `bCoef : float`
The bouncing coefficient.

### `cMask : string[]`
A list of flags that represent this object's collision mask.

### `cGroup : string[]`
A list of flags that represent this object's collision group.
