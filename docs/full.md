# HaxBall Stadium File (.hbs)

A HaxBall stadium file is a text file with json5 format which contains a single StadiumObject.



## StadiumObject

StadiumObjects are the root object of a stadium file.


### `name : string`

The name of the stadium.


### `cameraWidth : float`

The width of a rectangle centered in coordinates <0,0> in which the camera will be contained. The camera follows the player and ball until it reaches the bounds of this rectanngle.


### `cameraHeight : float`

The height of a rectangle centered in coordinates <0,0> in which the camera will be contained. The camera follows the player and ball until it reaches the bounds of this rectanngle.


### `maxViewWidth : float`

The maximum allowed width viewable width for the level. If the player screen were wide enough for him to see more width than maxViewWidth then the the game will zoom in to prevent that.

Setting maxViewWidth to 0 disables this feature.

Default value: 0


### `cameraFollow : string`

Changes the camera following behaviour.

If set to "player" the camera will follow the player only, ignoring the ball.

If set to "ball" the camera will follow the average position between the player and the ball, the camera will prioritize the player in case player and ball are too far apart.

Default value: "ball"


### `spawnDistance : float`

The distance from <0,0> at which the teams will spawn during kickoff. This value is ignored if `redSpawnPoints` or `blueSpawnPoints` are not empty.


### `canBeStored : Boolean`

This value defines whether this stadium can be stored with the /store command.

Default value: true


### `kickOffReset : String`

Can be set to either `"full"` or `"partial"`.

When set to `"partial"` only the ball and player discs are reset for the kickoff.

When set to `"full"` all discs will be reset for the kickoff.

Default value: `"partial"`


### `bg : BackgroundObject`

An object describing the background for the stadium.


### `traits : TraitsObject`

A map of traits. See TraitsObject documentation for an explanation of usage.

### `vertexes : Vertex[]`

List of vertexes.

### `segments : Segment[]`

List of segments.

### `goals : Goal[]`

List of goals.

### `discs : Disc[]`

List of discs.

### `planes : Plane[]`

List of planes.

### `joints : Joint[]`

List of joints.


### `redSpawnPoints : float[][]`
List of spawn points used for the red team kickoff.

If the list is empty then the default spawn behaviour is employed.

When a player is moved into a team after a game has started he will be positioned in the last point in this list. (Unless the list is empty in which case the old spawn behaviour is employed)

Example: `"redSpawnPoints" : [[100, 0], [100, 30], [100, -30], [100, 60], [100, -60], [130,0]]`

Default value: `[]`


### `blueSpawnPoints : float[][]`
Same as `redSpawnPoints` but for the blue team.


### `playerPhysics : PlayerPhysics`

Object describing the player physics.

If omitted player default player physics will be used.

### `ballPhysics : Disc`

The Disc used to create the ball. The collision flags "kick" and "score" will be automatically added.

Setting ballPhysics to the string "disc0" will instead use the first disc as the ball. In this case the cGroup will be left unmodified.

If omitted default ball physics will be used.


***

## BackgroundObject

### `type : string`

The type of background to use for the stadium. Possible values are "grass", "hockey", and "none".

Default value: "none"

### `width : float`

Width of the background rectangle.

Default value: 0

### `height : float`

Height of the background rectangle.

Default value : 0

### `kickOffRadius : float`

Radius of the kickoff circle.

Default value: 0

### `cornerRadius : float`

Radius of the corners of the circle (creates rounded corners if > 0)

Default value: 0

### `goalLine : float`

Horizontal distance to the goals from position <0,0>, used by "hockey" background only.

Default value: 0

### `color : Color`

Background color for the stadium.

Default value: "718C5A"


***

## Vertex

A vertex is a point which can collide with discs but cannot move and is not visible.

### `x : float`
The x position for the vertex.
### `y : float`
The y position for the vertex.
### `bCoef : float`
The bouncing coefficient.
### `cMask : string[]`
A list of flags that represent this object's collision mask.

