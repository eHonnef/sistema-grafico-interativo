# Sistema Gráfico Interativo

Sistema criado para a disciplina UFSC/INE5420-05208.  
Importado do GitLab. [LINK](https://gitlab.com/eHonnef/sistema-grafico-interativo/)

## Pré-requisitos

Para Linux basta instalar os pacotes via pacman, apt-get, yum, etc.

1. G++, Clang, GCC, CMake e Make.
    1. Opcional: Glade para desenhar a janela principal.
1. Bibliotecas do GTK+3.0 e GTKMM-3.0.
    1. Tutoriais para Windows:
        1. [Instalador GTK](http://www.tarnyko.net/dl/gtk.htm)
        1. [Instalador GTKMM](https://github.com/codekiddy2/Visual-Studio-gtkmm)
        1. [Usar GTK no visual studio](https://stackoverflow.com/questions/15906580/how-to-configure-gtk-on-visual-studio-2010)

Obs.: No Windows não tentei rodar o "acelerador" de compilação que é usado no Unix/Linux (sem ele leva cerca de 30 segundos para compilar o projeto).  
Módulo CMake (acelerador): [LINK](https://github.com/sakra/cotire)

## Compilar e rodar

### Linux / Unix

1. Abra o terminal e navegue ate a pasta `./Build`.
1. Rode o comando `cmake -j n CMakeLists.txt`, onde n é o número de threads (para acelerar o processo), este passo só é necessário uma vez.
1. Compile o código pelo comando `make -j n`, n é o número de threads.
1. Execute o programa pelo comando `./exe`.
    1. Se quiser liberar o terminal `nohup ./exe &!`

### Windows

1. Sorte.
1. O GTK e GTKMM devem estar instalados em `C:/gtkmm` e `C:/gtk`.
    1. Se não mude os locais de `include` e `lib` da solution.
1. Abra a solution e compile (não tentei usar o CMake no Windows).

Links possivelmente úteis:

- [Compiling a GTK3 program in Visual Studio 2012](https://stackoverflow.com/questions/20883854/compiling-a-gtk3-program-in-visual-studio-2012)
- [Error 3 error LNK1104: cannot open file 'gtk-3.lib'](https://stackoverflow.com/questions/20128380/error3error-lnk1104-cannot-open-file-gtk-3-lib?lq=1)
- [How to configure gtk on Visual studio 2010](https://stackoverflow.com/questions/15906580/how-to-configure-gtk-on-visual-studio-2010)
