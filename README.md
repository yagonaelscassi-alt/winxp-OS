# WinXP-OS

Sistema operativo experimental en C con aspecto de Windows XP (cielo, colina verde, barra de tareas azul, botón Inicio verde, ventana). Kernel de 32 bits, arranque Multiboot con GRUB, gráficos por framebuffer lineal 800x600x32.

Es un proyecto original e independiente; no usa código de Microsoft.

## Requisitos (Debian 9 "stretch")

```sh
sudo apt-get install build-essential gcc-multilib grub-pc-bin xorriso mtools qemu-system-x86 git
```

## Compilar y probar

```sh
make          # genera build/winxp.iso
make check    # verifica la cabecera multiboot
make run      # abre la ISO en QEMU
```

## Estructura

- `src/boot.S`: cabecera Multiboot (pide modo gráfico) y punto de entrada.
- `src/kernel.c`: dibujo del escritorio (fondo, iconos, ventana, barra de tareas) y fuente 8x8.
- `linker.ld`: mapa de memoria (kernel en 1 MiB).
- `grub.cfg`: entrada de arranque para la ISO.

## Hoja de ruta

1. GDT/IDT e interrupciones
2. Driver de teclado y mouse PS/2 (cursor)
3. Menú Inicio interactivo y ventanas movibles
4. Reloj (RTC/PIT) en la bandeja del sistema
5. Gestor de memoria y sistema de archivos simple

## Subir a GitHub

```sh
git remote add origin https://github.com/TU_USUARIO/winxp-os.git
git branch -M main
git push -u origin main
```
