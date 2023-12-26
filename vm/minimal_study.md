# Minimal Forth proposed word set
## Word creation
CREATE		-- primitive nh (from name)
FIND			-- primitive nf 
GET-PREV	-- primitive np
SET-PREV	-- primitive nP
GET-XT		-- primitive nx
SET-XT		-- primitive nX
GET-FLAGS	-- primitive nf
SET-FLAGS	-- primitive nF
GET-NAME	-- primitive nn
SET-NAME	-- primitive nN
## Memory Access
!					-- primitive ;
C!				-- primitive ,
,	
C,
@					-- primitive :
C@				-- primitive .
ALIGN
CALIGN
ALIGNED		-- primitive g
CALIGNED
CELL+
CHAR+
CELLS
CHARS
### Alternatives
CELL			-- primitive c
HERE			-- primitive h
## Arithmetic
+					-- primitive +
-					-- primitive -
*					-- primitive *
/					-- primitive /
2*
2/ 
*/MOD 
MOD				-- primitive %
## Logic
0=				-- primitive 0
=					-- primitive =
<					-- primitive <
>					-- primitive >
AND				-- primitive &
OR				-- primitive |
INVERT		-- primitive ~
XOR				-- primitive ^
TRUE
FALSE
LSHIFT
RSHIFT
## Stack
DUP				-- primitive d
DROP			-- primitive _
SWAP			-- primitive s
OVER			-- primitive o
>R				-- primitive (
R>				-- primitive )
R@				-- primitive @
ROT				-- primitive r
### Alternatives
SP@				-- primitive xs
SP!				-- primitive xS
RP@				-- primitive xr
RP!				-- primitive xR
## Flow control
IF
ELSE
THEN
BEGIN
WHILE
AGAIN
REPEAT
UNTIL
DO
LOOP
I
J
'					-- primitive?
EXECUTE		-- primitive i / x
### Alternatives
block			-- primitive { ... }
choice		-- primitive ?
times			-- primitive t
while			-- primitive w
EXIT			-- primitive ]
BRANCH		-- primitive j
?BRANCH		-- primitive z
## Definitions
:					-- ASSEMBLER 
;					-- ASSEMBLER
CONSTANT
VARIABLE
CREATE		-- primitive
DOES>
## Device
KEY				-- EXTENSION K
KEY?			-- EXTENSION?
EMIT			-- EXTENSION E
CR				-- ?
## Tools
(
\
.S
