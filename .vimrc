" Point the last line before termination
au BufRead *
			\ if line("'\"") > 0 && line("'\"") <= line("$") |
			\    exe "norm g'\"" |
			\ endif
" vmap paste and delete
vmap r "_dP
" vmap copy
vmap "y "+y
vmap <C-c> "+y
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
"set binary
"set noeol
" file tpye plugins are enabled
filetype plugin on
" Respect modeline in files
set modeline
set modelines=4
" Enable line numbers
set number

" Enable syntax highlighting
syntax on
" syntax highlighting from start
autocmd BufEnter * :syntax sync fromstart
" Syntax highlighting: OpenCL
au BufRead,BufNewFile *.cl set filetype=opencl
call tcomment#DefineType('opencl', '// %s')
" Syntax highlighting: Custom data
au BufRead,BufNewFile *.dat set filetype=dat
call tcomment#DefineType('dat', '# %s')
" llvm syntax highlighting (asm)
  augroup filetype
    au! BufRead,BufNewFile *.s     set filetype=llvm
  augroup END
" llvm makefile syntax highlighting
  augroup filetype
    au! BufRead,BufNewFile *Makefile*     set filetype=make
  augroup END
" Highlight current line
set cursorline

" Make tabs as wide as two spaces
" set tabstop=2
set softtabstop=2
set expandtab
set sw=2
" Show “invisible” characters
" set lcs=tab:▸\ ,trail:·,eol:¬,nbsp:_
" set lcs=tab:▸\
" set list
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

" Save a file as root (,W)
noremap <leader>W :w !sudo tee % > /dev/null<CR>

" Automatic commands
if has("autocmd")
  " Enable file type detection
  filetype on
  " Treat .json files as .js
  autocmd BufNewFile,BufRead *.json setfiletype json syntax=javascript
endif

" Ctags for gpgpusim
" set tags+=/home/younghwan/gpgpu-sim/v3.x/libcuda/tags
" set complete

" Cscope for gpgpusim
" set csprg=/usr/bin/cscope
" set cst " use cscope
" if filereadable("/home/younghwan/gpgpu-sim/v3.x/src/cscope.out")
"   cs add /home/younghwan/gpgpu-sim/v3.x/src/cscope.out
"   endif
