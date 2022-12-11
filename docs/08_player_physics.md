### `playerPhysics : PlayerPhysics`
Object describing the player physics.
If omitted player default player physics will be used.

"playerPhysics" : {
	"bCoef" : 0.5,
	"invMass" : 0.5,
	"damping" : 0.96,
	"acceleration" : 0.1,
	"kickingAcceleration" : 0.07,
	"kickingDamping" : 0.96,
	"kickStrength" : 5
},

## PlayerPhysics
PlayerPhysics describes physical constants affecting the players.

### `gravity : float[]`
The gravity vector of the object in array form [x, y].

### `radius : float`
The radius of the disc.

### `invMass : float`
The inverse of the disc's mass.

### `bCoef : float`
The bouncing coefficient.

### `damping : float`
The damping factor of the disc.

### `cGroup : string[]`
A list of flags that represent this object's collision group.

### `acceleration : float`
How fast a player accelerates when moving around with his keys.

### `kickingAcceleration : float`
Replaces acceleration when the player is holding the kick button.

### `kickingDamping : float`
Replaces damping when the player is holding the kick button.

### `kickStrength : float`
How much force the player applies to the a ball when kicking.

### `kickback : float`
Much like kickStrength but it's applied to the kicking player instead.
