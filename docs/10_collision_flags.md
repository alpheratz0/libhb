Collision flags are what haxball's physics uses to determine which objects collide and which objects ignore each other.

Each flag represents a group or category.

## Flags
### ball
This is the default collision group of the ball.
### red
This layer is automatically added to players of the red team.
### blue
This layer is automatically added to players of the blue team.
### redKO
This layer represents kickoff barriers that become active during kickOff for the red team.
### blueKO
This layer represents kickoff barriers that become active during kickOff for the blue team.
### wall
This is the default collision group for vertexes segments and planes.
### all
Represents a set including `ball`, `red`, `blue`, `redKO`, `blueKO` and `wall`
### kick
Objects with this flag in their `cGroup` will become kickable by the players.
### score
Objects with this flag in their `cGroup` will score goals if they cross a goal line.
### c0
Has no special meaning and can be used for any purpose
### c1
Has no special meaning and can be used for any purpose
### c2
Has no special meaning and can be used for any purpose
### c3
Has no special meaning and can be used for any purpose


***

## cGroup and cMask
Physical objects like discs vertexes segments and planes have two sets of flags: cGroup and cMask.

### cGroup
cGroup stands for collision group. With this property an object expresses which groups it belongs to.
For example, normally players belong to the "blue" or "red" groups while segments and vertexes use the "wall" group by default.

### cMask
cMask stands for collision mask. This property expresses which groups an object can collide with. Usually this is set to "all" meaning it can collide with almost any other object.

### Collision logic

Object A and B will collide if and only if the intersection of A.cGroup and B.cMask is not empty **AND** the intersection of B.cGroup and A.cMask is not empty.

Haxball expresses collision groups and mask as [Bit Fields](https://en.wikipedia.org/wiki/Bit_field) in the Headless Host API.

In javascript the collision logic can be expressed as:
```js
(objA.cGroup & objB.cMask!= 0) && (objB.cGroup & objA.cMask != 0)
```
Do not confuse `&` operator with `&&`, the former is a bitwise operator while the latter is boolean.

***

## Player disc cMask
Player's cMask is always set to `ball`, `red`, `blue` and `wall`.

During kickOff players will also have `redKO` or `blueKO` in their cMask depending on which team the kickoff belongs to.

It is not possible to modify a player's cMask (but it is possible to modify it's cGroup)


***

## Stadium (.hbs) File Collision Flags

In .hbs files collision flags are defined as an array of strings where each string is the name of a flag.

Example: `"cMask": ["red", "blue", "redKO", "ball", "c1"]`

The `all` flag can be combined with other flags to form an union. Example: `"cMask": ["all", "c0", "c2"]`.