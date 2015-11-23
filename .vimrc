" " Vundle start =============================================================================
" set nocompatible              " be iMproved, required
" filetype off                  " required
"
" " set the runtime path to include Vundle and initialize
" set rtp+=~/.vim/bundle/Vundle.vim
" call vundle#begin()
"
" " let Vundle manage Vundle, required
" Plugin 'VundleVim/Vundle.vim'
"
" " All of your Plugins must be added before the following line
" call vundle#end()            " required
" filetype plugin indent on    " required
"
" " Brief help
" " :PluginList       - lists configured plugins
" " :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" " :PluginSearch foo - searches for foo; append `!` to refresh local cache
" " :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" " Vundle end ===============================================================================

" Toolset start ============================================================================
" Code completion
cnoreabbrev t NeoCompleteToggle 
let g:neocomplete#enable_at_startup = 0
let g:neocomplete#enable_auto_select = 1
let g:neocomplete#auto_completion_start_length = 3
inoremap <expr><C-g>  neocomplete#undo_completion()
inoremap <expr><TAB>  pumvisible() ? "\<C-n>" : "\<TAB>"
autocmd FileType html,markdown setlocal omnifunc=htmlcomplete#CompleteTags
autocmd FileType javascript setlocal omnifunc=javascriptcomplete#CompleteJS
autocmd FileType python setlocal omnifunc=pythoncomplete#Complete


" Toolset end =============================================================================

" Point the last line before termination
au BufRead *
			\ if line("'\"") > 0 && line("'\"") <= line("$") |
			\    exe "norm g'\"" |
			\ endif
" vmap paste and delete
vmap r "_dP
" vmap copy
vmap "y "+y
" vmap remote copy: vis plugin is needed to yank selected only
" Download plugin @ http://vim.sourceforge.net/scripts/script.php?script_id=1195
" vmap <C-c> :B w !cb<CR><CR>
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
set binary
set noeol
" file tpye plugins are enabled
filetype plugin on
" Respect modeline in files
set modeline
set modelines=4
" Enable line numbers
set number

" Enable syntax highlighting ============================================================
syntax on
autocmd BufEnter * :syntax sync fromstart
" Syntax highlighting with tcomment
augroup filetype
  au! BufRead,BufNewFile *.perl              set filetype=perl
  au! BufRead,BufNewFile *.s                 set filetype=llvm
  au! BufRead,BufNewFile *Makefile*          set filetype=make
  au! BufRead,BufNewFile *.{dat,csv,log}     set filetype=dat
  au! BufRead,BufNewFile *.cl                set filetype=opencl
  au! BufRead,BufNewFile *.tex               set filetype=tex
augroup END
call tcomment#DefineType('opencl', '// %s')
call tcomment#DefineType('dat', '# %s')
" Automatic commands
if has("autocmd")
  " Enable file type detection
  filetype on
  " Treat .json files as .js
  autocmd BufNewFile,BufRead *.json setfiletype json syntax=javascript
endif
" Syntax highlighting end ==============================================================

" Highlight current line
set cursorline

" Make tabs as wide as two spaces
set tabstop=2
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
