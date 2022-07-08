" Syntax Highlight for Tiny Programming Language.
" Author: André G. (https://github.com/Andre0n).
if exists("b:current_syntax")
    finish
endif
let b:current_syntax = "tiny"
syntax keyword tinyKeyword  while repeat if then else until  endif endwhile
syntax keyword tinyFunction write read
syntax match tinyId  "[a-z][_\w]*"
syntax match tinyNum "\d"
syntax match tinyComment "{\_.\{-}}"
highlight link tinyKeyword Keyword
highlight link tinyFunction Function
highlight link tinyId  Constant
highlight link tinyComment Comment
highlight link tinyNum Number
