" Vim syntax file
" Language: Hylas Lisp
" Maintainer: Eudoxia
" Latest Revision: 18 September 2012

if exists("b:current_syntax")
  finish
endif

"Match constants

syn match Integer "[-+]?\d\+"
syn match Real "[-+]?\d\+\.?([eE][+-]?)?\d*"
syn match Character "\'[a-zA-Z0-9]\'"
syn match String "\"[.\n\r]*\""
syn match Boolean "true|false"
syn match Symbol "[.]*"

syn region Array start='\[' end='\]'
syn region Sequence start='<' end='>'

syn region SExp start='(' end=')' fold transparent
syn region SingleComment start='!' end='\n'
syn region MultiComment start='!-' end='-!'

syn keyword Core def set ret add fadd sub fsub mul fmul udiv sdiv urem srem frem icmp fcmp begin if flow construct access array nth call allocate store load main LLVM C C++ Objective-C Objective-C++ function recursive fast inline inline-recursive inline-fast declare type structure generic word-macro macro full-macro pointer unpointer typeof

syn match integerType "i[0-9]+"
syn keyword Type half float double x86_fp80 fp128 ppc_fp128 bool char short int long

let b:current_syntax = "hylas"

hi def link Integer         Number
hi def link Real            Float
hi def link Character       Character
hi def link String          String
hi def link Boolean         Boolean
hi def link Symbol          Identifier
hi def link SingleComment   Comment
hi def link MultiComment    Comment
hi def link Core            Function
hi def link integerType     Type
hi def link Type            Type
