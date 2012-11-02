" Vim syntax file
" Language:   Eureka
" Maintainer: Joe Drago <joedrago@gmail.com>
" URL:        http://www.github.com/joedrago/eureka
" This is basically a doctored Javascript syntax file.

if !exists("main_syntax")
  if version < 600
    syntax clear
  elseif exists("b:current_syntax")
    finish
  endif
  let main_syntax = 'eureka'
endif

" Drop fold if it set but vim doesn't support it.
if version < 600 && exists("eureka_fold")
  unlet eureka_fold
endif

syn keyword eurekaCommentTodo      TODO FIXME XXX TBD contained
syn match   eurekaLineComment      "\/\/.*" contains=@Spell,eurekaCommentTodo
syn match   eurekaLineComment      "#.*" contains=@Spell,eurekaCommentTodo
syn match   eurekaCommentSkip      "^[ \t]*\*\($\|[ \t]\+\)"
syn region  eurekaComment          start="/\*"  end="\*/" contains=@Spell,eurekaCommentTodo
syn match   eurekaSpecial          "\\\d\d\d\|\\."
syn region  eurekaStringD          start=+"+  skip=+\\\\\|\\"+  end=+"\|$+    contains=eurekaSpecial,@htmlPreproc
syn region  eurekaStringS          start=+'+  skip=+\\\\\|\\'+  end=+'\|$+    contains=eurekaSpecial,@htmlPreproc

syn match   eurekaSpecialCharacter "'\\.'"
syn match   eurekaNumber           "-\=\<\d\+L\=\>\|0[xX][0-9a-fA-F]\+\>"
syn region  eurekaRegexpString     start=+/[^/*]+me=e-1 skip=+\\\\\|\\/+ end=+/[gi]\{0,2\}\s*$+ end=+/[gi]\{0,2\}\s*[;.,)\]}]+me=e-1 contains=@htmlPreproc oneline

syn keyword eurekaConditional      if else
syn keyword eurekaRepeat           while for in
syn keyword eurekaBranch           break continue
syn keyword eurekaOperator         inherits cmp and or not
syn keyword eurekaStatement        return
syn keyword eurekaBoolean          true false
syn keyword eurekaNull             null
syn keyword eurekaIdentifier       this var
syn keyword eurekaMessage          print
syn keyword eurekaReserved         object dict inherit prototype string int float array length push keys eval type import

if exists("eureka_fold")
    syn match  eurekaFunction      "\<function\>"
    syn region eurekaFunctionFold  start="\<function\>.*[^};]$" end="^\z1}.*$" transparent fold keepend

    syn sync match eurekaSync grouphere eurekaFunctionFold "\<function\>"
    syn sync match eurekaSync grouphere NONE "^}"

    setlocal foldmethod=syntax
    setlocal foldtext=getline(v:foldstart)
else
    syn keyword eurekaFunction    function
    syn match   eurekaBraces      "[{}\[\]]"
    syn match   eurekaParens      "[()]"
endif

syn sync fromstart
syn sync maxlines=100

if main_syntax == "eureka"
  syn sync ccomment eurekaComment
endif

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_eureka_syn_inits")
  if version < 508
    let did_eureka_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif
  HiLink eurekaComment          Comment
  HiLink eurekaLineComment      Comment
  HiLink eurekaCommentTodo      Todo
  HiLink eurekaSpecial          Special
  HiLink eurekaStringS          String
  HiLink eurekaStringD          String
  HiLink eurekaCharacter        Character
  HiLink eurekaSpecialCharacter eurekaSpecial
  HiLink eurekaNumber           eurekaValue
  HiLink eurekaConditional      Conditional
  HiLink eurekaRepeat           Repeat
  HiLink eurekaBranch           Conditional
  HiLink eurekaOperator         Operator
  HiLink eurekaStatement        Statement
  HiLink eurekaFunction         Function
  HiLink eurekaBraces           Function
  HiLink eurekaError            Error
  HiLink ParenError             eurekaError
  HiLink eurekaNull             Keyword
  HiLink eurekaBoolean          Boolean
  HiLink eurekaRegexpString     String

  HiLink eurekaIdentifier       Identifier
  HiLink eurekaMessage          Keyword
  HiLink eurekaReserved         Keyword
  HiLink eurekaDebug            Debug
  HiLink eurekaConstant         Label

  delcommand HiLink
endif

let b:current_syntax = "eureka"
if main_syntax == 'eureka'
  unlet main_syntax
endif

" vim: ts=8
