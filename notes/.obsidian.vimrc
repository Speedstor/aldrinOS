syntax on
set tabstop=2
set shiftwidth=2
set expandtab
set ai
set number
set hlsearch
set ruler
highlight Comment ctermfg=green

filetype plugin indent on
" show existing tab with 4 spaces width
set tabstop=4
" when indenting with '>', use 4 spaces width
set shiftwidth=4
" On pressing tab, insert 4 spaces
set expandtab

set pastetoggle=<F3>
" Use system clipboard by default
set clipboard=unnamed

inoremap <C-w> <C-BS>

nnoremap yy "+yy
nnoremap yw "+yw
nnoremap yl "+yl
nnoremap yp "+yp
nnoremap y{ "+y{
nnoremap ye "+ye
nnoremap yb "+yb
nnoremap yH "+yH
nnoremap yM "+yM
nnoremap yL "+yL
vnoremap y "+y

noremap : :set wildmode=longest,full<CR>:
inoremap <C-n> <Esc>:set wildmode=longest,list,full<CR>a<C-n>

set cursorline

nnoremap S :%s//g<Left><Left>

" set noerrorbells
set colorcolumn=80

" Search through buffers for words
funct! GallFunction(re)
      cexpr []
        execute 'silent! noautocmd bufdo vimgrepadd /' . a:re . '/j %'
          cw  
      endfunct

      command! -nargs=1 Gall call GallFunction(<q-args>)

silent! source $VIMRUNTIME/defaults.vim



