#! /usr/bin/env python3

L_SHIFT = 0x2a
R_SHIFT = 0x36
BACKSPACE = 0x0e
TAB = 0x0f
L_ALT = 0x38
SPACE = 0x39
ENTER = 0x1c
L_CTRL = 0x1d

qwerty= {
    '1': (0x02,),
    '2': (0x03,),
    '3': (0x04,),
    '4': (0x05,),
    '5': (0x06,),
    '6': (0x07,),
    '7': (0x08,),
    '8': (0x09,),
    '9': (0x0a,),
    '0': (0x0b,),
    'q': (0x10,),
    'w': (0x11,),
    'e': (0x12,),
    'r': (0x13,),
    't': (0x14,),
    'y': (0x15,),
    'u': (0x16,),
    'i': (0x17,),
    'o': (0x18,),
    'p': (0x19,),
    'a': (0x1e,),
    's': (0x1f,),
    'd': (0x20,),
    'f': (0x21,),
    'g': (0x22,),
    'h': (0x23,),
    'j': (0x24,),
    'k': (0x25,),
    'l': (0x26,),
    'z': (0x2c,),
    'x': (0x2D,),
    'c': (0x2E,),
    'v': (0x2F,),
    'b': (0x30,),
    'n': (0x31,),
    'm': (0x32,),
    'Q': (L_SHIFT,0x10,),
    'W': (L_SHIFT,0x11,),
    'E': (L_SHIFT,0x12,),
    'R': (L_SHIFT,0x13,),
    'T': (L_SHIFT,0x14,),
    'Y': (L_SHIFT,0x15,),
    'U': (L_SHIFT,0x16,),
    'I': (L_SHIFT,0x17,),
    'O': (L_SHIFT,0x18,),
    'P': (L_SHIFT,0x19,),
    'A': (L_SHIFT,0x1e,),
    'S': (L_SHIFT,0x1f,),
    'D': (L_SHIFT,0x20,),
    'F': (L_SHIFT,0x21,),
    'G': (L_SHIFT,0x22,),
    'H': (L_SHIFT,0x23,),
    'J': (L_SHIFT,0x24,),
    'K': (L_SHIFT,0x25,),
    'L': (L_SHIFT,0x26,),
    'Z': (L_SHIFT,0x2c,),
    'X': (L_SHIFT,0x2D,),
    'C': (L_SHIFT,0x2E,),
    'V': (L_SHIFT,0x2F,),
    'B': (L_SHIFT,0x30,),
    'N': (L_SHIFT,0x31,),
    'M': (L_SHIFT,0x32,),
    ' ': (0x39,),
    '1': (0x02,),
    '2': (0x03,),
    '3': (0x04,),
    '4': (0x05,),
    '5': (0x06,),
    '6': (0x07,),
    '7': (0x08,),
    '8': (0x09,),
    '9': (0x0a,),
    '0': (0x0b,),
    '-': (0x0c,),
    '=': (0x0d,),
    '!': (L_SHIFT,0x02,),
    '@': (L_SHIFT,0x03,),
    '#': (L_SHIFT,0x04,),
    '$': (L_SHIFT,0x05,),
    '%': (L_SHIFT,0x06,),
    '^': (L_SHIFT,0x07,),
    '&': (L_SHIFT,0x08,),
    '*': (L_SHIFT,0x09,),
    '(': (L_SHIFT,0x0a,),
    ')': (L_SHIFT,0x0b,),
    '_': (L_SHIFT,0x0c,),
    '+': (L_SHIFT,0x0d,),
    '[': (0x1a,),
    ']': (0x1b,),
    '{': (L_SHIFT,0x1a,),
    '}': (L_SHIFT,0x1b,),
    ';': (0x27,),
    '\'': (0x28,),
    ':': (L_SHIFT,0x27,),
    '"': (L_SHIFT,0x28,),
    '`': (0x29,),
    '~': (L_SHIFT,0x29,),
    '\\': (0x2b,),
    '|': (L_SHIFT,0x2b,),
    ',': (0x33,),
    '.': (0x34,),
    '/': (0x35,),
    '<': (L_SHIFT,0x33,),
    '>': (L_SHIFT,0x34,),
    '?': (L_SHIFT,0x35,),
    
}


def press(c,l):

    hex_arr = []
    
    for val in l[c]:
        hex_arr.append(val)

    return hex_arr


def release(c,l):

    hex_arr = []
    
    for val in reversed(l[c]):
        hex_arr.append(val|0x80)

    return hex_arr


def type_in(c,l):
    hex_arr = []
    hex_arr.extend(press(c,l))
    hex_arr.extend(release(c,l))
    return hex_arr