[Read more about collision flags here](https://github.com/haxball/haxball-issues/wiki/Collision-Flags).
### `cGroup : string[]`
A list of flags that represent this object's collision group.

[Read more about collision flags here](https://github.com/haxball/haxball-issues/wiki/Collision-Flags).


***

## Segment

A segment is a line (curved or straight) that connects two vertexes. Discs can collide with segments and they can also be used as decoration.

### `v0 : int`

Index of a vertex in the stadium vertex list to be used as first point of the segment.

### `v1 : int`

Index of a vertex in the stadium vertex list to be used as the second point of the segment.

### `bCoef : float`
The bouncing coefficient.

### `curve : float`
The angle in degrees with which the segment will curve forming an arc between it's two vertexes.

Default value is 0 which makes the segment a straight line.

### `curveF : float`
Alternative representation of the segment's curve. If this value is present the `curve` value will be ignored.

This value is only useful for exporting stadiums without precision loss, it is recommended to remove `curveF` and use only `curve` when editing an exported stadium.

### `bias : float`

If set to 0 the segment will collide normally on both sides.

If greater or lower than 0 the bias determines the thickness of the segment. The segment also becomes a one-way segment that collides only in one of it's sides.

This property can be useful to create boundaries that small and fast moving balls are unable to pass through.

Default value: 0

### `cMask : string[]`
A list of flags that represent this object's collision mask.

[Read more about collision flags here](https://github.com/haxball/haxball-issues/wiki/Collision-Flags).
### `cGroup : string[]`
A list of flags that represent this object's collision group.

[Read more about collision flags here](https://github.com/haxball/haxball-issues/wiki/Collision-Flags).

### `vis : bool`

If set to false the segment will be invisible.

Default value: true

### `color : Color`

The color with which the segment will be drawn.

Default value: "000000" (black)


***

## Goal

Goals are lines belonging to a team, when the ball crosses this line the opossite team scores a goal.

### `p0 : float[]`

The coordinates of the fist point of the line in an array form [x, y].

### `p1 : float[]`

The coordinates of the second point of the line in an array form [x, y].

### `team : string`

The team the goal belongs to. Possible values: "red" or "blue"


***

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

[Read more about collision flags here](https://github.com/haxball/haxball-issues/wiki/Collision-Flags).
### `cGroup : string[]`
A list of flags that represent this object's collision group.

[Read more about collision flags here](https://github.com/haxball/haxball-issues/wiki/Collision-Flags).


***

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

[Read more about collision flags here](https://github.com/haxball/haxball-issues/wiki/Collision-Flags).
### `cGroup : string[]`
A list of flags that represent this object's collision group.

[Read more about collision flags here](https://github.com/haxball/haxball-issues/wiki/Collision-Flags).


***

## PlayerPhysics

PlayerPhysics describes physical constants affecting the players.

### `gravity : float[]`
See Disc.gravity
### `radius : float`
See Disc.radius
### `invMass : float`
See Disc.invMAss
### `bCoef : float`
See Disc.bCoef
### `damping : float`
See Disc.damping
### `cGroup : string[]`
See Disc.cGroup

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


***

## Joint
Joints are physical connections between two Discs.

### `d0 : Int`
Index of one of the two discs connected by the joint. 

Note: Index 0 will be used by the ball disc if `StadiumObject.ballPhysics` is not set to `"disc0"`.

### `d1 : Int`
Index of one of the two discs connected by the joint. 

Note: Index 0 will be used by the ball disc if `StadiumObject.ballPhysics` is not set to `"disc0"`.

### `length : float | [min, max] | null`
If set to null then the length will be automatically computed to match the distance between the two discs connected by the joint.

If set to a float value then the join will use that as length.

If set to a 2 elements array [min, max] then the joint will have a min length and max length. The joint will apply no forces if the distance between the discs is inside of that range.

Default value: `null`

### `strength : float | "rigid"`
If set to `"rigid"` then the joint acts like a solid.

If set to a float value then the joint will act like a spring.

Default value: `"rigid"`

### `color : Color`
The color of the joint. Supports "transparent" color.

Default value: `"000000"` (black)

***

## Color

A color can be either `"transparent"`, a string with hex values `"RRGGBB"` or an array of integer values `[Red,Green,Blue]`.

Examples:

```json
"color": "transparent" // Make color invisible / transparent

"color": "FF0000" // Make color red

"color": [0,200,0] // Make color green.
```
