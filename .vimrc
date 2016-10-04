" Default tab settings
set tabstop=2
set softtabstop=2
set sw=2
set expandtab

" Show invisible marks
" set lcs=tab:▸\ ,trail:·,eol:¬,nbsp:_
" set lcs=tab:▸\ ,
" set list

" Enable syntax highlighting ============================================================
syntax on
autocmd BufEnter * :syntax sync fromstart
" Syntax highlighting with tcomment
augroup filetype
  au! BufRead,BufNewFile *.python.*            set filetype=python
  au! BufRead,BufNewFile *.perl.*              set filetype=perl
  au! BufRead,BufNewFile *.bin.*               set filetype=bin
  au! BufRead,BufNewFile *.s.*                 set filetype=llvm
  au! BufRead,BufNewFile *Makefile*            set filetype=make
  au! BufRead,BufNewFile *.{dat,csv,log}.*     set filetype=dat
  au! BufRead,BufNewFile *.{dat,csv,log}       set filetype=dat
  au! BufRead,BufNewFile *.prototxt            set filetype=prototxt
  au! BufRead,BufNewFile *.cl.*                set filetype=opencl
  au! BufRead,BufNewFile *.gp.*                set filetype=gnuplot
  au! BufRead,BufNewFile *.tex.*               set filetype=tex
  au! BufRead,BufNewFile *.coffee.*            set filetype=coffee
  au! BufNewFile,BufRead *.json.*              set filetype=javascript
augroup END
call tcomment#DefineType('opencl', '// %s')
call tcomment#DefineType('dat', '# %s')
call tcomment#DefineType('prototxt', '# %s')
call tcomment#DefineType('gp', '# %s')

" file tpye plugins are enabled
filetype plugin on

" Syntax highlighting end ===========================================================

" Map key to toggle something =======================================================
function MapToggle(key, opt)
  " Only allows this on visual mode, not for insert
  let cmd = ':set '.a:opt.'! \| set '.a:opt."?\<CR>"
  exec 'nnoremap '.a:key.' '.cmd
  exec 'vnoremap '.a:key." \<C-O>".cmd
endfunction
command -nargs=+ MapToggle call MapToggle(<f-args>)

" number toggle
MapToggle '' nonumber

" ==================================================================================
" vmap paste and delete
vmap r "_dP
" vmap copy
vmap "y "+y
" vmap remote copy: vis plugin is needed to yank selected visual space.
" Download the plugin @ http://vim.sourceforge.net/scripts/script.php?script_id=1195
let host=$SSH_HOME
if host != ''
  vmap <C-c> :B w !rcb<CR><CR>
else
  vmap <C-c> :B w !cb<CR><CR>
endif
" use 256 bit color
let &t_Co=256
" disable background color erase
set t_ut=
" font size
set gfn=Monaco:h12
" Colorscheme
colorscheme molokai
" Use the OS clipboard by default (on versions compiled with `+clipboard`)
set clipboard=unnamed
" Enhance command-line completion
set wildmenu
" Allow cursor keys in insert mode
set esckeys
" Allow backspace in insert mode
set backspace=indent,eol,start
" Add the g flag to search/replace by default
set gdefault
" Use UTF-8 without BOM
set encoding=utf-8 nobomb
" Change mapleader
let mapleader=","
" Don’t add empty newlines at the end of files
set noeol
" Respect modeline in files
set modeline
set modelines=4
" Enable line numbers
set number

" To be sane paste behavior from GUI to Term
let &t_SI .= "\<Esc>[?2004h"
let &t_EI .= "\<Esc>[?2004l"
inoremap <special> <expr> <Esc>[200~ XTermPasteBegin()

function! XTermPasteBegin()
  set pastetoggle=<Esc>[201~
  set paste
  return ""
endfunction

" Highlight current line
set cursorline
" Highlight searches
set hlsearch
" Ignore case of searches
set ignorecase
" Highlight dynamically as pattern is typed
" set incsearch
" Always show status line
set laststatus=2
" Enable mouse in all modes
set mouse=a
" Disable error bells
set noerrorbells
" Don’t reset cursor to start of line when moving around.
set nostartofline
" Don’t show the intro message when starting Vim
set shortmess=atI
" Show the filename in the window titlebar
set title
" Show the (partial) command as it’s being typed
set showcmd
" Start scrolling three lines before the horizontal window border
set scrolloff=3
" Point the last line before termination
au BufRead *
  \ if line("'\"") > 0 && line("'\"") <= line("$") |
  \    exe "norm g'\"" |
  \ endif
