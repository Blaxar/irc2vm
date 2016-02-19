# irc2vm
An irc bot controlling a VirtualBox by translating chat commands to mouse and keyboard events.

# Commands


**TYPE sometext** : Type the given text following the command.

**MOUSEMOVE x y** : Move the mouse to a relative position expressed in pixels, x pixels on the right, y pixels to the bottom (works with negatives values too).

**MOUSESET x y** : Move the mouse to a absolute position expressed in pixels, x on the abscissa, y pixels on the ordinate (origin is top-left corner of the screen).

**LC** : Left mouse button click.

**RC** : Right mouse button click.

**MC** : Middle mouse button click.

**DC** : Left mouse button double-click.

**ML** : Move mouse slightly to the left.

**MR** : Move mouse slightly to the right.

**MU** : Move mouse slightly up.

**MD** : Move mouse slightly down.

**MLL** : Move mouse to the left.

**MRR** : Move mouse to the right.

**MUU** : Move mouse up.

**MDD** : Move mouse down.

**MHOLD** : Hold left mouse button.

**MRELEASE** : Release left mouse button.

**SHOLD** : Hold left shift key.

**SRELEASE** : Release left shift key.

**AHOLD** : Hold left Alt key.

**ARELEASE** : Release left Alt key.

**CHOLD** : Hold left Ctrl key.

**CRELEASE** : Release left Ctrl key.

**ENTER** : Press Enter once.

**SPACE** : Press Space once.

**RET** [**x**] : Press Backspace x times, if x is missing, press it only once.
