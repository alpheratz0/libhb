### `planes : Plane[]`
List of planes.



## Plane
Planes are collision objects that divide the map in two by an infinite line. They are useful for creating the boundaries of the stadium.

### `normal : float[]`
The direction vector of the plane in an array form [x, y].

### `dist : float`
The distance from coordinates [0,0] (in direction of the normal) in which the plane is located at.

### `bCoef : float`
The bouncing coefficient.

### `cMask : string[]`
A list of flags that represent this object's collision mask.

### `cGroup : string[]`
A list of flags that represent this object's collision group.
