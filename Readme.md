
## Lab 02 Programación VGA

## Introducción:

Elaboración de un driver VGA, utilizando el sistema operativo didáctico xv6. 

## Modularización: 

todo el desarrollo del driver en el archivo vga.{c,h} para una correcta modularización.

### Descripición de módulos

* **vga.c:** Es el encargado de tener todas las implemetaciónes para poder ejecutar las syscall's nuevas.

### Compilación

xv6 posee un archivo Makefile para compilar, que fue editado para que todo lo agregado sea compilado por el mismo.

1. Para compilar y ejecutar qemu.

> make qemu

2. Para borrar los archivos generados por la compilación.

> make clean

### Herramientas de Desarrollo

Se utilizó Visual Studio Code para su desarrollo.

## Conclusiones.

Se obtuvo un driver vga para el sistema operativo didáctico xv6 que cuenta con las siguientes funcionalidades:

* Imprime al pie de pantalla "SO2021" al iniciar el sistema.
* Nos permite cambiar de modo texto a modo grafico a través de la syscall `modeswitch`, siendo 0 modo texto y siendo 1 modo gráfico.
* Nos permite cambiar de modo texto a modo grafico y no perder las fuentes.
* Nos permite pintar un pixel a través de la syscall `plotpixel`.
* Nos permite pintar un rectángulo a través de la syscall `plotrectangle`.
* Nos permite ejecutar un programa de usuario que dibuja por pantalla.
* Posee una modularización delicada.
* Posee una paleta de 256 colores.


## Bibliografía Complementaria

http://www.sromero.org/ext/articulos/modox/modox1.html

> Donde hay páginas de modox1 hasta modox6.

http://www.brackeen.com/vga/basics.html

https://cs3210.cc.gatech.edu/r/xv6-rev9-book.pdf

https://github.com/sam46/xv6

https://www.cs.ucr.edu/~csong/cs153/20f/lab0.html
