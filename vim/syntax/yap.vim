" Vim syntax file
" Language:   Yap
" Maintainer: Joe Drago <joedrago@gmail.com>
" URL:        http://www.github.com/joedrago/yap
" This is basically a doctored yap syntax file.

if !exists("main_syntax")
  if version < 600
    syntax clear
  elseif exists("b:current_syntax")
    finish
  endif
  let main_syntax = 'yap'
endif

" Drop fold if it set but vim doesn't support it.
if version < 600 && exists("yap_fold")
  unlet yap_fold
endif

syn keyword yapCommentTodo      TODO FIXME XXX TBD contained
syn match   yapLineComment      "\/\/.*" contains=@Spell,yapCommentTodo
syn match   yapLineComment      "#.*" contains=@Spell,yapCommentTodo
syn match   yapCommentSkip      "^[ \t]*\*\($\|[ \t]\+\)"
syn region  yapComment          start="/\*"  end="\*/" contains=@Spell,yapCommentTodo
syn match   yapSpecial          "\\\d\d\d\|\\."
syn region  yapStringD          start=+"+  skip=+\\\\\|\\"+  end=+"\|$+    contains=yapSpecial,@htmlPreproc
syn region  yapStringS          start=+'+  skip=+\\\\\|\\'+  end=+'\|$+    contains=yapSpecial,@htmlPreproc

syn match   yapSpecialCharacter "'\\.'"
syn match   yapNumber           "-\=\<\d\+L\=\>\|0[xX][0-9a-fA-F]\+\>"
syn region  yapRegexpString     start=+/[^/*]+me=e-1 skip=+\\\\\|\\/+ end=+/[gi]\{0,2\}\s*$+ end=+/[gi]\{0,2\}\s*[;.,)\]}]+me=e-1 contains=@htmlPreproc oneline

syn keyword yapConditional      if else
syn keyword yapRepeat           while for in
syn keyword yapBranch           break continue
syn keyword yapOperator         inherits cmp and or not
syn keyword yapStatement        return
syn keyword yapBoolean          true false
syn keyword yapNull             null
syn keyword yapIdentifier       this var
syn keyword yapMessage          print
syn keyword yapReserved         object dict inherit prototype string int float array length push keys eval type import

if exists("yap_fold")
    syn match  yapFunction      "\<function\>"
    syn region yapFunctionFold  start="\<function\>.*[^};]$" end="^\z1}.*$" transparent fold keepend

    syn sync match yapSync grouphere yapFunctionFold "\<function\>"
    syn sync match yapSync grouphere NONE "^}"

    setlocal foldmethod=syntax
    setlocal foldtext=getline(v:foldstart)
else
    syn keyword yapFunction    function
    syn match   yapBraces      "[{}\[\]]"
    syn match   yapParens      "[()]"
endif

syn sync fromstart
syn sync maxlines=100

if main_syntax == "yap"
  syn sync ccomment yapComment
endif

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_yap_syn_inits")
  if version < 508
    let did_yap_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif
  HiLink yapComment          Comment
  HiLink yapLineComment      Comment
  HiLink yapCommentTodo      Todo
  HiLink yapSpecial          Special
  HiLink yapStringS          String
  HiLink yapStringD          String
  HiLink yapCharacter        Character
  HiLink yapSpecialCharacter yapSpecial
  HiLink yapNumber           yapValue
  HiLink yapConditional      Conditional
  HiLink yapRepeat           Repeat
  HiLink yapBranch           Conditional
  HiLink yapOperator         Operator
  HiLink yapStatement        Statement
  HiLink yapFunction         Function
  HiLink yapBraces           Function
  HiLink yapError            Error
  HiLink javaScrParenError   yapError
  HiLink yapNull             Keyword
  HiLink yapBoolean          Boolean
  HiLink yapRegexpString     String

  HiLink yapIdentifier       Identifier
  HiLink yapMessage          Keyword
  HiLink yapReserved         Keyword
  HiLink yapDebug            Debug
  HiLink yapConstant         Label

  delcommand HiLink
endif

let b:current_syntax = "yap"
if main_syntax == 'yap'
  unlet main_syntax
endif

" vim: ts=8
